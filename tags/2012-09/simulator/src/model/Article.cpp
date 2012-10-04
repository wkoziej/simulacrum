/*
 * Article.cpp
 *
 *  Created on: 19-10-2011
 *      Author: wojtas
 */

#include "Article.h"

Article::Article() {
	food = false;
}

Article::~Article() {
}

bool Article::isFood() {
	return food;
}

bool Article::setFood(bool food) {
	bool t = this->food;
	this->food = food;
	return t;
}
