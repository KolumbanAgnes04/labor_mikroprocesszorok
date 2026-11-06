/*
 * humidity_sensor.c
 */

#include "main.h"
#include "i2c.h"
#include "humidity_sensor.h"
#include <math.h>

// Mind a páratartalom, mind a hőmérséklet értékének kiolvasása
HAL_StatusTypeDef HumTempSensor_GetValues(float* humidity, float* temperature)
{
	uint8_t receive_buffer[4];
	HAL_StatusTypeDef stat_tx, stat_rx;

	uint16_t humidity_reg;
	uint16_t temperature_reg;

	/*
	 * Mérési kérelem küldése a szenzornak, aztán annyi ms-ig
	 * történő várakozás a mérés befejeztére, amely a TempHum_Sensor_MEASURE_DELAY
	 * konstansban került megadásra, majd az érték kiolvasása.
	 */
	stat_tx = HAL_I2C_Master_Transmit(&hi2c1, TempHum_Sensor_ADDRESS, NULL, 0, TempHum_Sensor_TIMEOUT);
	HAL_Delay(TempHum_Sensor_MEASURE_DELAY);
	stat_rx = HAL_I2C_Master_Receive(&hi2c1, TempHum_Sensor_ADDRESS, receive_buffer, 4, TempHum_Sensor_TIMEOUT);

	/*
	 * Ha az állapot indikátorok nem S1 = 0 és S0 = 0, akkor
	 * a mérés nem volt sikeres.
	 */
	if((receive_buffer[0] & 0xC0) != 0) return HAL_ERROR;
	receive_buffer[0] &= 0x3F;

	/*
	 * A páratartalom és hőmérséklet értékek maszkolása a kapott adatbájtokból
	 */
	humidity_reg = (receive_buffer[0]<<8) + receive_buffer[1];
	temperature_reg = ((receive_buffer[2]<<8) + receive_buffer[3])>>2;

	/*
	 * Az aktuális páratartalom (%) és hőmérséklet (°C)
	 * számítása az adatlapban megadott képletek alapján.
	 *
	 * Megtalálhatóak a 6. oldalon a 2. táblázat alatt.
	 */
	if(humidity != NULL) *humidity = (((float)humidity_reg)/((float)pow(2,14)-2))*100;
	if(temperature != NULL) *temperature = ((((float)temperature_reg)/((float)pow(2,14)-2))*165) - 40;

	return (stat_tx == HAL_OK && stat_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}

// Mért hőmérsékletérték kiolvasása
HAL_StatusTypeDef HumTempSensor_GetTemperature(float* temperature)
{
	/*
	 * A szenzor mindkét mért értéket visszaadja, mivel a korábbi függvényt használja,
	 * de a páratartalom értékhez nullpointert ad.
	 */
	return HumTempSensor_GetValues(NULL, temperature);
}

// Mért páratartalom érték kiolvasása
HAL_StatusTypeDef HumTempSensor_GetHumidity(float* humidity)
{
	/*
	 * A szenzor mindkét mért értéket visszaadja, mivel a korábbi függvényt használja,
	 * de a hőmérsékletértékhez nullpointert ad.
	 */
	return HumTempSensor_GetValues(humidity, NULL);
}

// Szenzor státusz kiolvasása
HAL_StatusTypeDef HumTempSensor_GetStatus(uint8_t* status)
{
	uint8_t receive_buffer[4];
	HAL_StatusTypeDef stat_tx, stat_rx;

	/*
	 * Mérés elvégzése szükséges ahhoz, hogy a státusz bájtokat
	 * megkapjuk a mért adattal együtt.
	 */
	stat_tx = HAL_I2C_Master_Transmit(&hi2c1, TempHum_Sensor_ADDRESS, NULL, 0, TempHum_Sensor_TIMEOUT);
	HAL_Delay(TempHum_Sensor_MEASURE_DELAY);
	stat_rx = HAL_I2C_Master_Receive(&hi2c1, TempHum_Sensor_ADDRESS, receive_buffer, 4, TempHum_Sensor_TIMEOUT);

	*status = receive_buffer[0] & 0xC0;
	return (stat_tx == HAL_OK && stat_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}
