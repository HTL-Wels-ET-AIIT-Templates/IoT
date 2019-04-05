/**
  ******************************************************************************
  * @file    common.h 
  * @author  MMY Application Team
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
  *
  * Licensed under MMY-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMON_H
#define __COMMON_H

/* Includes ------------------------------------------------------------------*/
#include "x_nucleo_nfc04a1_nfctag.h"
#include "version.h"
#include <string.h>

/* Exported types ------------------------------------------------------------*/
typedef uint8_t boolean;

/**
 * @brief  GPO status information structure definition
 */
typedef struct
{
  uint8_t WritenEEPROM;
  uint8_t RfBusy;
  uint8_t FieldOn;
  uint8_t FieldOff;
  uint8_t MsgInMailbox;
  uint8_t MailboxMsgRead;
  uint8_t RfInterrupt;
  uint8_t Rfuser;
} IT_GPO_STATUS;

/* Exported macro ------------------------------------------------------------*/
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif  // MIN

#undef FAIL
#define FAIL 0

#undef PASS
#define PASS !FAIL

/* Exported constants --------------------------------------------------------*/
#define MAX_NDEF_MEM                 0x200
#define ST25DV_MAX_SIZE              NFCTAG_4K_SIZE
#define ST25DV_NDEF_MAX_SIZE         MIN(ST25DV_MAX_SIZE,MAX_NDEF_MEM)
#define NFC_DEVICE_MAX_NDEFMEMORY    ST25DV_NDEF_MAX_SIZE

#define RESULTOK                     0x00
#define ERRORCODE_GENERIC            1

/* Exported functions ------------------------------------------------------- */

#endif /* __COMMON_H */

/************************ (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
