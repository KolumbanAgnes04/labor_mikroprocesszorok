/*
 * motor.c
 *
 *  Created on: 2019. nov. 22.
 *      Author: girgaszpeter
 */

#include "motor.h"
#include "tim.h"


int32_t set_speed_percent;
uint16_t Motor_PWMDuty;
uint8_t encoder_state;
int32_t encoder_position_act;
int32_t encoder_position_prev;
int32_t enc_error;
float encoder_speed_rpm;
float output_speed_rpm;

/*
 * Macro to check if the received parameter is a valid
 * running direction of the motor.
 */
#define IS_MOTOR_DIR(DIR) (((DIR) == Motor_DIR_FORWARD) || \
						   ((DIR) == Motor_DIR_REVERSE))

/*
 * Initial values for the global variables
 * of the motor driver library.
 */
void Motor_init(void)
{
	enc_error = 0;
	Motor_PWMDuty = 0;
	encoder_state = 0;
	encoder_position_act = 0;
	__HAL_TIM_ENABLE_IT(&htim10, TIM_IT_UPDATE );
}

void Motor_setPWMDuty(uint16_t value){
	TIM_OC_InitTypeDef sConfigOC = {0};

	// Fill the initStruct of the timer, and set the mode
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = value;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
		Error_Handler();
	}

	// Start the generation of the PWM
	HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
}

void Motor_setPWMDutyPercent(float value) {
	uint32_t duty;

	/*
	 * Calculating the new value for the Capture-compare
	 * register. Timer14 has 16 bit CC register, so
	 * the value should be between 0x0000 (0) and 0xFFFF
	 * (65565). 0 for 0%, 65565 for 100% duty cycle.
	 */

	if(value > 0)
	{
		Motor_setDirection(Motor_DIR_FORWARD);
		if(value > 100)
		{
			value = 100;
		}
		duty = 65535 * (value/100);
	}
	else
	{
		Motor_setDirection(Motor_DIR_REVERSE);
		if(value < -100)
		{
			value = -100;
		}
		duty = 65535 * (-value/100);
	}



	Motor_setPWMDuty(duty);
}

void Motor_setDirection(uint8_t dir) {
	if(dir == Motor_DIR_FORWARD){
		HAL_GPIO_WritePin(MOTOR_DIR_GPIO_Port, MOTOR_DIR_Pin, 0); //DIR pin LOW for forward rotation
	}
	else if(dir == Motor_DIR_REVERSE) {
		HAL_GPIO_WritePin(MOTOR_DIR_GPIO_Port, MOTOR_DIR_Pin, 1); //DIR pin HIGH for reverse rotation
	}
}

void updateEncoderPosition(uint8_t phaseA, uint8_t phaseB) {
	/*
	 * Copy the latest encoder state to a temporary
	 * variable, keep only the two last bits.
	 */
	uint8_t s = encoder_state & 3;

	/*
	 * Put the new values of the encoder phases to
	 * the temporary variable
	 */
	if (phaseA) s |= 4;
	if (phaseB) s |= 8;

	/*
	 * The s variable holds the latest and the new
	 * state of the encoder channels state.
	 *
	 * Case New B  New A Last B Last A
	 * 0:       0      0      0      0
	 * 1:       0      0      0      1
	 * 2:       0      0      1      0
	 * 3:       0      0      1      1
	 * 4:       0      1      0      0
	 * 5:       0      1      0      1
	 * 6:       0      1      1      0
	 * 7:       0      1      1      1
	 * 8:       1      0      0      0
	 * 9:       1      0      0      1
	 * 10:      1      0      1      0
	 * 11:      1      0      1      1
	 * 12:      1      1      0      0
	 * 13:      1      1      0      1
	 * 14:      1      1      1      0
	 * 15:      1      1      1      1
	 *
	 */
	switch (s) {
		/*
		 * If the new values equal to the last values,
		 * then nothing changed.
		 */
		case 0: case 5: case 10: case 15:
			break;
		/*
		 * If A channel has a falling edge while B is low,
		 * or B channel has a falling edge while A is high,
		 * or A channel has a rising edge while B is high,
		 * or B channel has a rising edge while A is low,
		 * then the encoder is turning in the positive
		 * direction, so increase the encoder_position variable
		 */
		case 1: case 7: case 8: case 14:
			encoder_position_act++; break;
		/*
		 * If A channel has a rising edge while B is low,
		 * or B channel has a rising edge while A is high,
		 * or A channel has a falling edge while B is high,
		 * or B channel has a falling edge while A is low,
		 * then the encoder is turning in the negative
		 * direction, so decrease the encoder_position variable
		 */
		case 2: case 4: case 11: case 13:
			encoder_position_act--; break;
		/*
		 * If both A and B channels value change in one step,
		 * then minimum one edge transition was missed. Error
		 * counter should be increased
		 */
		default:
			enc_error++; break;
	}
	/*
	 * Put the encoder channels state in the encoder_state
	 * global variable for the next evaluation of this interrupt
	 * service routine.
	 */
	encoder_state = (s >> 2);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/*
	 * Check if the interrupt request was made by either one
	 * of the encoder pins
	 */
    if ( GPIO_Pin == ENC_PHASE_A_Pin || GPIO_Pin == ENC_PHASE_B_Pin)
    {
    	// If so, then evaluate the encoder_state updater function.
    	updateEncoderPosition(
    		HAL_GPIO_ReadPin(ENC_PHASE_A_GPIO_Port, ENC_PHASE_A_Pin),
    		HAL_GPIO_ReadPin(ENC_PHASE_B_GPIO_Port, ENC_PHASE_B_Pin)
    	);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim10)
	{
		/* Calculate change of encoder position */
		int32_t position_difference = encoder_position_act - encoder_position_prev;
		encoder_position_prev = encoder_position_act;

		float frequency = (float) SystemCoreClock / ((float) (htim->Init.Period + 1) * (float) (htim->Init.Prescaler + 1));
		float speed = (float) position_difference * frequency;
		speed /= MOTOR_ENCODER_RESOLUTION * MOTOR_ENCODER_STEPS;

		encoder_speed_rpm = speed * 60;
		output_speed_rpm = encoder_speed_rpm / MOTOR_GEAR_RATIO;

		/*
		 * Re-enable the interrupt after evaluating the controller
		 * function.
		 */
		HAL_TIM_Base_Start_IT(htim);
	}
}
