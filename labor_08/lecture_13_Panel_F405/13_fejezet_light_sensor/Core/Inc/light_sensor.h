/*
 * light_sensor.h
 */

#ifndef INC_LIGHT_SENSOR_H_
#define INC_LIGHT_SENSOR_H_

#include "main.h"

/*
 * Lehetséges I2C címek
 *
 * A szenzor címkiválasztó lába lehet a VDD-re vagy a GND-re kötve
 * az áramkör kivezetésének segíségével. Ez a láb ADDRSEL felirattal
 * van jelölve az fejlesztőpanelen.
 */
#define LightSensor_ADDR_GND           0x52 // ADDRSEL GND-re kötve
#define LightSensor_ADDR_FLOAT         0x72 // ADDRSEL lebegve hagyva
#define LightSensor_ADDR_VDD           0x92 // ADDRSEL VDD-re kötve

/*
 * Alapértelmezett időtúllépési érték ms-ban az I2C kommunikációhoz a fényszenzorral
 */
#define LightSensor_Timeout 1000

/*
 * Fénymérő szenzor regisztereinek címei
 */
#define LightSensor_REG_CONTROL        0x00
#define LightSensor_REG_TIMING         0x01
#define LightSensor_REG_THRESHLOWLOW   0x02
#define LightSensor_REG_THRESHLOWHIGH  0x03
#define LightSensor_REG_THRESHHIGHLOW  0x04
#define LightSensor_REG_THRESHHIGHHIGH 0x05
#define LightSensor_REG_INTERRUPT      0x06
#define LightSensor_REG_CRC            0x08
#define LightSensor_REG_ID             0x0A
#define LightSensor_REG_DATA0LOW       0x0C
#define LightSensor_REG_DATA0HIGH      0x0D
#define LightSensor_REG_DATA1LOW       0x0E
#define LightSensor_REG_DATA1HIGH      0x0F

/*
 * I2C beállítások, jelzi a szenzornak, hogy az adat egy bájt vagy szó
 */
#define LightSensor_CMDREG_CMD         0x80
#define LightSensor_CMDREG_CLEARINT    0x40
#define LightSensor_CMDREG_WORD        0x20

/*
 * Táp állapot definíciók
 */
#define LightSensor_POWER_ON           0x03
#define LightSensor_POWER_OFF          0x00

/*
 * Szimbólumok az erősítő erősítésének kiválasztására
 */
#define LightSensor_GAIN_1X            0x00
#define LightSensor_GAIN_16X           0x10
#define LightSensor_GAIN_MASK          0xEF
#define LightSensor_GAIN_POS		   4

/*
 * Szimbólumok az integrálási idő kiválasztására
 */
#define LightSensor_INTEG_13m7         0x00
#define LightSensor_INTEG_101m         0x01
#define LightSensor_INTEG_402m         0x02
#define LightSensor_INTEG_MASK         0xFC

/*
 * Szimbólumok az integrálási mód beállítására
 */
#define LightSensor_INT_OFF            0x00
#define LightSensor_INT_ON             0x10
#define LightSensor_INT_MASK		   0xCF

/*
 * Szimbólumok arra, hogy egymás után hányszor
 * kell nagyobbnak lennie a mért fényerősségnek
 * annál, mint a beállított küszöbérték a
 * megszakítás létrejöttéhez.
 */
#define LightSensor_PERSIST_EVERY      0x00
#define LightSensor_PERSIST_ANY        0x01
#define LightSensor_PERSIST_2CYCLE     0x02
#define LightSensor_PERSIST_3CYCLE     0x03
#define LightSensor_PERSIST_4CYCLE     0x04
#define LightSensor_PERSIST_5CYCLE     0x05
#define LightSensor_PERSIST_6CYCLE     0x06
#define LightSensor_PERSIST_7CYCLE     0x07
#define LightSensor_PERSIST_8CYCLE     0x08
#define LightSensor_PERSIST_9CYCLE     0x09
#define LightSensor_PERSIST_10CYCLE    0x0A
#define LightSensor_PERSIST_11CYCLE    0x0B
#define LightSensor_PERSIST_12CYCLE    0x0C
#define LightSensor_PERSIST_13CYCLE    0x0D
#define LightSensor_PERSIST_14CYCLE    0x0E
#define LightSensor_PERSIST_15CYCLE    0x0F
#define LightSensor_PERSIST_MASK	   0xF0



/*
 * Globális változó, amely a szenzor I2C címét tartalmazza.
 * A korábban megadott LightSensor_ADDR_XXXX konstansok valamelyike lehet.
 *
 *
 * A szenzor címkiválasztó lába lehet a VDD-re vagy a GND-re kötve
 * az áramkör kivezetésének segíségével. Ez a láb ADDRSEL felirattal
 * van jelölve az fejlesztőpanelen.
 */
extern uint8_t LightSensor_Address;/*

 * @brief Megszakítást jelző bit törlése a szenzorban
 */
HAL_StatusTypeDef LightSensor_ClearInterrupt();

/*
 * @brief Parancs küldése a szenzornak
 *
 * @param A parancs
 */
HAL_StatusTypeDef LightSensor_Cmd(uint8_t cmd);

/*
 * @brief I2C cím beállítása
 *
 * @param A cím, ami az ADDRSEL láb által beállításra került
 *        a VDD-hez vagy a GND-hez való csatlakoztatás függvényében.
 *
 */
void LightSensor_SetAddress(uint8_t address);

/*
 * @brief Egy utasítás írása a szenzor kiválasztott regiszterébe
 *
 * @param Az írni kívánt regiszter, ami a LightSensor_REG_XXXX valamelyike lehet
 *
 * @param A regiszter új értéke
 */
HAL_StatusTypeDef LightSensor_Write(uint8_t reg, uint8_t value);

/*
 * @brief Szó írása a szenzor kiválasztott regiszterébe
 *
 * @param Az írni kívánt regiszter, ami a LightSensor_REG_XXXX valamelyike lehet
 *
 * @param A regiszter új értéke
 */
HAL_StatusTypeDef LightSensor_WriteWord(uint8_t reg, uint16_t value);

/*
 * @brief Egy bájt olvasása a szenzor kiválasztott regiszteréből
 *
 * @param Az olvasni kívánt regiszter, ami a LightSensor_REG_XXXX valamelyike lehet
 *
 * @param uint8_t típusú változóra mutató pointer, ahová a kapott érték kerül
 */
HAL_StatusTypeDef LightSensor_Read(uint8_t reg, uint8_t* value);

/*
 * @brief Szenzor teljesítménymódjának beállítása
 *
 * @param A beállítandó teljesítménymód, értéke LightSensor_POWER_ON vagy LightSensor_POWER_OFF lehet
 */
HAL_StatusTypeDef LightSensor_SetPowerState(uint8_t state);

/*
 * @brief Erősítés beállítása a szenzor ADC-jének
 *
 * @param Beállítandó erősítés, értéke LightSensor_GAIN_1X vagy LightSensor_GAIN_16X lehet
 */
HAL_StatusTypeDef LightSensor_SetGain(uint8_t gain);

/*
 * @brief Integrálási idő beállítása a szenzor ADC-jének
 *
 * @param Beállítandó integrálási idő, értéke LightSensor_INTEG_XXXX lehet
 */
HAL_StatusTypeDef LightSensor_SetIntegrationTime(uint8_t time);

/*
 * @brief Szenzor megszakítás állapotának beállítása
 *
 * @param Beállítandó állapot, értéke LightSensor_INT_OFF vagy LightSensor_INT_ON lehet
 */
HAL_StatusTypeDef LightSensor_SetInterruptState(uint8_t state);

/*
 * @brief Az érzékelő megszakítási perzisztenciájának beállítása.
 *
 * @param Beállítandó perzisztencia, értéke LightSensor_PERSIST_XXXX lehet
 */
HAL_StatusTypeDef LightSensor_SetInterruptPersist(uint8_t persist);

/*
 * @brief Megszakítási határok beállítása
 *
 * @param Beállítandó alsó határ
 *
 * @param Beállítandó felső határ
 */
HAL_StatusTypeDef LightSensor_SetThreshold(uint16_t lower, uint16_t upper);

/*
 * @brief Szenzor ADC-jének az integrálási idejének kiolvasása
 *
 * @param Pointer arra a változóra, amely a kapott integrálási időt tárolja.
 *        Értéke LightSensor_INTEG_XXXX lehet
 */
HAL_StatusTypeDef LightSensor_GetIntegrationTime(uint8_t* regVal);

/*
 * @brief Szenzor ADC-jének erősítésének kiolvasása
 *
 * @param Pointer arra a változóra, amely a kapott erősítési értéket tárolja.
 *        Értéke LightSensor_GAIN_1X vagy LightSensor_GAIN_16x lehet
 */
HAL_StatusTypeDef LightSensor_GetGain(uint8_t* regVal);

/*
 * @brief Szenzor 0-s csatornájának kiolvasása.
 *        Ez a csatorna mind a látható, mind az infravörös fényt méri.
 *
 * @param Pointer arra a változóra, amely a mért fényintenzitási értéket tárolja.
 */
HAL_StatusTypeDef LightSensor_GetCH0Value(uint16_t* value);

/*
 * @brief Szenzor 1-es csatornájának kiolvasása. Ez a csatorna csak az infravörös fényt méri.
 *
 * @param Pointer arra a változóra, amely a mért fényintenzitási értéket tárolja..
 */
HAL_StatusTypeDef LightSensor_GetCH1Value(uint16_t* value);

/*
 * @brief Fényintezitás kiolvasása a szenzorból lux mértékegységben.
 *
 * @param Pointer arra a változóra, amely a mért fényintenzitási értéket tárolja..
 */
HAL_StatusTypeDef LightSensor_GetLightLevel(float* lightLevel);

#endif /* INC_LIGHT_SENSOR_H_ */
