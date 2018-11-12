#include "GUI.h"
#include "stm32f7xx_it.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_audio.h"

#ifdef USE_RTOS_SYSTICK
#include <cmsis_os.h>
#endif

extern volatile GUI_TIMER_TIME OS_TimeMS;
extern LTDC_HandleTypeDef hltdc;
extern SAI_HandleTypeDef haudio_in_sai;

/**
 * @brief  This function handles SysTick Handler, but only if no RTOS defines it.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
    /* Update the LocalTime by adding 1 ms each SysTick interrupt */
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
#ifdef USE_RTOS_SYSTICK
    osSystickHandler();
#endif

    OS_TimeMS++;
}
/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1) {
    }
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief This function handles DMA2 Stream 7 interrupt request for noise detect.
 * @param None
 * @retval None
 */
void AUDIO_IN_SAIx_DMAx_IRQHandler(void)
{
    HAL_DMA_IRQHandler(haudio_in_sai.hdmarx);
}

/**
 * @brief  This function handles External line 15_10 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI15_10_IRQHandler(void)
{
    /* Interrupt handler shared between SD_DETECT pin, USER_KEY button and touch screen interrupt */
    if (__HAL_GPIO_EXTI_GET_IT(SD_DETECT_PIN) != RESET) {
        HAL_GPIO_EXTI_IRQHandler(
        SD_DETECT_PIN | TS_INT_PIN | AUDIO_IN_INT_GPIO_PIN); /* SD detect event or touch screen interrupt */
    } else { /* User button event or Touch screen interrupt */
        HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN);
    }
}

/**
 * @brief  This function handles LTDC global interrupt request.
 * @param  None
 * @retval None
 */
void LTDC_IRQHandler(void)
{
    HAL_LTDC_IRQHandler(&hltdc);
}
