#include <motor_controller.h>

void motor_forward_pin_setup()
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	motor_forward.Pin = GPIO_PIN_0;
	motor_forward.Mode = GPIO_MODE_OUTPUT_PP;
	motor_forward.Pull = GPIO_NOPULL;
	motor_forward.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &motor_forward);
}

void motor_backward_pin_setup()
{
	__HAL_RCC_GPIOF_CLK_ENABLE();

	motor_backward.Pin = GPIO_PIN_10;
	motor_backward.Mode = GPIO_MODE_OUTPUT_PP;
	motor_backward.Pull = GPIO_NOPULL;
	motor_backward.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOF, &motor_backward);
}

void motor_pin_setup()
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	motor_pwm.Pin = GPIO_PIN_15;
	motor_pwm.Mode = GPIO_MODE_AF_PP;
	motor_pwm.Pull = GPIO_NOPULL;
	motor_pwm.Speed = GPIO_SPEED_HIGH;
	motor_pwm.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &motor_pwm);
}

void motor_pwm_setup()
{
	__HAL_RCC_TIM2_CLK_ENABLE();

	motor_handle.Instance = TIM2;
	motor_handle.Init.Period = 1000;
	motor_handle.Init.Prescaler = 1080;
	motor_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	motor_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&motor_handle);
	HAL_TIM_Base_Start(&motor_handle);

	motor_Config.OCMode = TIM_OCMODE_PWM1;
	motor_Config.Pulse = 0;

	HAL_TIM_PWM_ConfigChannel(&motor_handle, &motor_Config, TIM_CHANNEL_1);
	HAL_TIM_PWM_Init(&motor_handle);
	HAL_TIM_PWM_Start(&motor_handle, TIM_CHANNEL_1);
}
