#include "stm32f7xx.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <string.h>
#include <stdlib.h>

UART_HandleTypeDef uart_handle;
GPIO_InitTypeDef rc_servo_pin;
GPIO_InitTypeDef rc_motor_pin;
TIM_HandleTypeDef rc_timer;

void motor_states();
void servo_states();
volatile int get_motor_state = 0;
volatile int get_servo_state = 0;
volatile int state_motor = 0;
volatile int state_servo = 0;

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

 void rc_servo_pin_setup()
 {
     __HAL_RCC_GPIOB_CLK_ENABLE();
     rc_servo_pin.Pin = GPIO_PIN_15;
     rc_servo_pin.Mode = GPIO_MODE_IT_RISING_FALLING;
     rc_servo_pin.Pull = GPIO_NOPULL;
     rc_servo_pin.Speed = GPIO_SPEED_HIGH;
     HAL_GPIO_Init(GPIOB, &rc_servo_pin);
     HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0x0F, 0x00);
     HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
 }

 void rc_motor_pin_setup()
 {
     __HAL_RCC_GPIOF_CLK_ENABLE();
     rc_motor_pin.Pin = GPIO_PIN_9;
     rc_motor_pin.Mode = GPIO_MODE_IT_RISING_FALLING;
     rc_motor_pin.Pull = GPIO_NOPULL;
     rc_motor_pin.Speed = GPIO_SPEED_HIGH;
     HAL_GPIO_Init(GPIOF, &rc_motor_pin);
     HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0x0F, 0x00);
     HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
 }

 void rc_init_timer()
 {
     __HAL_RCC_TIM9_CLK_ENABLE();
     rc_timer.Instance = TIM9;
     rc_timer.Init.Period = 25000;
     rc_timer.Init.Prescaler = 14;
     rc_timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
     rc_timer.Init.CounterMode = TIM_COUNTERMODE_UP;
     HAL_TIM_Base_Init(&rc_timer);
 }

int main(void) {
    MPU_Config();
    CPU_CACHE_Enable();
    HAL_Init();
    SystemClock_Config();
    uart_setup();

    rc_servo_pin_setup();
    rc_motor_pin_setup();
    rc_init_timer();

    while (1) {
    }
}

void EXTI15_10_IRQHandler()
{
    HAL_GPIO_EXTI_IRQHandler(rc_servo_pin.Pin);
}

void EXTI9_5_IRQHandler()
{
    HAL_GPIO_EXTI_IRQHandler(rc_motor_pin.Pin);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == rc_motor_pin.Pin) {
        if (state_motor == 0){
            rc_init_timer();
            HAL_TIM_Base_Start(&rc_timer);
            rc_motor_pin.Mode = GPIO_MODE_IT_FALLING;
            HAL_GPIO_Init(GPIOF, &rc_motor_pin);
            state_motor++;
        } else if (state_motor == 1){
            get_motor_state = __HAL_TIM_GetCounter(&rc_timer);
            __HAL_TIM_SetCounter(&rc_timer, 0);
            HAL_TIM_Base_Stop(&rc_timer);
            HAL_TIM_Base_DeInit(&rc_timer);
            rc_motor_pin.Mode = GPIO_MODE_IT_RISING;
            HAL_GPIO_Init(GPIOF, &rc_motor_pin);
            state_motor--;
        }
        motor_states();
    }

    if (GPIO_Pin == rc_servo_pin.Pin) {
        if (state_servo == 0){
            rc_init_timer();
            HAL_TIM_Base_Start(&rc_timer);
            rc_servo_pin.Mode = GPIO_MODE_IT_FALLING;
            HAL_GPIO_Init(GPIOB, &rc_servo_pin);
            state_servo++;
        } else if (state_servo == 1){
            get_servo_state = __HAL_TIM_GetCounter(&rc_timer);
            __HAL_TIM_SetCounter(&rc_timer, 0);
            HAL_TIM_Base_Stop(&rc_timer);
            HAL_TIM_Base_DeInit(&rc_timer);
            rc_servo_pin.Mode = GPIO_MODE_IT_RISING;
            HAL_GPIO_Init(GPIOB, &rc_servo_pin);
            state_servo--;
        }
        servo_states();
    }
}

void motor_states()
{
    if (get_motor_state > 13400 && get_motor_state < 13550) {
    	// printf("motor is going forward\r\n");
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_SET);
        TIM2->CCR1 = 155;
    } else if (get_motor_state > 1950 && get_motor_state < 2050) {
    	// printf("motor is going back\r\n");
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
        TIM2->CCR1 = 155;
    } else if (get_motor_state > 20340 && get_motor_state < 20450) {
        // printf("neutral motor state\r\n");
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
        TIM2->CCR1 = 0;
    }
}

void servo_states()
{
    if (get_servo_state > 12300 && get_servo_state < 12400) {
    	// printf("servo is going right\r\n");
        TIM3->CCR1 = 90;
    } else if (get_servo_state > 2000 && get_servo_state < 2100) {
    	// printf("servo is going left\r\n");
        TIM3->CCR1 = 180;
    } else if (get_servo_state > 20000 && get_servo_state < 20050) {
    	//printf("neutral servo state\r\n");
        TIM3->CCR1 = 130;
    }
}

PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&uart_handle, (uint8_t *) &ch, 1, 0xFFFF);
    return ch;
}

static void SystemClock_Config(void)
{
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

static void Error_Handler(void)
{
    while (1) {
    }
}

static void MPU_Config(void)
{
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

static void CPU_CACHE_Enable(void)
{
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
