/*
 * velo-pc_atmega328pb_ST7565.c
 *
 * Created: 16.07.2023 21:13:26
 * Author : princ
 * I use in this project Display library - https://github.com/olikraus/u8g2
 */ 

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
#include "string.h"
#include "stdbool.h"
#include <stdio.h>


gpio rtc_int = {(uint8_t *)&PORTD , PORTD2};
gpio lcd_blk = {(uint8_t *)&PORTE , PORTE2};
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



ISR(INT0_vect){
	rtc_int_request=1;
}


int main(void)
{
	sei();
	EICRA |= (0b10 << ISC00);
	EIMSK = 0x01; //0b00000001
	stdout = &mystdout;
	
	char char_array[128]="255\0";
	char time_line[10]="12:30:21";
	u8g2_Setup_st7565_zolen_128x64_f( &u8g2, U8G2_MIRROR_VERTICAL, u8x8_byte_4wire_sw_spi, u8x8_avr_gpio_and_delay );
	uart_init(250000,0);
	twi_init(400000);
	adc_init();
	rtc_int_enable(&sys_rtc ,0);
	adc_init();
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
			rtc_sync(&sys_rtc);
		}
		
		u8g2_SetFont(&u8g2, u8g2_font_freedoomr25_mn);
		u8g2_DrawStr(&u8g2, 1, 25, "88");
		
		
		
		u8g2_SetFont(&u8g2, u8g2_font_freedoomr10_mu);
		u8g2_DrawStr(&u8g2, 5, 52, "00001.15");
		u8g2_DrawStr(&u8g2, 5, 64, "00099.35");
		
		sprintf(time_line , "%02d:%02d:%02d", sys_rtc.hour, sys_rtc.minute, sys_rtc.second);
		
		u8g2_DrawStr(&u8g2, 5, 39, time_line);
		
		u8g2_SendBuffer(&u8g2);
		//_delay_ms(100);
		sleep_cpu();
	}
}

