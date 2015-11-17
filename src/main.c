/* Includes ------------------------------------------------------------------*/
#include <usbd_cdc_if.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "usb_device.h"
#include "main.h"
#include "morse.h"

#define MSG_RECEIVED               "MESSAGE RECEIVED\nRETRANSMITTING VIA MORSE CODE\n"
#define MSG_READY                  "READY TO RECEIVE MESSAGE\n"
#define MSG_MORSE_CODE_TRANSMITTED "MESSAGE RETRANSMITTED VIA MORSE CODE\n"

volatile int morse_code_active = 0;
TIM_HandleTypeDef TimHandle;
uint32_t uwPrescalerValue = 0;
char message[1024];

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

int main(void) {
    memset(message, 0, 1024);
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    /* Configure the system clock */
    SystemClock_Config();
    /* Initialize timer */
    uwPrescalerValue = (uint32_t)((SystemCoreClock / 2) / 10000) - 1;
    TimHandle.Instance = TIM3;
    TimHandle.Init.Period = 1000 - 1;
    TimHandle.Init.Prescaler = uwPrescalerValue;
    TimHandle.Init.ClockDivision = 0;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&TimHandle);
    HAL_TIM_Base_Start_IT(&TimHandle);
    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USB_DEVICE_Init();
    /* Allow USB subsys to settle and send initial prompt */
    HAL_Delay(500);
    CDC_Transmit_FS((uint8_t *) MSG_READY, strlen(MSG_READY));

    /* Infinite loop */
    while (1) {
    }
}

/**
 * System Clock Configuration
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 15;
    RCC_OscInitStruct.PLL.PLLN = 144;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

    if (HAL_GetREVID() == 0x1001) {
        /* Enable the Flash prefetch */
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    }
}

/**
 * GPIO Configuration
 */
void MX_GPIO_Init(void) {
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    /*Configure GPIO pins : LED1 Pin, LED2 Pin */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/**
 * A callback that notifies that data has arrived and should be played as morse code.
 */
void data_has_arrived(char *data) {
    morse_code_active = 1;
    strncpy(message, data, 1024);
    strupr(message);
    memset(data, '\0', 512);
}

/**
 * A callback when timer period has elapsed.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (morse_code_active == 1) {
        NVIC_DisableIRQ(TIM3_IRQn);
        CDC_Transmit_FS((uint8_t *) MSG_RECEIVED, strlen(MSG_RECEIVED));
        play_message(message);
        CDC_Transmit_FS((uint8_t *) MSG_MORSE_CODE_TRANSMITTED, strlen(MSG_MORSE_CODE_TRANSMITTED));
        HAL_Delay(100);
        CDC_Transmit_FS((uint8_t *) MSG_READY, strlen(MSG_READY));
        NVIC_EnableIRQ(TIM3_IRQn);
        morse_code_active = 0;
    }
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
