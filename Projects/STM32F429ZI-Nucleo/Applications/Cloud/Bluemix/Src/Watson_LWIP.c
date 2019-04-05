/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   This sample code implements a http server application based on 
  *          Netconn API of LwIP stack and FreeRTOS. This application uses 
  *          STM32F4xx the ETH HAL API to transmit and receive data. 
  *          The communication is done with a web browser of a remote PC.
  ******************************************************************************
  * @attention
  *
 * @attention
 *
  * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Watson_LWIP.h"

/** @addtogroup FFT_DEMO_NUCLEO_F429ZI
  * @{
  */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern struct netif gnetif; /* network interface structure */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void StartThread(void const * argument);
void Netif_Config(void);
void Error_Handler(void);

/* Global I2C handle */
extern UART_HandleTypeDef UartHandle;
uint8_t gUseDhcp;

#if (defined USE_STM32F4XX_NUCLEO)
void InitTimers(void);
void InitRTC(void);
void InitHardware(void);
#endif // USE_STM32F4XX_NUCLEO

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Watson_LWIP
  * @param  None
  * @retval None
  */
void Watson_LWIP(void const * argument)
{
  gUseDhcp = 0;
  InitHardware();

  platform_init();
  bluemix_test();
  platform_deinit();

}

/**
  * @brief  HardwareInit
  * Initializes HAL, UART, I2C, GPIOs (PushButton, LED), RTC
  * @param  None
  * @retval None
  */
void InitHardware(void) {
  unsigned int random_number = 0;
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();      
  
  /* Reset all the Target's Features */
  memset(&TargetBoardFeatures, 0, sizeof(TargetFeatures_t)); 
  
  __HAL_RCC_GPIOF_CLK_ENABLE();
  
  /* UART Initialization */
  if(UART_Global_Init()!=HAL_OK) {
	Error_Handler();
  } else {
	printf("UART Initialized\r\n");
  }

  /* I2C Initialization */
  if(I2C_Global_Init()!=HAL_OK) {
	printf("I2C Init Error \r\n");
	Error_Handler();
  }
  
  // TODO: BB make configuration of button and LED student readable
  /* Initialize button */
  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);  
  
  /* Initialize LED */
  BSP_LED_Init(LED1);  
  BSP_LED_Init(LED2);  
  BSP_LED_Init(LED3);  

  /* configure the timers  */
  InitTimers();
  
  /* Initialize random number generartion */
  TargetBoardFeatures.RngHandle.Instance = RNG;
  //hrng.Instance = TargetBoardFeatures.RngHandle.Instance;
  if (HAL_RNG_Init(&TargetBoardFeatures.RngHandle) != HAL_OK)
  {
	Error_Handler();
  }  
  
  /* initialize Real Time Clock */
  InitRTC();

  /* Initialize the seed of the stdlib rand() SW implementation from the RNG. */
  if (HAL_RNG_GenerateRandomNumber(&TargetBoardFeatures.RngHandle, (uint32_t *) &random_number) == HAL_OK)
  {
    srand(random_number);
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

void Error_Handler(void)
{
  while(1)
  {
      BSP_LED_On(LED2);
      HAL_Delay(200);
      BSP_LED_Off(LED2);
      HAL_Delay(400);
  }
}

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
