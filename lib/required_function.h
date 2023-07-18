
/*
 * requred_function.h
 *
 * Created: 17.07.2023 22:28:59
 *  Author: princ
 */ 


#ifndef REQUIRED_FUNCTION_H_
#define REQUIRED_FUNCTION_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


typedef struct {
	double daily_odo;        
	double full_odo;        
	uint16_t max_speed;          
	uint16_t speed;                     
} dashboard_struc;
#endif

void init_timers(void);
void elapse_detect(void);

uint8_t get_current_speed(void);
uint8_t normalize_speed(uint16_t value);