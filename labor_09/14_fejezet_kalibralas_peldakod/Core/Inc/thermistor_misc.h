#ifndef _THERMISTOR_MISC_H
#define _THERMISTOR_MISC_H

#include "stdint.h"

//lehetséges mértékegységek
enum Unit
{
	NoUnit = 0,
	Celsius = 1
};

// a hőmérséklet számítása a konverziós eredményből
float CalculateTemperature(uint32_t conversion_result);
// szöveg megjelenítése
void DisplayText(char* text_first_line, char* text_second_line, int unit);
// szöveg és hozzá tartozó integer érték megjelenítése
void DisplayTextAndInteger(char* text, const int value, int unit);
// szöveg és hozzá tartozó float érték megjelenítése
void DisplayTextAndFloat(char* text, const float value, int unit);

#endif  // _THERMISTOR_MISC_H
