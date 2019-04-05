/**
 ******************************************************************************
 * @file    LIS2DW12_ACC_driver_HL.c
 * @author  MEMS Application Team
 * @version V4.1.0
 * @date    01-November-2017
 * @brief   This file provides a set of high-level functions needed to manage
            the LIS2DW12 sensor
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

/* Includes ------------------------------------------------------------------*/

#include "LIS2DW12_ACC_driver_HL.h"



/** @addtogroup BSP BSP
 * @{
 */

/** @addtogroup COMPONENTS COMPONENTS
 * @{
 */

/** @addtogroup LIS2DW12 LIS2DW12
 * @{
 */

/** @addtogroup LIS2DW12_Private_Function_Prototypes Private function prototypes
 * @{
 */

static DrvStatusTypeDef LIS2DW12_Set_ODR_When_Enabled( DrvContextTypeDef *handle, SensorOdr_t odr );
static DrvStatusTypeDef LIS2DW12_Set_ODR_When_Disabled( DrvContextTypeDef *handle, SensorOdr_t odr );
static DrvStatusTypeDef LIS2DW12_Set_ODR_Value_When_Enabled( DrvContextTypeDef *handle, float odr );
static DrvStatusTypeDef LIS2DW12_Set_ODR_Value_When_Disabled( DrvContextTypeDef *handle, float odr );

/**
 * @}
 */

/** @addtogroup LIS2DW12_Callable_Private_Function_Prototypes Callable private function prototypes
 * @{
 */

static DrvStatusTypeDef LIS2DW12_Init( DrvContextTypeDef *handle );
static DrvStatusTypeDef LIS2DW12_DeInit( DrvContextTypeDef *handle );
static DrvStatusTypeDef LIS2DW12_Sensor_Enable( DrvContextTypeDef *handle );
static DrvStatusTypeDef LIS2DW12_Sensor_Disable( DrvContextTypeDef *handle );
static DrvStatusTypeDef LIS2DW12_Get_WhoAmI( DrvContextTypeDef *handle, uint8_t *who_am_i );
static DrvStatusTypeDef LIS2DW12_Check_WhoAmI( DrvContextTypeDef *handle );
static DrvStatusTypeDef LIS2DW12_Get_Axes( DrvContextTypeDef *handle, SensorAxes_t *acceleration );
static DrvStatusTypeDef LIS2DW12_Get_AxesRaw( DrvContextTypeDef *handle, SensorAxesRaw_t *value );
static DrvStatusTypeDef LIS2DW12_Get_Sensitivity( DrvContextTypeDef *handle, float *sensitivity );
static DrvStatusTypeDef LIS2DW12_Get_ODR( DrvContextTypeDef *handle, float *odr );
static DrvStatusTypeDef LIS2DW12_Set_ODR( DrvContextTypeDef *handle, SensorOdr_t odr );
static DrvStatusTypeDef LIS2DW12_Set_ODR_Value( DrvContextTypeDef *handle, float odr );
static DrvStatusTypeDef LIS2DW12_Get_FS( DrvContextTypeDef *handle, float *fullScale );
static DrvStatusTypeDef LIS2DW12_Set_FS( DrvContextTypeDef *handle, SensorFs_t fs );
static DrvStatusTypeDef LIS2DW12_Set_FS_Value( DrvContextTypeDef *handle, float fullScale );
static DrvStatusTypeDef LIS2DW12_Read_Reg( DrvContextTypeDef *handle, uint8_t reg, uint8_t *data );
static DrvStatusTypeDef LIS2DW12_Write_Reg( DrvContextTypeDef *handle, uint8_t reg, uint8_t data );
static DrvStatusTypeDef LIS2DW12_Get_DRDY_Status( DrvContextTypeDef *handle, uint8_t *status );

/**
 * @}
 */

/** @addtogroup LIS2DW12_Public_Variables Public variables
 * @{
 */

/**
 * @brief LIS2DW12 accelero driver structure
 */
ACCELERO_Drv_t LIS2DW12_Drv =
{
  LIS2DW12_Init,
  LIS2DW12_DeInit,
  LIS2DW12_Sensor_Enable,
  LIS2DW12_Sensor_Disable,
  LIS2DW12_Get_WhoAmI,
  LIS2DW12_Check_WhoAmI,
  LIS2DW12_Get_Axes,
  LIS2DW12_Get_AxesRaw,
  LIS2DW12_Get_Sensitivity,
  LIS2DW12_Get_ODR,
  LIS2DW12_Set_ODR,
  LIS2DW12_Set_ODR_Value,
  LIS2DW12_Get_FS,
  LIS2DW12_Set_FS,
  LIS2DW12_Set_FS_Value,
  0,
  0,
  LIS2DW12_Read_Reg,
  LIS2DW12_Write_Reg,
  LIS2DW12_Get_DRDY_Status
};

/**
 * @}
 */

/** @addtogroup LIS2DW12_Callable_Private_Functions Callable private functions
 * @{
 */

/**
 * @brief Initialize the LIS2DW12 sensor
 * @param handle the device handle
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Init( DrvContextTypeDef *handle )
{

  if ( LIS2DW12_Check_WhoAmI( handle ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Enable BDU. */
  if ( LIS2DW12_ACC_W_BlockDataUpdate( ( void * )handle, LIS2DW12_ACC_BDU_ENABLE ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* FIFO mode selection - FIFO disable. */
  if ( LIS2DW12_ACC_W_FIFO_mode( ( void * )handle, LIS2DW12_ACC_FMODE_BYPASS ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Output data rate selection - power down. */
  if ( LIS2DW12_ACC_W_OutputDataRate( ( void* )handle, LIS2DW12_ACC_ODR_POWER_DOWN ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Set default power mode - High resolution. */
  /* _NOTE_: In case of LIS2DW12_ACC_MODE_LOW_POWER_STD user should also choose one of four Low Power modes */
  if ( LIS2DW12_ACC_W_ModeSelection( ( void * )handle, LIS2DW12_ACC_MODE_HIGH_PERFORMANCE ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Select default output data rate. */
  if ( LIS2DW12_Set_ODR_When_Disabled( handle, ODR_MID_HIGH ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Full scale selection. */
  if ( LIS2DW12_Set_FS( handle, FS_LOW ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  handle->isInitialized = 1;

  return COMPONENT_OK;
}

/**
 * @brief Deinitialize the LIS2DW12 sensor
 * @param handle the device handle
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_DeInit( DrvContextTypeDef *handle )
{
  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
  LIS2DW12_Data_t *pComponentData = ( LIS2DW12_Data_t * )pData->pComponentData;

  if ( LIS2DW12_Check_WhoAmI( handle ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Disable the component */
  if( LIS2DW12_Sensor_Disable( handle ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Reset previous output data rate. */
  pComponentData->Previous_ODR = 0.0f;

  handle->isInitialized = 0;

  return COMPONENT_OK;
}

/**
 * @brief Enable the LIS2DW12 sensor
 * @param handle the device handle
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Sensor_Enable( DrvContextTypeDef *handle )
{
  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
  LIS2DW12_Data_t *pComponentData = ( LIS2DW12_Data_t * )pData->pComponentData;

  /* Check if the component is already enabled */
  if ( handle->isEnabled == 1 )
  {
    return COMPONENT_OK;
  }

  /* Output data rate selection. */
  if ( LIS2DW12_Set_ODR_Value_When_Enabled( handle, pComponentData->Previous_ODR ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  handle->isEnabled = 1;

  return COMPONENT_OK;
}

/**
 * @brief Disable the LIS2DW12 sensor
 * @param handle the device handle
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Sensor_Disable( DrvContextTypeDef *handle )
{
  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
  LIS2DW12_Data_t *pComponentData = ( LIS2DW12_Data_t * )pData->pComponentData;

  /* Check if the component is already disabled */
  if ( handle->isEnabled == 0 )
  {
    return COMPONENT_OK;
  }

  /* Store actual output data rate. */
  if ( LIS2DW12_Get_ODR( handle, &( pComponentData->Previous_ODR ) ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Output data rate selection - power down. */
  if ( LIS2DW12_ACC_W_OutputDataRate( ( void* )handle, LIS2DW12_ACC_ODR_POWER_DOWN ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  handle->isEnabled = 0;

  return COMPONENT_OK;
}

/**
 * @brief Get the WHO_AM_I ID of the LIS2DW12 sensor
 * @param handle the device handle
 * @param who_am_i pointer to the value of WHO_AM_I register
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Get_WhoAmI( DrvContextTypeDef *handle, uint8_t *who_am_i )
{
  /* Read WHO AM I register */
  if ( LIS2DW12_ACC_R_WhoAmI( ( void * )handle, ( uint8_t* )who_am_i ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief Check the WHO_AM_I ID of the LIS2DW12 sensor
 * @param handle the device handle
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Check_WhoAmI( DrvContextTypeDef *handle )
{
  uint8_t who_am_i = 0x00;

  if ( LIS2DW12_Get_WhoAmI( handle, &who_am_i ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  if ( who_am_i != handle->who_am_i )
  {
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief Get the LIS2DW12 accelerometer sensor axes
 * @param handle the device handle
 * @param acceleration pointer to where acceleration data write to
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Get_Axes( DrvContextTypeDef *handle, SensorAxes_t *acceleration )
{
  SensorAxesRaw_t dataRaw;
  float sensitivity = 0;

  /* Read raw data from LIS2DW12 output register. */
  if ( LIS2DW12_Get_AxesRaw( handle, &dataRaw ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Get LIS2DW12 actual sensitivity. */
  if ( LIS2DW12_Get_Sensitivity( handle, &sensitivity ) == COMPONENT_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Calculate the data. */
  acceleration->AXIS_X = ( int32_t )( dataRaw.AXIS_X * sensitivity );
  acceleration->AXIS_Y = ( int32_t )( dataRaw.AXIS_Y * sensitivity );
  acceleration->AXIS_Z = ( int32_t )( dataRaw.AXIS_Z * sensitivity );

  return COMPONENT_OK;
}

/**
 * @brief Get the LIS2DW12 accelerometer sensor raw axes
 * @param handle the device handle
 * @param acceleration_raw pointer to where acceleration raw data write to
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Get_AxesRaw( DrvContextTypeDef *handle, SensorAxesRaw_t *value )
{
  uint8_t regValue[6] = { 0, 0, 0, 0, 0, 0 };
  int16_t dataRaw[3];
  LIS2DW12_ACC_MODE_t mode;
  LIS2DW12_ACC_LP_MODE_t lp_mode;

  /* Read raw data from LIS2DW12 output register. */
  if ( LIS2DW12_ACC_Get_Acceleration( handle, regValue ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  dataRaw[0] = ( ( ( ( int16_t )regValue[1] ) << 8 ) + ( int16_t )regValue[0] );
  dataRaw[1] = ( ( ( ( int16_t )regValue[3] ) << 8 ) + ( int16_t )regValue[2] );
  dataRaw[2] = ( ( ( ( int16_t )regValue[5] ) << 8 ) + ( int16_t )regValue[4] );

  /* Read actual power mode selection from sensor. */
  if ( LIS2DW12_ACC_R_ModeSelection( ( void * )handle, &mode ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }
  if ( LIS2DW12_ACC_R_LowPowerModeSelection( ( void * )handle, &lp_mode ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Select raw data format according to power mode selected. */
  if ( (mode == LIS2DW12_ACC_MODE_LOW_POWER_STD || mode == LIS2DW12_ACC_MODE_LOW_POWER_SINGLE)
      && lp_mode == LIS2DW12_ACC_LP_MODE1_12bit )
  {
    /* 12 bits left justified */
    dataRaw[0] >>= 4;
    dataRaw[1] >>= 4;
    dataRaw[2] >>= 4;
  }
  else
  {
    /* 14 bits left justified */
    dataRaw[0] >>= 2;
    dataRaw[1] >>= 2;
    dataRaw[2] >>= 2;
  }

  /* Set the raw data. */
  value->AXIS_X = dataRaw[0];
  value->AXIS_Y = dataRaw[1];
  value->AXIS_Z = dataRaw[2];

  return COMPONENT_OK;
}

/**
 * @brief Get the LIS2DW12 accelerometer sensor sensitivity
 * @param handle the device handle
 * @param sensitivity pointer to where sensitivity write to
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Get_Sensitivity( DrvContextTypeDef *handle, float *sensitivity )
{
  LIS2DW12_ACC_MODE_t mode;
  LIS2DW12_ACC_LP_MODE_t lp_mode;
  LIS2DW12_ACC_FS_t fullScale;

  /* Read actual power mode selection from sensor. */
  if ( LIS2DW12_ACC_R_ModeSelection( ( void * )handle, &mode ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }
  if ( LIS2DW12_ACC_R_LowPowerModeSelection( ( void * )handle, &lp_mode ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Read actual full scale selection from sensor. */
  if ( LIS2DW12_ACC_R_FullScaleSelection( ( void * )handle, &fullScale ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Store the sensitivity based on actual operating mode and full scale. */
  if ( (mode == LIS2DW12_ACC_MODE_LOW_POWER_STD || mode == LIS2DW12_ACC_MODE_LOW_POWER_SINGLE)
      && lp_mode == LIS2DW12_ACC_LP_MODE1_12bit )
  {
    switch ( fullScale )
    {
      case LIS2DW12_ACC_FS_2g:
        *sensitivity = ( float )LIS2DW12_ACC_SENSITIVITY_FOR_FS_2G_LOPOW1_MODE;
        break;
      case LIS2DW12_ACC_FS_4g:
        *sensitivity = ( float )LIS2DW12_ACC_SENSITIVITY_FOR_FS_4G_LOPOW1_MODE;
        break;
      case LIS2DW12_ACC_FS_8g:
        *sensitivity = ( float )LIS2DW12_ACC_SENSITIVITY_FOR_FS_8G_LOPOW1_MODE;
        break;
      case LIS2DW12_ACC_FS_16g:
        *sensitivity = ( float )LIS2DW12_ACC_SENSITIVITY_FOR_FS_16G_LOPOW1_MODE;
        break;
      default:
        *sensitivity = -1.0f;
        return COMPONENT_ERROR;
    }
  }
  else
  {
    switch ( fullScale )
    {
      case LIS2DW12_ACC_FS_2g:
        *sensitivity = ( float )LIS2DW12_ACC_SENSITIVITY_FOR_FS_2G_OTHER_MODES;
        break;
      case LIS2DW12_ACC_FS_4g:
        *sensitivity = ( float )LIS2DW12_ACC_SENSITIVITY_FOR_FS_4G_OTHER_MODES;
        break;
      case LIS2DW12_ACC_FS_8g:
        *sensitivity = ( float )LIS2DW12_ACC_SENSITIVITY_FOR_FS_8G_OTHER_MODES;
        break;
      case LIS2DW12_ACC_FS_16g:
        *sensitivity = ( float )LIS2DW12_ACC_SENSITIVITY_FOR_FS_16G_OTHER_MODES;
        break;
      default:
        *sensitivity = -1.0f;
        return COMPONENT_ERROR;
    }
  }

  return COMPONENT_OK;
}

/**
 * @brief Get the LIS2DW12 accelerometer sensor output data rate
 * @param handle the device handle
 * @param odr pointer to where output data rate write to
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Get_ODR( DrvContextTypeDef *handle, float *odr )
{
  LIS2DW12_ACC_MODE_t mode;
  LIS2DW12_ACC_ODR_t odr_low_level;

  /* Read actual power mode selection from sensor. */
  if ( LIS2DW12_ACC_R_ModeSelection( ( void * )handle, &mode ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  /* Read actual output data rate selection from sensor. */
  if ( LIS2DW12_ACC_R_OutputDataRate( ( void * )handle, &odr_low_level ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  switch ( odr_low_level )
  {
  case LIS2DW12_ACC_ODR_POWER_DOWN:
    *odr = 0.0f;
    break;
  case LIS2DW12_ACC_ODR_LP_1Hz6_HP_12Hz5:
    switch ( mode )
    {
    case LIS2DW12_ACC_MODE_LOW_POWER_STD:
    case LIS2DW12_ACC_MODE_LOW_POWER_SINGLE:
      *odr = 1.6f;
      break;
    case LIS2DW12_ACC_MODE_HIGH_PERFORMANCE:
      *odr = 12.5f;
      break;
    default:
      *odr = -1.0f;
      return COMPONENT_ERROR;
    }
  case LIS2DW12_ACC_ODR_LP_12Hz5_HP_12Hz5:
    *odr = 12.5f;
    break;
  case LIS2DW12_ACC_ODR_LP_25Hz_HP_25Hz:
    *odr = 25.0f;
    break;
  case LIS2DW12_ACC_ODR_LP_50Hz_HP_50Hz:
    *odr = 50.0f;
    break;
  case LIS2DW12_ACC_ODR_LP_100Hz_HP_100Hz:
    *odr = 100.0f;
    break;
  case LIS2DW12_ACC_ODR_LP_200Hz_HP_200Hz:
    *odr = 200.0f;
    break;
  case LIS2DW12_ACC_ODR_LP_200Hz_HP_400Hz:
    switch ( mode )
    {
    case LIS2DW12_ACC_MODE_LOW_POWER_STD:
    case LIS2DW12_ACC_MODE_LOW_POWER_SINGLE:
      *odr = 200.0f;
      break;
    case LIS2DW12_ACC_MODE_HIGH_PERFORMANCE:
      *odr = 400.0f;
      break;
    default:
      *odr = -1.0f;
      return COMPONENT_ERROR;
    }
  case LIS2DW12_ACC_ODR_LP_200Hz_HP_800Hz:
    switch ( mode )
    {
    case LIS2DW12_ACC_MODE_LOW_POWER_STD:
    case LIS2DW12_ACC_MODE_LOW_POWER_SINGLE:
      *odr = 200.0f;
      break;
    case LIS2DW12_ACC_MODE_HIGH_PERFORMANCE:
      *odr = 800.0f;
      break;
    default:
      *odr = -1.0f;
      return COMPONENT_ERROR;
    }
  case LIS2DW12_ACC_ODR_LP_200Hz_HP_1600Hz:
    switch ( mode )
    {
    case LIS2DW12_ACC_MODE_LOW_POWER_STD:
    case LIS2DW12_ACC_MODE_LOW_POWER_SINGLE:
      *odr = 200.0f;
      break;
    case LIS2DW12_ACC_MODE_HIGH_PERFORMANCE:
      *odr = 1600.0f;
      break;
    default:
      *odr = -1.0f;
      return COMPONENT_ERROR;
    }
  default:
    *odr = -1.0f;
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief Set the LIS2DW12 accelerometer sensor output data rate
 * @param handle the device handle
 * @param odr the functional output data rate to be set
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Set_ODR( DrvContextTypeDef *handle, SensorOdr_t odr )
{
  if ( handle->isEnabled == 1 )
  {
    if ( LIS2DW12_Set_ODR_When_Enabled( handle, odr ) == COMPONENT_ERROR )
    {
      return COMPONENT_ERROR;
    }
  }
  else
  {
    if ( LIS2DW12_Set_ODR_When_Disabled( handle, odr ) == COMPONENT_ERROR )
    {
      return COMPONENT_ERROR;
    }
  }

  return COMPONENT_OK;
}

/**
 * @brief Set the LIS2DW12 accelerometer sensor output data rate
 * @param handle the device handle
 * @param odr the output data rate value to be set
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Set_ODR_Value( DrvContextTypeDef *handle, float odr )
{
  if ( handle->isEnabled == 1 )
  {
    if( LIS2DW12_Set_ODR_Value_When_Enabled( handle, odr ) == COMPONENT_ERROR )
    {
      return COMPONENT_ERROR;
    }
  }
  else
  {
    if ( LIS2DW12_Set_ODR_Value_When_Disabled( handle, odr ) == COMPONENT_ERROR )
    {
      return COMPONENT_ERROR;
    }
  }

  return COMPONENT_OK;
}

/**
 * @brief Get the LIS2DW12 accelerometer sensor full scale
 * @param handle the device handle
 * @param fullScale pointer to where full scale write to
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Get_FS( DrvContextTypeDef *handle, float *fullScale )
{
  LIS2DW12_ACC_FS_t fs_low_level;

  if ( LIS2DW12_ACC_R_FullScaleSelection( ( void* )handle, &fs_low_level ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  switch( fs_low_level )
  {
    case LIS2DW12_ACC_FS_2g:
      *fullScale = 2.0f;
      break;
    case LIS2DW12_ACC_FS_4g:
      *fullScale = 4.0f;
      break;
    case LIS2DW12_ACC_FS_8g:
      *fullScale = 8.0f;
      break;
    case LIS2DW12_ACC_FS_16g:
      *fullScale = 16.0f;
      break;
    default:
      *fullScale = -1.0f;
      return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief Set the LIS2DW12 accelerometer sensor full scale
 * @param handle the device handle
 * @param fullScale the functional full scale to be set
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Set_FS( DrvContextTypeDef *handle, SensorFs_t fullScale )
{
  LIS2DW12_ACC_FS_t new_fs;

  switch( fullScale )
  {

  case FS_LOW:
    new_fs = LIS2DW12_ACC_FS_2g;
    break;

  case FS_MID_LOW:
  case FS_MID:
    new_fs = LIS2DW12_ACC_FS_4g;
    break;

  case FS_MID_HIGH:
    new_fs = LIS2DW12_ACC_FS_8g;
    break;

  case FS_HIGH:
    new_fs = LIS2DW12_ACC_FS_16g;
    break;

  default:
    return COMPONENT_ERROR;
  }

  if ( LIS2DW12_ACC_W_FullScaleSelection( ( void* )handle, new_fs ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief Set the LIS2DW12 accelerometer sensor full scale
 * @param handle the device handle
 * @param fullScale the full scale value to be set
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Set_FS_Value( DrvContextTypeDef *handle, float fullScale )
{
  LIS2DW12_ACC_FS_t new_fs;

  new_fs = ( fullScale <= 2.0f ) ?  LIS2DW12_ACC_FS_2g
         : ( fullScale <= 4.0f ) ?  LIS2DW12_ACC_FS_4g
         : ( fullScale <= 8.0f ) ?  LIS2DW12_ACC_FS_8g
         :                          LIS2DW12_ACC_FS_16g;

  if ( LIS2DW12_ACC_W_FullScaleSelection( ( void* )handle, new_fs ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief Read the data from register
 * @param handle the device handle
 * @param reg register address
 * @param data register data
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Read_Reg( DrvContextTypeDef *handle, uint8_t reg, uint8_t *data )
{

  if ( LIS2DW12_ACC_ReadReg( (void *)handle, reg, data, 1 ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief Write the data to register
 * @param handle the device handle
 * @param reg register address
 * @param data register data
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Write_Reg( DrvContextTypeDef *handle, uint8_t reg, uint8_t data )
{

  if ( LIS2DW12_ACC_WriteReg( (void *)handle, reg, &data, 1 ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief Get data ready status
 * @param handle the device handle
 * @param status the data ready status
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Get_DRDY_Status( DrvContextTypeDef *handle, uint8_t *status )
{
  LIS2DW12_ACC_DRDY_t status_raw = LIS2DW12_ACC_DRDY_NOT_READY;

  if ( LIS2DW12_ACC_R_DataStatus( (void *)handle, &status_raw ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  *status = (uint8_t)status_raw;

  return COMPONENT_OK;
}

/**
 * @}
 */

/** @addtogroup LIS2DW12_Private_Functions Private functions
 * @{
 */

/**
 * @brief Set the LIS2DW12 accelerometer sensor output data rate when enabled
 * @param handle the device handle
 * @param odr the functional output data rate to be set
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Set_ODR_When_Enabled( DrvContextTypeDef *handle, SensorOdr_t odr )
{
  LIS2DW12_ACC_ODR_t new_odr;

  switch( odr )
  {

  case ODR_LOW:
    new_odr = LIS2DW12_ACC_ODR_LP_1Hz6_HP_12Hz5;
    break;

  case ODR_MID_LOW:
    new_odr = LIS2DW12_ACC_ODR_LP_25Hz_HP_25Hz;
    break;

  case ODR_MID:
    new_odr = LIS2DW12_ACC_ODR_LP_50Hz_HP_50Hz;
    break;

  case ODR_MID_HIGH:
    new_odr = LIS2DW12_ACC_ODR_LP_100Hz_HP_100Hz;
    break;

  case ODR_HIGH:
    new_odr = LIS2DW12_ACC_ODR_LP_200Hz_HP_200Hz;
    break;

  default:
    return COMPONENT_ERROR;
  }

  if ( LIS2DW12_ACC_W_OutputDataRate( ( void * )handle, new_odr ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief Set the LIS2DW12 accelerometer sensor output data rate when disabled
 * @param handle the device handle
 * @param odr the functional output data rate to be set
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Set_ODR_When_Disabled( DrvContextTypeDef *handle, SensorOdr_t odr )
{
  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
  LIS2DW12_Data_t *pComponentData = ( LIS2DW12_Data_t * )pData->pComponentData;

  switch( odr )
  {

  case ODR_LOW:
    pComponentData->Previous_ODR =  12.5f;
    break;

  case ODR_MID_LOW:
    pComponentData->Previous_ODR =  25.0f;
    break;

  case ODR_MID:
    pComponentData->Previous_ODR =  50.0f;
    break;

  case ODR_MID_HIGH:
    pComponentData->Previous_ODR = 100.0f;
    break;

  case ODR_HIGH:
    pComponentData->Previous_ODR = 200.0f;
    break;

  default:
    return COMPONENT_ERROR;
  }

  return COMPONENT_OK;
}

/**
 * @brief Set the LIS2DW12 accelerometer sensor output data rate when enabled
 * @param handle the device handle
 * @param odr the output data rate value to be set
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Set_ODR_Value_When_Enabled( DrvContextTypeDef *handle, float odr )
{
  LIS2DW12_ACC_ODR_t new_odr;

  new_odr = ( odr <=   1.6f ) ? LIS2DW12_ACC_ODR_LP_1Hz6_HP_12Hz5
          : ( odr <=  12.5f ) ? LIS2DW12_ACC_ODR_LP_12Hz5_HP_12Hz5
          : ( odr <=  25.0f ) ? LIS2DW12_ACC_ODR_LP_25Hz_HP_25Hz
          : ( odr <=  50.0f ) ? LIS2DW12_ACC_ODR_LP_50Hz_HP_50Hz
          : ( odr <= 100.0f ) ? LIS2DW12_ACC_ODR_LP_100Hz_HP_100Hz
          : ( odr <= 200.0f ) ? LIS2DW12_ACC_ODR_LP_200Hz_HP_200Hz
          : ( odr <= 400.0f ) ? LIS2DW12_ACC_ODR_LP_200Hz_HP_400Hz
          : ( odr <= 800.0f ) ? LIS2DW12_ACC_ODR_LP_200Hz_HP_800Hz
          :                     LIS2DW12_ACC_ODR_LP_200Hz_HP_1600Hz;

  if ( LIS2DW12_ACC_W_OutputDataRate( ( void* )handle, new_odr ) == MEMS_ERROR )
  {
    return COMPONENT_ERROR;
  }

  if ( odr <= 1.6f )
  {
    /* Set low-power mode for 1.6 Hz ODR */
    if ( LIS2DW12_ACC_W_ModeSelection( ( void * )handle, LIS2DW12_ACC_MODE_LOW_POWER_STD ) == MEMS_ERROR )
    {
      return COMPONENT_ERROR;
    }
  }

  if ( odr > 200.0f )
  {
    /* Set high-performance mode for ODR higher then 200 Hz */
    if ( LIS2DW12_ACC_W_ModeSelection( ( void * )handle, LIS2DW12_ACC_MODE_HIGH_PERFORMANCE ) == MEMS_ERROR )
    {
      return COMPONENT_ERROR;
    }
  }

  return COMPONENT_OK;
}

/**
 * @brief Set the LIS2DW12 accelerometer sensor output data rate when disabled
 * @param handle the device handle
 * @param odr the output data rate value to be set
 * @retval COMPONENT_OK in case of success
 * @retval COMPONENT_ERROR in case of failure
 */
static DrvStatusTypeDef LIS2DW12_Set_ODR_Value_When_Disabled( DrvContextTypeDef *handle, float odr )
{
  ACCELERO_Data_t *pData = ( ACCELERO_Data_t * )handle->pData;
  LIS2DW12_Data_t *pComponentData = ( LIS2DW12_Data_t * )pData->pComponentData;

  pComponentData->Previous_ODR = ( odr <=   1.6f ) ?    1.6f
                               : ( odr <=  12.5f ) ?   12.5f
                               : ( odr <=  25.0f ) ?   25.0f
                               : ( odr <=  50.0f ) ?   50.0f
                               : ( odr <= 100.0f ) ?  100.0f
                               : ( odr <= 200.0f ) ?  200.0f
                               : ( odr <= 400.0f ) ?  400.0f
                               : ( odr <= 800.0f ) ?  800.0f
                               :                     1600.0f;

  return COMPONENT_OK;
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
