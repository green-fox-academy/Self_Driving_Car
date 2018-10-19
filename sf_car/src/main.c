#include "stm32f7xx.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <string.h>
#include <stdlib.h>

UART_HandleTypeDef uart_handle;  //uart typedef
TIM_HandleTypeDef servo_handle;  //pwm timer typedef
TIM_OC_InitTypeDef servo_Config; // pwm typedef for servo
GPIO_InitTypeDef servo_motor;	//servo_motor typedef
TS_StateTypeDef ts_state;		 // steering for button
GPIO_InitTypeDef motor_forward;  //engine turning forward
TIM_OC_InitTypeDef motor_Config; //pwm typedef for motor
TIM_HandleTypeDef motor_handle;  //pwm timer typedef for motor
GPIO_InitTypeDef motor_pwm;		 //pwm motor gpio init
GPIO_InitTypeDef motor_backward; //engine turning backward
GPIO_InitTypeDef remote_control; //remote control pin
TIM_HandleTypeDef rc_handle;  //rc control timer

volatile int x;
volatile int y;
volatile int state;
volatile int counter;
volatile int start;
volatile int stop;
volatile int time_result;

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static void SystemClock_Config(void);
static void Error_Handler(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);

void uart_setup() {
	//uart setup
	uart_handle.Init.BaudRate = 115200;
	uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	uart_handle.Init.StopBits = UART_STOPBITS_1;
	uart_handle.Init.Parity = UART_PARITY_NONE;
	uart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_handle.Init.Mode = UART_MODE_TX_RX;
	BSP_COM_Init(COM1, &uart_handle);
}

void servo_motor_setup() {

	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

	//servo_motor setup
	servo_motor.Pin = GPIO_PIN_4;
	servo_motor.Mode = GPIO_MODE_AF_PP;
	servo_motor.Pull = GPIO_NOPULL;
	servo_motor.Speed = GPIO_SPEED_HIGH;
	servo_motor.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOB, &servo_motor);
}

void servo_pwm_setup() {

	__HAL_RCC_TIM3_CLK_ENABLE()
	;

	//pwm setup
	servo_handle.Instance = TIM3;
	servo_handle.Init.Period = 2000;
	servo_handle.Init.Prescaler = 1080;
	servo_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	servo_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&servo_handle);
	HAL_TIM_Base_Start(&servo_handle);

	//pwm config
	servo_Config.OCMode = TIM_OCMODE_PWM1;
	servo_Config.Pulse = 145;

	//pwm init & start
	HAL_TIM_PWM_ConfigChannel(&servo_handle, &servo_Config, TIM_CHANNEL_1);
	HAL_TIM_PWM_Init(&servo_handle);
	HAL_TIM_PWM_Start(&servo_handle, TIM_CHANNEL_1);
}

void motor_forward_setup() {

	//motor forward
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;

	motor_forward.Pin = GPIO_PIN_0;
	motor_forward.Mode = GPIO_MODE_OUTPUT_PP;
	motor_forward.Pull = GPIO_NOPULL;
	motor_forward.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &motor_forward);
}

void motor_backward_setup() {

	//motor backward
	__HAL_RCC_GPIOF_CLK_ENABLE()
	;

	motor_backward.Pin = GPIO_PIN_10;
	motor_backward.Mode = GPIO_MODE_OUTPUT_PP;
	motor_backward.Pull = GPIO_NOPULL;
	motor_backward.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOF, &motor_backward);
}

void motor_pin_setup() {

	__HAL_RCC_GPIOA_CLK_ENABLE()
	;

	//motor pin setup
	motor_pwm.Pin = GPIO_PIN_15;
	motor_pwm.Mode = GPIO_MODE_AF_PP;
	motor_pwm.Pull = GPIO_NOPULL;
	motor_pwm.Speed = GPIO_SPEED_HIGH;
	motor_pwm.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOA, &motor_pwm);
}

void motor_pwm_setup() {

	__HAL_RCC_TIM2_CLK_ENABLE()
	;

	//motor pwm setup
	motor_handle.Instance = TIM2;
	motor_handle.Init.Period = 1000;
	motor_handle.Init.Prescaler = 1080;
	motor_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	motor_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&motor_handle);
	HAL_TIM_Base_Start(&motor_handle);

	//motor pwm config
	motor_Config.OCMode = TIM_OCMODE_PWM1;
	motor_Config.Pulse = 0;

	//motor pwm init & start
	HAL_TIM_PWM_ConfigChannel(&motor_handle, &motor_Config, TIM_CHANNEL_1);
	HAL_TIM_PWM_Init(&motor_handle);
	HAL_TIM_PWM_Start(&motor_handle, TIM_CHANNEL_1);
}

void lcd_setup() {
	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_SelectLayer(0);
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
}

void remote_control_init() {

	__HAL_RCC_GPIOA_CLK_ENABLE()
	;

	//remote control pin setup
	remote_control.Pin = GPIO_PIN_8;
	remote_control.Mode = GPIO_MODE_IT_RISING;
	remote_control.Pull = GPIO_NOPULL;
	remote_control.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &remote_control);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0x0F, 0x00);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void gui_interface() {
	//start
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(0, 150, 75, 75);
	BSP_LCD_DisplayStringAt(0, 230, "START", LEFT_MODE);
	//stop
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(0, 10, 75, 75);
	BSP_LCD_DisplayStringAt(0, 90, "STOP", LEFT_MODE);
	//right
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillRect(200, 10, 75, 75);
	BSP_LCD_DisplayStringAt(200, 90, "RIGHT", LEFT_MODE);
	//left
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_FillRect(200, 150, 75, 75);
	BSP_LCD_DisplayStringAt(200, 230, "LEFT", LEFT_MODE);
	//neutral
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_FillRect(380, 150, 75, 75);
	BSP_LCD_DisplayStringAt(363, 230, "NEUTRAL", LEFT_MODE);
	//changing direction
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
	BSP_LCD_FillRect(380, 10, 75, 75);
	BSP_LCD_DisplayStringAt(380, 90, "SHIFT", LEFT_MODE);
}

void rc_control_timer() {
	__HAL_RCC_TIM4_CLK_ENABLE()
	;

	//pwm setup
	rc_handle.Instance = TIM4;
	rc_handle.Init.Period = 65535;
	rc_handle.Init.Prescaler = 0;
	rc_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	rc_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&rc_handle);
	HAL_TIM_Base_Start_IT(&rc_handle);

	HAL_NVIC_SetPriority(TIM4_IRQn, 0x0F, 0x00);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);

}

int main(void) {

	MPU_Config();
	CPU_CACHE_Enable();
	HAL_Init();
	SystemClock_Config();
	uart_setup();
	servo_motor_setup();
	servo_pwm_setup();
	lcd_setup();
	gui_interface();
	motor_forward_setup();
	motor_backward_setup();
	motor_pin_setup();
	motor_pwm_setup();
	remote_control_init();
	rc_control_timer();

	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

	while (1) {
		BSP_TS_GetState(&ts_state);

		if (ts_state.touchDetected) {
			x = ts_state.touchX[0];
			y = ts_state.touchY[0];

			if (x >= 0 && x <= 75 && y >= 150 && y <= 225) {
				TIM2->CCR1 = 75;
			} else if (x >= 0 && x <= 75 && y >= 10 && y <= 85) {
				TIM2->CCR1 = 0;
			} else if (x >= 200 && x <= 275 && y >= 10 && y <= 85) {
				TIM3->CCR1 = 110;
			} else if (x >= 200 && x <= 275 && y >= 150 && y <= 225) {
				TIM3->CCR1 = 150;
			} else if (x >= 380 && x <= 455 && y >= 150 && y <= 225) {
				TIM3->CCR1 = 130;
			} else if (x >= 380 && x <= 455 && y >= 10 && y <= 85) {
				HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
				HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
			}
		}
	}
}

void TIM4_IRQHandler() {
	HAL_TIM_IRQHandler(&rc_handle);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	counter++;

}

void EXTI9_5_IRQHandler() {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	HAL_TIM_Base_Start_IT(&rc_handle);
	start = __HAL_TIM_GET_COMPARE(rc_handle, TIM_CHANNEL_1);
	remote_control.Mode = GPIO_MODE_IT_FALLING;




}

PUTCHAR_PROTOTYPE {
	HAL_UART_Transmit(&uart_handle, (uint8_t *) &ch, 1, 0xFFFF);

	return ch;
}

static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK) {
		Error_Handler();
	}
}
static void Error_Handler(void) {
	while (1) {
	}
}
static void MPU_Config(void) {
	MPU_Region_InitTypeDef MPU_InitStruct;

	HAL_MPU_Disable();

	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0x20010000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}
static void CPU_CACHE_Enable(void) {
	SCB_EnableICache();
	SCB_EnableDCache();
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	while (1)
	{
	}
}
#endif
