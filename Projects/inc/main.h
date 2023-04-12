/**
  ******************************************************************************
  * @file    main.h
  * @author  Central LAB
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
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
   
#endif  // STM32L475xx

/* Includes ------------------------------------------------------------------*/
	 
#ifdef USE_STM32L0XX_NUCLEO
#include "stm32l0xx_hal.h"
#include "stm32l0xx_nucleo.h"
#include "stm32l0xx_hal_rcc.h"
#include "stm32l0xx_hal_rcc_ex.h"
#endif
#ifdef USE_STM32F1xx_NUCLEO
#include "stm32f1xx_hal.h"
#include "stm32f1xx_nucleo.h"
#include "stm32f1xx_hal_rcc.h"
#endif
#ifdef USE_STM32F4XX_NUCLEO
#include "stm32f4xx_hal.h"
  #ifdef STM32F429xx
    #include "stm32f4xx_nucleo_144.h"
  #else /* STM32F429xx */
    #include "stm32f4xx_nucleo.h"
  #endif /* STM32F429xx */
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_rcc_ex.h"
#include "nucleo_com.h"
#endif  
#ifdef USE_STM32L4XX_NUCLEO
#include "stm32l4xx_hal.h"
#include "stm32l4xx_nucleo.h"
#include "stm32l4xx_hal_rcc.h"
#include "stm32l4xx_hal_rcc_ex.h"
#include "nucleo_com.h"
#include "Watson_WIFI.h"
#include "wifi_globals.h"   
#endif    
   
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
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
#include "nucleo_com.h"
   
#ifdef USE_WIFI
  #include "Watson_WIFI.h"
#else /* USE_WIFI */
#ifdef STM32F429xx
  #include "cmsis_os.h"
  #include "iotDevice_LWIP.h"
extern __IO uint8_t DHCP_state;   
#endif /* STM32F429xx */
#endif /* USE_WIFI */   

#include "TargetFeatures.h"
//#include "MetaDataManager.h"


uint32_t user_currentTimeGetTick(void);

#ifdef USE_MBED_TLS
#include "iot_mbedtls_config.h"
extern int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen );
#endif /* USE_MBED_TLS */

void BSP_COM_Init(COM_TypeDef COM, UART_HandleTypeDef *huart);
 
 
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if defined(USE_WIFI_IOT01)
#define NET_IF  NET_IF_WLAN

#elif USE_WIFI_SPWF01
#define NET_IF  NET_IF_WLAN_SPWF01

#elif USE_WIFI_SPWF04
#define NET_IF  NET_IF_WLAN_SPWF04
#elif defined(USE_LWIP)
#define NET_IF  NET_IF_ETH

#elif defined(USE_C2C)
#define NET_IF  NET_IF_C2C

#endif

/* Exported functions --------------------------------------------------------*/

void    Periph_Config(void);

extern net_hnd_t         hnet;

extern void *ACCELERO_handle;

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

extern volatile uint8_t DataLoggerActive; /*!< DataLogger Flag */
extern volatile uint32_t Int_Current_Time1; /*!< Int_Current_Time1 Value */
extern volatile uint32_t Int_Current_Time2; /*!< Int_Current_Time2 Value */
extern volatile uint8_t AXL_INT_received;
extern volatile uint8_t useTerm;
extern volatile uint8_t funcChanged;

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
