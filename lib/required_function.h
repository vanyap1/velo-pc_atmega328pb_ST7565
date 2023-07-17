
/*
 * requred_function.h
 *
 * Created: 17.07.2023 22:28:59
 *  Author: princ
 */ 
#include <avr/io.h>
#include "string.h"
#include "stdbool.h"

#ifndef REQUIRED_FUNCTION_H_
#define REQUIRED_FUNCTION_H_

typedef struct {
	double daily_odo;        
	double full_odo;        
	uint16_t max_speed;          
	uint16_t speed;                     
} dashboard_struc;
#endif