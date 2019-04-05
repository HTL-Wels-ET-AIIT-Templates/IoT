/**
 ******************************************************************************
 * @file    H3LIS331DL_ACC_driver_HL.h
 * @author  MEMS Application Team
 * @version V4.1.0
 * @date    01-November-2017
 * @brief   This file contains definitions for the H3LIS331DL_ACC_driver_HL.c firmware driver
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
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
#ifndef __H3LIS331DL_ACC_DRIVER_HL_H
#define __H3LIS331DL_ACC_DRIVER_HL_H

#ifdef __cplusplus
extern "C" {
#endif



/* Includes ------------------------------------------------------------------*/

#include "accelerometer.h"

/* Include accelero sensor component drivers. */
#include "H3LIS331DL_ACC_driver.h"



/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup COMPONENTS COMPONENTS
 * @{
 */

/** @addtogroup H3LIS331DL H3LIS331DL
 * @{
 */

/** @addtogroup H3LIS331DL_Public_Constants Public constants
 * @{
 */

/** @addtogroup H3LIS331DL_ACC_SENSITIVITY Accelero sensitivity values based on selected full scale
 * @{
 */

#define H3LIS331DL_ACC_SENSITIVITY_FOR_FS_100G   49  /**< Sensitivity value for 100g full scale [mg/digit] */
#define H3LIS331DL_ACC_SENSITIVITY_FOR_FS_200G   98  /**< Sensitivity value for 200g full scale [mg/digit] */
#define H3LIS331DL_ACC_SENSITIVITY_FOR_FS_400G  195  /**< Sensitivity value for 400g full scale [mg/digit] */

/**
 * @}
 */

/**
 * @}
 */

/** @addtogroup H3LIS331DL_Public_Types H3LIS331DL Public Types
 * @{
 */

#if 0 /* _NOTE_: Remove this line if you plan to use this structure below */
/**
 * @brief H3LIS331DL accelero extended features driver internal structure definition
 */
typedef struct
{
  /* _NOTE_: Possible to add the functions for extended features of the sensor here */
} H3LIS331DL_ExtDrv_t;
#endif /* _NOTE_: Remove this line if you plan to use this structure above */

/**
 * @brief H3LIS331DL specific data internal structure definition
 */
typedef struct
{
  float Previous_ODR;
} H3LIS331DL_Data_t;

/**
 * @}
 */

/** @addtogroup H3LIS331DL_Public_Variables Public variables
 * @{
 */

extern ACCELERO_Drv_t H3LIS331DL_Drv;
#if 0 /* _NOTE_: Remove this line if you plan to use this structure below */
extern H3LIS331DL_ExtDrv_t H3LIS331DL_ExtDrv;
#endif /* _NOTE_: Remove this line if you plan to use this structure above */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __H3LIS331DL_ACC_DRIVER_HL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
