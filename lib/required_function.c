
/*
 * requred_function.c
 *
 * Created: 17.07.2023 22:28:51
 *  Author: princ
 */ 
#include "required_function.h"

uint16_t tmp = 0;
uint16_t speed = 0;

ISR(TIMER1_OVF_vect){
	
	
}

ISR(TIMER1_COMPA_vect){
	
}


void init_timers(void){
	TCCR1A = 0x00;
	TCCR1B |= (0b101 << CS10);
	TCNT1 = 22768;
}

void elapse_detect(void){
	speed = TCNT1;
	TCNT1 = 0;
	
}
uint8_t get_current_speed(void){
	return normalize_speed(speed);
}

uint8_t normalize_speed(uint16_t value) {
	uint16_t inverted_value = 65535-value;
	 
	if(inverted_value > 35000){
		inverted_value = inverted_value - 35000;
		uint8_t speed_in_kmh = inverted_value/600;
		if (speed_in_kmh < 99){
			return speed_in_kmh;
		}else{
			return 0;
		}
		//printf("%u; %u; %u\n\r", tmp, tmp2, tmp3);
	}else{
		return 0;
	}
}