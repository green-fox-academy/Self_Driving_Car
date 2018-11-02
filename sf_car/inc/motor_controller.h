#include "stm32f7xx.h"
#include "stm32746g_discovery.h"

#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_

TIM_HandleTypeDef motor_handle;
TIM_OC_InitTypeDef motor_Config;
GPIO_InitTypeDef motor_forward;
GPIO_InitTypeDef motor_backward;
GPIO_InitTypeDef motor_pwm;

void motor_forward_pin_setup();
void motor_backward_pin_setup();
void motor_pin_setup();
void motor_pwm_setup();

#endif /* MOTOR_CONTROLLER_H_ */
