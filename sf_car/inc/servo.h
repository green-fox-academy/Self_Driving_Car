#include "stm32f7xx.h"
#include "stm32746g_discovery.h"

#ifndef SERVO_H_
#define SERVO_H_

TIM_HandleTypeDef servo_handle;
TIM_OC_InitTypeDef servo_Config;
GPIO_InitTypeDef servo_signal;

void servo_pin_setup();
void servo_pwm_setup();

#endif /* SERVO_H_ */
