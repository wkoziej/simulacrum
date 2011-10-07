/*
 * Config.h
 *
 *  Created on: 24-01-2011
 *      Author: wojtas
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define NO_OF_RESOURCES 2 // Liczba surowców -> 0 - natchnienie, 1 - materia
#define NO_OF_PRODUCTS 2 // Liczba surowców -> 0 - natchnienie, 1 - materia

#define MAX_MOVE_LAG 2 // Maksymalne opóźnienie
#define INITIAL_NO_OF_CREATURES_IN_FIELD 10 // Początkowa liczba istot na polu
#define NO_OF_POPULATIONS 10
#define MAX_RENEWAL 5 // Maksymalna ilość w jakiej surowiec się odnawia
#define CROSSOVER 0.95
#define MUTATION  0.01
#define MAX_POINT_ON_ROAD 4
#define MAX_RESOURCE_QUANT 50
#define MAX_PRODUCT_QUANT 50
#define POSITIVE_INFINITY 1e+37

#endif /* CONFIG_H_ */
