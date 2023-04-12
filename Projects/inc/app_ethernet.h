/**
  ******************************************************************************
  * @file    app_ethernet.h 
  * @author  MCD Application Team
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   Header for app_ethernet.c module
  ******************************************************************************
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
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_ETHERNET_H
#define __APP_ETHERNET_H

/** @addtogroup FFT_DEMO_NUCLEO_F429ZI
  * @{
  */


#ifdef __cplusplus
 extern "C" {
#endif
   
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/ 
#define USE_DHCP       /* enable DHCP, if disabled static address is used*/
 
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */   
   

/* Includes ------------------------------------------------------------------*/
#include "lwip/netif.h"
//#include "user.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* DHCP process states */
#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5
   
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#ifdef USE_DHCP
void DHCP_thread(void const * argument);
#endif

#ifdef __cplusplus
}
#endif
   

/**
 * @}
 */
#endif /* __APP_ETHERNET_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

