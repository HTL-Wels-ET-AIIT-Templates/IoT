/**
  ******************************************************************************
  * @file    sensors_data.c
  * @author  MCD Application Team
  * @version	3.0.0
  * @date	20-July-2018
  * @brief   Manage sensors of STM32L475 IoT board.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sensors_data.h"

#ifdef STM32L476xx
#include "stm32l4xx_hal_i2c.h"
#endif // STM32L476xx

#if defined (USE_WIFI_IOT01)
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include "vl53l0x_proximity.h"
#include "msg.h"
#elif (defined USE_STM32F4XX_NUCLEO) || (defined USE_STM32L4XX_NUCLEO)
#include "IBM_Watson_Config.h"
#include "msg.h"
#include "fft.h"
/* Sensors. Private variables ---------------------------------------------------------*/
void *ACCELERO_handle = NULL;
void *ACCELERO_handle_LSM6DSL = NULL;
void *GYRO_handle = NULL;
void *MAGNETO_handle = NULL;
void *HUMIDITY_handle = NULL;
void *TEMPERATURE_handle = NULL;
void *PRESSURE_handle = NULL;

extern RawTemperatureValues_t gRawTemperatureValues;
//uint8_t acquire_data( void );
//float *get_fft_data( void );
#endif  // USE_WIFI_IOT01

#if  (defined USE_STM32F4XX_NUCLEO)
#include "stm32f4xx_hal.h"
#elif  (defined USE_STM32L4XX_NUCLEO)
#include "stm32l4xx_hal.h"
#endif // defined USE_STM32F4XX_NUCLEO

#include "user.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  init_sensors
  * @param  none
  * @retval 0 in case of success
  *         -1 in case of failure
  */
int init_sensors(void)
{
    int ret = 0;

#ifdef __IKS01A1
    /* Try to use LSM6DS3 DIL24 if present */
    if(BSP_ACCELERO_Init( LSM6DS3_X_0, &ACCELERO_handle)!=COMPONENT_OK){
      /* otherwise try to use LSM6DS on board */
      if(BSP_ACCELERO_Init(LSM6DS0_X_0, &ACCELERO_handle)!=COMPONENT_OK){
         ret = -1;
      }
    }
#else
  /* Try to use IIS2DH, LSM303AGR or LSM6DSL accelerometer */
    if(BSP_ACCELERO_Init( LSM303AGR_X_0, &ACCELERO_handle )!=COMPONENT_OK){
         ret = -1;
    }
#endif

    /* gyro sensor */
    if(BSP_GYRO_Init( GYRO_SENSORS_AUTO, &GYRO_handle )!=COMPONENT_OK){
         ret = -1;
    }

    /* magneto sensor */
    if(BSP_MAGNETO_Init( MAGNETOMETER_SENSOR_AUTO, &MAGNETO_handle )!=COMPONENT_OK){
         ret = -1;
    }

    /* Force to use HTS221 */
    if(BSP_HUMIDITY_Init( HTS221_H_0, &HUMIDITY_handle )!=COMPONENT_OK){
         ret = -1;
    }

    /* Force to use HTS221 */
    if(BSP_TEMPERATURE_Init( HTS221_T_0, &TEMPERATURE_handle )!=COMPONENT_OK){
         ret = -1;
    }

    /* Try to use LPS25HB DIL24 if present, otherwise use LPS25HB on board */
    if(BSP_PRESSURE_Init( PRESSURE_SENSORS_AUTO, &PRESSURE_handle )!=COMPONENT_OK){
         ret = -1;
    }

    /* Set ODR to 400Hz for FFT demo */
    if(BSP_ACCELERO_Set_ODR_Value( ACCELERO_handle, 400.0f ) != COMPONENT_OK)
    {
         ret = -1;
    }

    /*  Enable all the sensors */
    BSP_ACCELERO_Sensor_Enable( ACCELERO_handle );
    BSP_GYRO_Sensor_Enable( GYRO_handle );
    BSP_MAGNETO_Sensor_Enable( MAGNETO_handle );
    BSP_HUMIDITY_Sensor_Enable( HUMIDITY_handle );
    BSP_TEMPERATURE_Sensor_Enable( TEMPERATURE_handle );
    BSP_PRESSURE_Sensor_Enable( PRESSURE_handle );
    
    
    // below code id for LSM303AGR only
//      if(BSP_ACCELERO_Enable_HP_Filter_Ext( ACCELERO_handle, HPF_MODE_NORMAL, CUTOFF_MODE2) != COMPONENT_OK)
//      {
//        return 0;
//      }
//      if(BSP_ACCELERO_Set_Active_Axis_Ext( ACCELERO_handle, Z_AXIS ) != COMPONENT_OK)
//      {
//        return 0;
//      }
//      if(BSP_ACCELERO_Set_INT1_DRDY_Ext(ACCELERO_handle, INT1_DRDY_ENABLED) != COMPONENT_OK)
//      {
//        return 0;
//      }
      
      /* Set ODR to 400Hz for FFT demo */
//      if(BSP_ACCELERO_Set_ODR_Value( ACCELERO_handle, 400.0f ) != COMPONENT_OK)
//      {
//        return 0;
//      }
      
//      if(BSP_ACCELERO_Set_INT1_DRDY_Ext(ACCELERO_handle, INT1_DRDY_DISABLED) != COMPONENT_OK)
//      {
//        return 0;
//      }    
      
//      if(BSP_ACCELERO_Set_Active_Axis_Ext(ACCELERO_handle, ALL_ACTIVE) != COMPONENT_OK)
//      {
//        return 0;
//      }           
      
    

#if (defined FFT_DEMO)
    if(0 == init_fft( TERMINAL ) )
    {
      printf("fft library initialized\n\r");     
      
//      if(BSP_ACCELERO_Set_Active_Axis_Ext(ACCELERO_handle, ALL_ACTIVE) != COMPONENT_OK)
//      {
//        return 0;
//      }         
    }
    else
    {
      printf("FFT library init failed...\n\rPlease restart nucleo board.\n\r");
      ret = -2;
    }
#endif // FFT_DEMO

    if (userInit() != 0) {
      ret = -3;
    }

    return ret;
}

RawTemperatureValues_t SensorGetTemperature(void) {
  float value;
    
  BSP_TEMPERATURE_Get_Temp(TEMPERATURE_handle,(float *)&value);
  // after calling this function the variable gRawTemperatureValues contains
  // values from the HTS221 sensor
  return gRawTemperatureValues;
}

float SensorGetHumidity(void) {
  float value;
  BSP_HUMIDITY_Get_Hum(HUMIDITY_handle,(float *)&value);
  return value;
}

float SensorGetPressure(void) {
  float value;
  BSP_PRESSURE_Get_Press(PRESSURE_handle,(float *)&value);
  return value;
}

SensorAxes_t SensorGetAcceleration(void) {
  SensorAxes_t sensValue = {-1, -1, -1};
  
  if(BSP_ACCELERO_Set_Active_Axis_Ext( ACCELERO_handle, ALL_ACTIVE ) != COMPONENT_OK)
  {
    return sensValue;
  }  
  HAL_Delay(100);
  /* ACC_Value[0] = X axis, ACC_Value[1] = Y axis, ACC_Value[2] = Z axis */
  BSP_ACCELERO_Get_Axes(ACCELERO_handle, &sensValue);

  BSP_ACCELERO_Set_Active_Axis_Ext( ACCELERO_handle, Z_AXIS );
  return sensValue;
}

SensorAxes_t SensorGetGyro(void) {
  SensorAxes_t sensValue = {-1, -1, -1};
  
  if(BSP_ACCELERO_Set_Active_Axis_Ext( ACCELERO_handle, ALL_ACTIVE ) != COMPONENT_OK)
  {
    return sensValue;
  }  
  HAL_Delay(100);
  /* ACC_Value[0] = X axis, ACC_Value[1] = Y axis, ACC_Value[2] = Z axis */
  BSP_GYRO_Get_Axes(GYRO_handle, &sensValue);

  BSP_ACCELERO_Set_Active_Axis_Ext( ACCELERO_handle, Z_AXIS );
  return sensValue;
}

SensorAxes_t SensorGetMagneto(void) {
  SensorAxes_t sensValue = {-1, -1, -1};
  
  if(BSP_ACCELERO_Set_Active_Axis_Ext( ACCELERO_handle, ALL_ACTIVE ) != COMPONENT_OK)
  {
    return sensValue;
  }  
  HAL_Delay(100);
  /* ACC_Value[0] = X axis, ACC_Value[1] = Y axis, ACC_Value[2] = Z axis */
  BSP_MAGNETO_Get_Axes(MAGNETO_handle, &sensValue);

  BSP_ACCELERO_Set_Active_Axis_Ext( ACCELERO_handle, Z_AXIS );
  return sensValue;
}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
