/*
 * adc_sensor.c
 *
 *  Created on: Aug 22, 2020
 *      Author: quangnd
 */

#include "adc_sensor.h"


void adc_sensor_init(){
	adc_hardware_init();
}

void update_volt_charger(ADC_Sensor* p_ss){
	adc_read_volt_charger(&p_ss->result);
}
void update_volt_au_bat(ADC_Sensor* p_ss){
	adc_read_volt_au_bat(&p_ss->result);
}

