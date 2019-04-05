/**
  ******************************************************************************
  * @file    flash.h
  * @author  MCD Application Team
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   Management of the internal flash memory.
  *          Header for flash.c
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
#ifndef __flash_H
#define __flash_H

/** @addtogroup FFT_DEMO_NUCLEO_F429ZI
  * @{
  */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

int FLASH_unlock_erase(uint32_t address, uint32_t len_bytes);
int FLASH_update(uint32_t dst_addr, const void *data, uint32_t size);
uint32_t FLASH_get_bank(uint32_t addr);


#if defined (STM32L475xx) || defined (STM32L496xx) || defined (STM32L476xx)  // <hdd>
int FLASH_write_at(uint32_t address, uint64_t *pData, uint32_t len_bytes);
int FLASH_get_pageInBank(uint32_t addr);
int FLASH_set_boot_bank(uint32_t bank);
#else
int FLASH_write_at(uint32_t address, uint32_t *pData, uint32_t len_bytes);
uint32_t GetSectorMap(void);
#endif



#ifdef __cplusplus
}
#endif


/**
 * @}
 */

#endif /* __flash_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

