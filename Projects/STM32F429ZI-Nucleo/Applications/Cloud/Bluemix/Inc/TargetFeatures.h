/**
  ******************************************************************************
  * @file    TargetFeatures.h 
  * @author  Central LAB
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   Specification of the HW Features for each target platform
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
#ifndef _TARGET_FEATURES_H_
#define _TARGET_FEATURES_H_

/** @addtogroup FFT_DEMO_NUCLEO_F429ZI
  * @{
  */


#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_UART.h"
#include "stm32f4xx_I2C.h"
#include "stm32f4xx_periph_conf.h"

#include "x_nucleo_iks01a2.h"
#include "x_nucleo_iks01a2_accelero.h"
#include "x_nucleo_iks01a2_gyro.h"
#include "x_nucleo_iks01a2_magneto.h"
#include "x_nucleo_iks01a2_humidity.h"
#include "x_nucleo_iks01a2_temperature.h"
#include "x_nucleo_iks01a2_pressure.h"
   
#define MAX_TEMP_SENSORS 2

/**
 * @brief  Target's Features data structure definition
 */
typedef struct
{
  int32_t NumTempSensors;
  void *HandleTempSensors[MAX_TEMP_SENSORS];

  void *HandlePressSensor;
  void *HandleHumSensor;

  uint32_t HWAdvanceFeatures;
  void *HandleAccSensor;
  void *HandleGyroSensor;
  void *HandleMagSensor;

  uint8_t SnsAltFunc;
  uint8_t EmulateSns;
  RTC_HandleTypeDef RtcHandle;
  uint32_t TIM_CC1_Pulse;
  uint32_t NFCInitialized;

  RNG_HandleTypeDef  RngHandle;
} TargetFeatures_t;

/* Exported variables ------------------------------------------------------- */
extern TargetFeatures_t TargetBoardFeatures;

#ifdef __cplusplus
}
#endif

   

/**
 * @}
 */

#endif /* _TARGET_FEATURES_H_ */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/

