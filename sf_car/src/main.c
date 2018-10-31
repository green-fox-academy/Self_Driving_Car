/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_ts.h"
#include "stm32f7xx.h"
#include "stm32746g_discovery.h"
#include "GUI.h"
#include "WM.h"
#include <string.h>
#include <stdlib.h>

UART_HandleTypeDef uart_handle;
TIM_HandleTypeDef servo_handle;
TIM_OC_InitTypeDef servo_Config;
GPIO_InitTypeDef servo_signal;
TIM_HandleTypeDef motor_handle;
TIM_OC_InitTypeDef motor_Config;
GPIO_InitTypeDef motor_forward;
GPIO_InitTypeDef motor_backward;
GPIO_InitTypeDef motor_pwm;
TS_StateTypeDef ts_state;

uint8_t GUI_Initialized = 0;
TIM_HandleTypeDef TimHandle;
uint32_t uwPrescalerValue = 0;

volatile int touch_x;
volatile int touch_y;

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

static void SystemClock_Config(void);
static void Error_Handler(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);

void uart_setup()
{
	uart_handle.Init.BaudRate = 115200;
	uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	uart_handle.Init.StopBits = UART_STOPBITS_1;
	uart_handle.Init.Parity = UART_PARITY_NONE;
	uart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_handle.Init.Mode = UART_MODE_TX_RX;
	BSP_COM_Init(COM1, &uart_handle);
}

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

void lcd_setup()
{
	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_SelectLayer(0);
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
}

void gui_setup()
{
	//start
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(0, 136, 160, 136);
	BSP_LCD_DisplayStringAt(38, 191, "START", LEFT_MODE);
	//stop
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(0, 0, 160, 136);
	BSP_LCD_DisplayStringAt(45, 55, "STOP", LEFT_MODE);
	//right
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillRect(160, 0, 160, 136);
	BSP_LCD_DisplayStringAt(197, 55, "RIGHT", LEFT_MODE);
	//left
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_FillRect(160, 136, 160, 136);
	BSP_LCD_DisplayStringAt(205, 191, "LEFT", LEFT_MODE);
	//neutral
	BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
	BSP_LCD_FillRect(320, 136, 160, 136);
	BSP_LCD_DisplayStringAt(340, 191, "NEUTRAL", LEFT_MODE);
	//changing direction
	BSP_LCD_SetTextColor(LCD_COLOR_BROWN);
	BSP_LCD_FillRect(320, 0, 160, 136);
	BSP_LCD_DisplayStringAt(357, 55, "SHIFT", LEFT_MODE);
}

void stem_gui(){

	__HAL_RCC_CRC_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	uwPrescalerValue = (uint32_t) ((SystemCoreClock /2) / 10000) - 1;
	TimHandle.Instance = TIM3;
	TimHandle.Init.Period = 500 - 1;
	TimHandle.Init.Prescaler = uwPrescalerValue;
	TimHandle.Init.ClockDivision = 0;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
	if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
		while (1) {
		}
	}
	if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK) {
		while (1) {
		}
	}

	BSP_TS_Init(480, 272);
	BSP_SDRAM_Init();
	GUI_Init();
	WM_MULTIBUF_Enable(1);
	GUI_Initialized = 1;
	WM_SetCreateFlags(WM_CF_MEMDEV);

	  GUI_Clear();
	  GUI_SetFont(&GUI_Font20_1);
	  GUI_DispStringAt("Hello world!", (LCD_GetXSize()-100)/2, (LCD_GetYSize()-20)/2);
	  while(1);

}

int main(void) {

	MPU_Config();
	CPU_CACHE_Enable();
	HAL_Init();
	SystemClock_Config();


	uart_setup();
	servo_pin_setup();
	servo_pwm_setup();
	lcd_setup();
	stem_gui();
	//gui_setup();

	motor_forward_pin_setup();
	motor_backward_pin_setup();
	motor_pin_setup();
	motor_pwm_setup();

	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);

	while (1) {
		BSP_TS_GetState(&ts_state);

		if (ts_state.touchDetected) {
			touch_x = ts_state.touchX[0];
			touch_y = ts_state.touchY[0];

			if (touch_x >= 0 && touch_x <= 160 && touch_y >= 136 && touch_y <= 272) {
				TIM2->CCR1 = 75;
			} else if (touch_x >= 0 && touch_x <= 160 && touch_y >= 10 && touch_y <= 85) {
				TIM2->CCR1 = 0;
			} else if (touch_x >= 200 && touch_x <= 275 && touch_y >= 10 && touch_y <= 85) {
				TIM3->CCR1 = 110;
			} else if (touch_x >= 200 && touch_x <= 275 && touch_y >= 150 && touch_y <= 225) {
				TIM3->CCR1 = 150;
			} else if (touch_x >= 380 && touch_x <= 455 && touch_y >= 150 && touch_y <= 225) {
				TIM3->CCR1 = 130;
			} else if (touch_x >= 380 && touch_x <= 455 && touch_y >= 10 && touch_y <= 85) {
				HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
				HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
			}
		}
    }
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
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
    {
        Error_Handler();
    }
}

static void Error_Handler(void) {
    while (1)
    {
    }
}
static void MPU_Config(void) {
    MPU_Region_InitTypeDef MPU_InitStruct;

    /* Disable the MPU */
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

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
    while (1)
    {
    }
}
#endif
