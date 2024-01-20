/*
 * adc_sensor.h
 *
 *  Created on: Aug 22, 2020
 *      Author: quangnd
 */

#ifndef COMPONENT_ADC_SENSOR_ADC_SENSOR_H_
#define COMPONENT_ADC_SENSOR_ADC_SENSOR_H_

#include "stdint.h"
#include "adc_hw.h"
/**ADC channels HW */

typedef struct ADC_Sensor_t ADC_Sensor;

struct ADC_Sensor_t{
	uint16_t result;
	uint16_t gain;
	void (*update_adc)(ADC_Sensor* p_ss);
};

void adc_sensor_init(void);
void update_volt_charger(ADC_Sensor* p_ss);
void update_volt_au_bat(ADC_Sensor* p_ss);

#endif /* COMPONENT_ADC_SENSOR_ADC_SENSOR_H_ */
