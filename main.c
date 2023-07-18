/*
 * velo-pc_atmega328pb_ST7565.c
 *
 * Created: 16.07.2023 21:13:26
 * Author : princ
 * I use in this project Display library - https://github.com/olikraus/u8g2
 */ 
//-Wl,-u,vfprintf -lprintf_flt -lm


#include <avr/io.h>
#include <u8g2.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "lib/display_fn.h"
#include "lib/adc_hal.h"
#include "lib/uart_hal.h"
#include "lib/gpio_driver.h"
#include "lib/twi_hal.h"
#include "lib/rtc.h"
#include "lib/required_function.h"
#include "string.h"
#include "float.h"
#include "stdbool.h"
#include <stdio.h>


gpio rtc_int = {(uint8_t *)&PORTD , PORTD2};
gpio hal_int = {(uint8_t *)&PORTD , PORTD3};

gpio lcd_blk = {(uint8_t *)&PORTE , PORTE2};
gpio health = {(uint8_t *)&PORTB , PORTB5};

float const wheel_lenght = 0.00205f;


rtc_date sys_rtc = {.date = 16,
	.month = 7,
	.year = 23,
	.dayofweek = 6,
	.hour = 22,
	.minute = 32,
	.second = 00
};

	
uint8_t rtc_int_request = 0;
uint16_t BAT_VOLT = 0;

static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);

u8g2_t u8g2;
dashboard_struc dashboard ;



ISR(INT0_vect){
	rtc_int_request=1;
	//toggle_pin_level(&health);
}

ISR(INT1_vect){
	dashboard.daily_odo = dashboard.daily_odo + wheel_lenght;
	dashboard.full_odo = dashboard.full_odo + wheel_lenght;
	elapse_detect();
}


int main(void)
{
	sei();
	EICRA |= (0b10 << ISC00) | (0b10 << ISC10);
	EIMSK |= (1 << INT0) | (1 << INT1); 
	stdout = &mystdout;

	char time_line[10]="00:00:00";
	char speed[3]="00";
	char max_speed[3]="00";
	char d_odo[10]="\0";
	char f_odo[10]="\0";

	u8g2_Setup_st7565_zolen_128x64_f( &u8g2, U8G2_MIRROR_VERTICAL, u8x8_byte_4wire_sw_spi, u8x8_avr_gpio_and_delay );
	uart_init(250000,0);
	twi_init(400000);
	adc_init();
	rtc_int_enable(&sys_rtc ,0);
	adc_init();
	init_timers();
	//rtc_set(&sys_rtc);
	
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_SetFlipMode(&u8g2, 1);
	u8g2_SetContrast(&u8g2, 100);
    
	set_pin_dir(&lcd_blk , PORT_DIR_OUT); set_pin_level(&lcd_blk, true);
	set_pin_dir(&rtc_int, PORT_DIR_IN);
	
	u8g2_ClearBuffer(&u8g2);
	
	u8g2_SetFont(&u8g2, u8g2_font_freedoomr10_mu);
    u8g2_DrawRFrame(&u8g2, 0, 38, 74, 26 , 3);
	u8g2_DrawRFrame(&u8g2, 0, 25, 74, 14 , 3);
	
	u8g2_DrawStr(&u8g2, 77, 64, "KM");
	u8g2_DrawStr(&u8g2, 50, 25, "KM/H");
	
	u8g2_SendBuffer(&u8g2);
	sleep_enable();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	while (1) 
    {
		if (rtc_int_request != 0){
			rtc_int_request = 0;
			rtc_sync(&sys_rtc);
			dashboard.speed = get_current_speed();
			if(dashboard.max_speed < dashboard.speed){
				dashboard.max_speed = dashboard.speed;
			}
			sprintf(time_line , "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
			sprintf(speed, "%02d", dashboard.speed);
			sprintf(max_speed, "%02d", dashboard.max_speed);
			sprintf(d_odo,"%08.3f", dashboard.daily_odo);
			sprintf(f_odo,"%08.2f", dashboard.full_odo);
		
			u8g2_SetFont(&u8g2, u8g2_font_freedoomr25_mn);
			u8g2_DrawStr(&u8g2, 1, 25, speed);
			u8g2_SetFont(&u8g2, u8g2_font_freedoomr10_mu);
			u8g2_DrawStr(&u8g2, 50, 12, max_speed);
			
			u8g2_DrawStr(&u8g2, 5, 52, d_odo);
			u8g2_DrawStr(&u8g2, 5, 64, f_odo);
			
			u8g2_DrawStr(&u8g2, 5, 39, time_line);
			u8g2_SendBuffer(&u8g2);
			//sleep_cpu();
		}
		_delay_ms(100);
		
		printf("%u\n\r", get_current_speed());
		//sleep_cpu();
	}
}

