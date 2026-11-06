/*
 * humidity_sensor.h
 */

#ifndef INC_HUMIDITY_SENSOR_H_
#define INC_HUMIDITY_SENSOR_H_

#include "main.h"

/*
 * A hőmérő és páratartalom mérő szenzor címe az I2C buszon
 */
#define TempHum_Sensor_ADDRESS 0x4E

/*
 * Alapértelmezett I2C kommunikációs időtúllépési érték a szenzornak
 */
#define TempHum_Sensor_TIMEOUT 1000

/*
 * A mérési kérés elküldése után a hőmérséklet
 * és páratartalom érzékelőnek a mérés elvégzése némi
 * időt vesz igénybe. Ez alatt az idő alatt nem lehet
 * kiolvasni az adatokat a kimeneti regiszteréből.
 * Ez a mérés befejezésének várakozási ideje.
 */
#define TempHum_Sensor_MEASURE_DELAY 50



/*
 * @brief Mind a páratartalom, mind a hőmérséklet értékének kiolvasása
 *
 * @param Változóra mutató pointer, amelybe a mért páratartalom érték kerül százalékban, 14 biten.
 *
 * @param Változóra mutató pointer, amelybe a mért hőmérsékletérték kerül Celsius-fokban, 14 biten.
 */
HAL_StatusTypeDef HumTempSensor_GetValues(float* humidity, float* temperature);

/*
 * @brief Mért hőmérsékletérték kiolvasása
 *
 * @param Változóra mutató pointer, amelybe a mért hőmérsékletérték kerül Celsius-fokban, 14 biten.
 */
HAL_StatusTypeDef HumTempSensor_GetTemperature(float* temperature);

/*
 * @brief Mért páratartalom érték kiolvasása
 *
 * @param Változóra mutató pointer, amelybe a mért páratartalom érték kerül százalékban, 14 biten.
 */
HAL_StatusTypeDef HumTempSensor_GetHumidity(float* humidity);

/*
 * @brief Szenzor státusz kiolvasása
 *
 * @note Lehetséges státusz értékek:
 *       S1 = 0 ; S0 = 0:
 *       	Normál üzemmód, érvényes adatok, amelyek az utolsó
 *       	mérési ciklus óta még nem kerültek lekérésre
 *       S1 = 0 ; S0 = 1:
 *       	Elavult adatok: az adatok már lekérésre kerültek az utolsó
 *       	mérés óta vagy mielőtt az első mérés befejeződött volna
 *       S1 = 1 ; S0 = 0:
 *       	Az eszköz parancsra várakozik
 *       S1 = 1 ; S0 = 1:
 *       	Nem használt
 *
 * @param Változóra mutató pointer, amibe a státusz információt kapjuk jobbra igazítva
 */
HAL_StatusTypeDef HumTempSensor_GetStatus(uint8_t* status);

#endif /* INC_HUMIDITY_SENSOR_H_ */
