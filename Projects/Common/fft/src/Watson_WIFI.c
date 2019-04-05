/**
  ******************************************************************************
  * @file    Watson_WIFI.c
  * @author  MCD Application Team
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   Main program body.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Watson_WIFI.h"
#include "IBM_Watson_Config.h"
#include "wifi_globals.h"

/* Global variables ---------------------------------------------------------*/
#ifdef HAL_RNG_MODULE_ENABLED
RNG_HandleTypeDef hrng;
#endif //HAL_RNG_MODULE_ENABLED

void RTC_Init(void);
int Console_UART_Init(void);
void cloud_test(void const *arg);
void SystemClock_Config(void);

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef console_uart;


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval Status
  */
int Watson_WIFI(void)
{
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
  
  Periph_Config();
  
  /* UART Initialization */  
  if(Console_UART_Init()!=HAL_OK) {
    Error_Handler();
  } else {
    printf("UART Initialized\r\n");
  }  

  BSP_LED_Init(LED_GREEN);
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
#ifdef HAL_RNG_MODULE_ENABLED
  /* RNG init function */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
#endif //HAL_RNG_MODULE_ENABLED
  /* RTC init */
  InitRTC();

  /* configure the timers  */
  Timer_Config( );
 
//  /* UART console init */
//  Console_UART_Init();

#if defined(SENSOR) && defined(USE_STM32F4XX_NUCLEO)
  /* I2C Initialization */
  if(I2C_Global_Init()!=HAL_OK) {
    printf("I2C Init Error \r\n");
    Error_Handler();
  }
//#endif //  FFT_DEMO
#endif //   definrf(SENSOR) & defined(USE_STM32F4XX_NUCLEO)

#ifdef FIREWALL_MBEDLIB
  firewall_init();
#endif

  cloud_test(0);
}

/**
  * @brief UART console init function
  */
#define USART_PRINT_MSG_BAUDRATE 115200

int Console_UART_Init(void)
{


  console_uart.Instance = NUCLEO_COM1;
  console_uart.Init.BaudRate = USART_PRINT_MSG_BAUDRATE;
  console_uart.Init.WordLength = UART_WORDLENGTH_8B;
  console_uart.Init.StopBits = UART_STOPBITS_1;
  console_uart.Init.Parity = UART_PARITY_NONE;
  console_uart.Init.Mode = UART_MODE_TX_RX;
  console_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  console_uart.Init.OverSampling = UART_OVERSAMPLING_16;
#ifdef UART_ONE_BIT_SAMPLE_DISABLE
  console_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  console_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
#endif
  BSP_COM_Init(COM1,&console_uart);
  
  return HAL_OK;

}
#if (defined(__GNUC__) && !defined(__CC_ARM))
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  while (HAL_OK != HAL_UART_Transmit(&console_uart, (uint8_t *) &ch, 1, 30000))
  {
    ;
  }
  return ch;
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
  uint8_t ch = 0;
  while (HAL_OK != HAL_UART_Receive(&console_uart, (uint8_t *)&ch, 1, 30000))
  {
    ;
  }
  return ch;
}





/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
#ifdef STM32F429xx
void Error_Handler(void)
{
  while(1)
  {
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(200);
  }
}
#endif // STM32F429xx

//#ifdef USE_STM32F4XX_NUCLEO
//#ifdef USART_PRINT_MSG
//
//#define USART_PRINT_MSG_BAUDRATE 460800
//#define WIFI_UART_MSG                           USART2
//extern UART_HandleTypeDef UartMsgHandle;
//
//#define WiFi_USART_PRINT_TX_PIN                    GPIO_PIN_2
//#define WiFi_USART_PRINT_TX_GPIO_PORT              GPIOA
//#define WiFi_USART_PRINT_RX_PIN                    GPIO_PIN_3
//#define WiFi_USART_PRINT_RX_GPIO_PORT              GPIOA
//#define USARTx_PRINT_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
//#define USARTx_PRINT_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
//#define USARTx_PRINT_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
//
//#define USARTx_PRINT_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
//#define USARTx_PRINT_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()
//
//#define PRINTMSG_USARTx_TX_AF                       GPIO_AF7_USART2
//#define PRINTMSG_USARTx_RX_AF                       GPIO_AF7_USART2
//
//void UART_Msg_Gpio_Init()
//{
//  GPIO_InitTypeDef  GPIO_InitStruct;
//
//  /*##-1- Enable peripherals and GPIO Clocks #################################*/
//  /* Enable GPIO TX/RX clock */
//  USARTx_PRINT_TX_GPIO_CLK_ENABLE();
//  USARTx_PRINT_RX_GPIO_CLK_ENABLE();
//
//
//  /* Enable USARTx clock */
//  USARTx_PRINT_CLK_ENABLE();
//    __SYSCFG_CLK_ENABLE();
//  /*##-2- Configure peripheral GPIO ##########################################*/
//  /* UART TX GPIO pin configuration  */
//  GPIO_InitStruct.Pin       = WiFi_USART_PRINT_TX_PIN;
//  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Pull      = GPIO_PULLUP;
//  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
//#if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
//  GPIO_InitStruct.Alternate = PRINTMSG_USARTx_TX_AF;
//#endif
//  HAL_GPIO_Init(WiFi_USART_PRINT_TX_GPIO_PORT, &GPIO_InitStruct);
//
//  /* UART RX GPIO pin configuration  */
//  GPIO_InitStruct.Pin = WiFi_USART_PRINT_RX_PIN;
//  GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
//#if defined (USE_STM32L0XX_NUCLEO) || (USE_STM32F4XX_NUCLEO) || (USE_STM32L4XX_NUCLEO)
//  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
//  GPIO_InitStruct.Alternate = PRINTMSG_USARTx_RX_AF;
//#endif
//
//  HAL_GPIO_Init(WiFi_USART_PRINT_RX_GPIO_PORT, &GPIO_InitStruct);
//
//#ifdef WIFI_USE_VCOM
//  /*##-3- Configure the NVIC for UART ########################################*/
//  /* NVIC for USART */
//  HAL_NVIC_SetPriority(USARTx_PRINT_IRQn, 0, 1);
//  HAL_NVIC_EnableIRQ(USARTx_PRINT_IRQn);
//#endif
//}
//#endif
//#endif //USE_STM32F4XX_NUCLEO




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
