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

float randomFloat01() {
	return (random() / (float) RAND_MAX);
}

float randBetweenAndStepped(float min, float max, float step) {
	float zeroOne = randomFloat01();
	float randBetween = min + zeroOne * (max - min);
	int intVal = randBetween / step;
	return intVal * step;
}

bool flipCoin(float p) {
	return ((p == 1.0) ? true :
			(p == 0.0) ? false : ((randomFloat01() <= p) ? true : false));
}
