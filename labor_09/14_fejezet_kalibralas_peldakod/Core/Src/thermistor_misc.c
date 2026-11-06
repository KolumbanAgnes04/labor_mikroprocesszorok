/*
 * thermistor_misc.c
 *
 *  Created on: Feb 11, 2021
 *      Author: Palotas Gergely
 */

#include "thermistor_misc.h"
#include "stm32f4xx_hal.h"
#include "math.h"
#include "float.h"
#include "string.h"
#include "stdio.h"
#include "OLED.h"

extern OLED_t SSD1306;

//a termisztor ellenállásának számítása a konverziós eredményből
float CalculateImpedance(uint32_t conversion_result)
{
	//referencia feszültség
	const float VDD = 3.3;

	//referencia ellenállás
	const float REFERENCE_IMPEDANCE = 10000;

	//az ADC bemeneti feszültégének kiszámítása
	float input_voltage = VDD * (float)conversion_result / 4095.0;

	//a termisztor ellenállásának kiszámítása
	float thermistor_impedance = 0.0;
	if (input_voltage == 0) thermistor_impedance = FLT_MAX;
	else thermistor_impedance = (VDD / input_voltage - 1) * REFERENCE_IMPEDANCE;

	return (thermistor_impedance);
}

//a hőmérséklet számítása a konverziós eredményből
float CalculateTemperature(uint32_t conversion_result)
{
	//nulla fok Kelvin-skála szerint
	const float ZERO_IN_KELVIN = 273.15;

	//Steinhart-Hart együtthatók
	const float A = 1.135824991E-3f;
	const float B = 2.328905183E-4f;
	const float C = 0.9367585770E-7f;

	//a termisztor ellenállásának kiszámítása
	float thermistor_impedance = CalculateImpedance(conversion_result);

	//hőmérséklet kiszámítása Steinhart-Hart egyenlet segítségével
	float reciprocal_temp = A + B*log(thermistor_impedance) + C* pow(log(thermistor_impedance),3);
	float temp = pow(reciprocal_temp, -1);

	return (temp-ZERO_IN_KELVIN);
}

//szöveg megjelenítése
void DisplayText(char* text_first_line, char* text_second_line, int unit)
{
	//előző kijelző törlése
	OLED_Fill(Black);

	//szöveg kiírása az első sorba
	OLED_SetCursor(0, 0);
	OLED_WriteString(text_first_line, Font_11x18, White);

	//szöveg kiírása a második sorba
	OLED_SetCursor(0, 36);
	OLED_WriteString(text_second_line, Font_11x18, White);

	//mértékegység kiírása
	switch (unit)
	{
	case Celsius:
		OLED_DrawCircle(SSD1306.CurrentX + 4, 38, 2, White);
		OLED_SetCursor(SSD1306.CurrentX + 8, 36);
		OLED_WriteString("C", Font_11x18, White);
		break;
	case NoUnit:
		break;
	default:
		break;
	}

	//megjelenítés
	OLED_UpdateScreen();
}

//szöveg és hozzá tartozó integer érték megjelenítése
void DisplayTextAndInteger(char* text, const int value, int unit)
{
	//integer érték átalakítása szöveggé
	char temp_string[64] = "";
	int ret = snprintf(temp_string, sizeof(temp_string), "%d", value);
	if ((ret < 0) || (ret >= sizeof(temp_string)))
	{
		strcpy(temp_string, "Conv Err");
	}

	DisplayText(text, temp_string, unit);
}

//szöveg és hozzá tartozó float érték megjelenítése
void DisplayTextAndFloat(char* text, const float value, int unit)
{
	//float érték átalakítása szöveggé egy tizedes jegy pontossággal
	char temp_string[64] = "";
	int ret = snprintf(temp_string, sizeof(temp_string), "%.1f", value);
	if ((ret < 0) || (ret >= sizeof(temp_string)))
	{
		strcpy(temp_string, "Conv Err");
	}

	DisplayText(text, temp_string, unit);
}
