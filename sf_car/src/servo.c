#include <servo.h>

void servo_pin_setup()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();

	servo_signal.Pin = GPIO_PIN_4;
	servo_signal.Mode = GPIO_MODE_AF_PP;
	servo_signal.Pull = GPIO_NOPULL;
	servo_signal.Speed = GPIO_SPEED_HIGH;
	servo_signal.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOB, &servo_signal);
}

void servo_pwm_setup()
{
	__HAL_RCC_TIM3_CLK_ENABLE();

	servo_handle.Instance = TIM3;
	servo_handle.Init.Period = 2000;
	servo_handle.Init.Prescaler = 1080;
	servo_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	servo_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&servo_handle);
	HAL_TIM_Base_Start(&servo_handle);

	servo_Config.OCMode = TIM_OCMODE_PWM1;
	servo_Config.Pulse = 145;

	HAL_TIM_PWM_ConfigChannel(&servo_handle, &servo_Config, TIM_CHANNEL_1);
	HAL_TIM_PWM_Init(&servo_handle);
	HAL_TIM_PWM_Start(&servo_handle, TIM_CHANNEL_1);
}
