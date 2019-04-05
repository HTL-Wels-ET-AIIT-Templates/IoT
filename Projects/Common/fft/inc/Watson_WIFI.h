/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   main application header file.
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
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
#define __main_h__
#ifdef __cplusplus
 extern "C" {
#endif

#if defined (STM32L475xx)
#include "stm32l4xx_hal.h"  
#include "stm32l475e_iot01.h"

#ifdef SENSOR
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "vl53l0x_proximity.h"
#endif  // SENSOR
   
#else
#include "cube_hal.h"
#include "x_nucleo_iks01a2.h"
#include "x_nucleo_iks01a2_accelero.h"   
#endif // STM32L475xx

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "timedate.h"
#include "flash.h"
#ifdef FIREWALL_MBEDLIB
#include "firewall_wrapper.h"
#include "firewall.h"
#endif /* FIREWALL_MBEDLIB */
#include "net.h"
#include "iot_flash_config.h"
#include "msg.h"
#include "cloud.h"
#include "sensors_data.h"


#ifdef USE_MBED_TLS
#include "bluemix_mbedtls_config.h"
extern int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen );
#endif /* USE_MBED_TLS */
 
 
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if defined(USE_WIFI_IOT01)
#define NET_IF  NET_IF_WLAN

#elif USE_WIFI_SPWF04
#define NET_IF  NET_IF_WLAN_SPWF04

#elif USE_WIFI_SPWF01
#define NET_IF  NET_IF_WLAN_SPWF01

#elif defined(USE_LWIP)
#define NET_IF  NET_IF_ETH

#elif defined(USE_C2C)
#define NET_IF  NET_IF_C2C

#elif defined(USE_LWIP)
#define NET_IF  NET_IF_ETH

#endif

enum {BP_NOT_PUSHED=0, BP_SINGLE_PUSH, BP_MULTIPLE_PUSH};

/* Exported functions --------------------------------------------------------*/


//void    Error_Handler(void);
uint8_t Button_WaitForPush(uint32_t timeout);
void    Led_On(void);
void    Led_Off(void);
void    Led_Blink(int period, int duty, int count);
void    Periph_Config(void);

#ifdef HAL_RNG_MODULE_ENABLED
extern RNG_HandleTypeDef hrng;
#endif //HAL_RNG_MODULE_ENABLED
extern RTC_HandleTypeDef hrtc;
extern net_hnd_t         hnet;

/* IAR */
#ifdef __ICCARM__  
extern const user_config_t lUserConfig;
/* Keil / armcc */
#elif defined ( __CC_ARM   )
extern user_config_t lUserConfig;
/* GNU Compiler */
#elif defined ( __GNUC__ )     
extern user_config_t lUserConfig;
#endif

#ifdef FFT_DEMO
uint32_t user_currentTimeGetTick(void);
uint32_t user_currentTimeGetElapsedMS(uint32_t Tick1);
void RTC_TimeRegulate(uint8_t hh, uint8_t mm, uint8_t ss);
#endif  // FFT_DEMO


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
