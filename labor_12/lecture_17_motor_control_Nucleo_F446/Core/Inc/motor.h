/*
 * motor.h
 *
 *  Created on: 2019. nov. 22.
 *      Author: girgaszpeter
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "main.h"

#define MOTOR_SUPPLY_VOLTAGE		5.0F
#define MOTOR_KV					3000
/*
 * Constants for motor encoder resolution. Means the number
 * of encoder pulses per a complete motor rotation.
 */
#define MOTOR_ENCODER_RESOLUTION	16U

/*
 * Constants for motor encoder evaluation. Means the number
 * of encoder pulses per an encoder step.
 */
#define MOTOR_ENCODER_STEPS			2U

/*
 * Constants for motor gear reduction ratio. Means the number
 * of motor rotations per output shaft rotation.
 */
#define MOTOR_GEAR_RATIO			120U

/*
 * Constants for defining the running direction of the motor.
 */
#define Motor_DIR_FORWARD 0x00
#define Motor_DIR_REVERSE 0x01

/*
 * The required percent of maximum speed.
 */
extern int32_t set_speed_percent;

/*
 * The previous state of the HMI switches.
 */
extern uint8_t switch_state_prev;

/*
 * The PWM duty of the motor. This variable is used to store
 * the duty cycle when switching between operating modes of
 * the motor driver IC, because changing operating mode resets
 * the duty cycle to 0. Value is between 0x0000 and 0xFFFF.
 */
extern uint16_t Motor_PWMDuty;

/*
 * The latest state of the encoders inputs. It's needed for the
 * evaluation of the new input states in the interrupt service
 * routine. Values is one of 0b0000, 0b0001, 0b0010, 0b0011.
 * The LSB stores Phase A value, the second bit stores the
 * Phase B value.
 */
extern uint8_t encoder_state;

/*
 * The actual position of the encoder.
 */
extern int32_t encoder_position_act;

/*
 * The previous position of the encoder.
 */
extern int32_t encoder_position_prev;

/*
 * Encoder error counter
 */
extern int32_t enc_error;

/*
 * Speed of the encoder in RPM.
 */
extern float encoder_speed_rpm;

/*
 * Speed of the output shaft in RPM.
 */
extern float output_speed_rpm;

/*
 * @brief Initialize global variables of motor driver.
 *
 * @param none
 */
void Motor_init(void);

/*
 * @brief Set the duty cycle of the PWM output.
 *        The CCR of the used timer (TIM14) will
 *        be set accoring to the value parameter. Can be
 *        between 0x0000 and 0xFFFF.
 *
 * @param The new CCR value.
 */
void Motor_setPWMDuty(uint16_t value);

/*
 * @brief Set the duty cycle in percent of the
 *        PWM output. The CCR of the used timer(TIM14
 *        will be set according to the value parameter.
 *        The value can be between -100.0f and 100.0f
 *
 * @param The new duty cycle of the PWM.
 */
void Motor_setPWMDutyPercent(float value);


/*
 * @brief Set the RPM of the motor
 *
 * @param The new speed of the motor in RPM.
 */
void Motor_setMotorSpeedRPM(float speed);

/*
 * @brief Set the RPM of the output shaft
 *
 * @param The new speed of the output in RPM.
 */
void Motor_setOutputSpeedRPM(float speed);

/*
 * @brief Set the running direction of the motor.
 *        Can be one of MOTOR_DIR_FORWARD
 *        or MOTOR_DIR_REVERSE
 *
 * @param The new running direction.
 */
void Motor_setDirection(uint8_t dir);

/*
 * @brief Shorthand for running the motor in forward direction with
 *        the maximum speed.
 */
void Motor_runForward();

/*
 * @brief Shorthand for running the motor in reverse direction with
 *        the maximum speed.
 */
void Motor_runReverse();


/*
 * @brief Shorthand for stopping the motor.
 */
void Motor_stop();

void updateEncoderPosition(uint8_t phaseA, uint8_t phaseB);

/*
 * @brief This interrupt service routine is called by the HAL library
 *        when there's a change of any of the EXTI5:9 lines.
 *
 * @param The GPIO pin wich caused the interrupt
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/*
 * @brief A simple proportional controller implementation.
 *        This P-type controller will turn the motors shaft
 *        to the same position the potentiometers control
 *        shaft is turned into. The potentiometer that comes
 *        with the board turns 300 degrees, so the motors shaft
 *        also turns the maximum of 300 degrees on the geared
 *        side, and 300*120=36000 = 100 full turns on the
 *        non-geared side, because of the 1:120 gear ratio.
 */
void runProportionalController();

/*
 * @brief Callback for the TIM10s overflow update
 *
 * @param The instance of the timer that made the interrupt
 *
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* MOTOR_H_ */
