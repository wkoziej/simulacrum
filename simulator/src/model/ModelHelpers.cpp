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
