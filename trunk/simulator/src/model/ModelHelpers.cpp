/*
 * ModelHelpers.cpp
 *
 *  Created on: 2011-10-09
 *      Author: wojtas
 */

#include "ModelHelpers.h"

std::string wstring2string(std::wstring wstr) {
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str;
}

float randBetweenAndStepped(float min, float max, float step) {
	float zeroOne = (random() / (float) RAND_MAX);
	float randBetween = min + zeroOne * (max - min);
	int intVal = randBetween / step;
	return intVal * step;
}
