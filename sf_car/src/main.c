#include <servo.h>
#include <motor_controller.h>
#include <gui.h>
#include "stm32f7xx.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <string.h>
#include <stdlib.h>

UART_HandleTypeDef uart_handle;
TS_StateTypeDef ts_state;

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

int main(void) {
	MPU_Config();
	CPU_CACHE_Enable();
	HAL_Init();
	SystemClock_Config();

	uart_setup();
	servo_pin_setup();
	servo_pwm_setup();
	lcd_setup();
	gui_setup();
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
