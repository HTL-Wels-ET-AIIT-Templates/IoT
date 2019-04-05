/**
  ******************************************************************************
  * @file    nucleo_com.h
  * @author  SRA
 * @version	3.0.0
 * @date	20-July-2018
  * @brief   Nucleo COM Configuration.
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
#ifndef __NUCLEO_COM_H__
#define __NUCLEO_COM_H__

/** @addtogroup FFT_DEMO_NUCLEO_F429ZI
  * @{
  */
   



typedef enum 
{
  COM1 = 0,
}COM_TypeDef;  

/**
 * @brief Definition for COM port1, connected to USART6
 */ 
#define NUCLEO_COM1                          USART2
#define NUCLEO_COM1_CLK_ENABLE()             __HAL_RCC_USART2_CLK_ENABLE()
#define NUCLEO_COM1_CLK_DISABLE()            __HAL_RCC_USART2_CLK_DISABLE()

#define NUCLEO_COM1_TX_PIN                   GPIO_PIN_2
#define NUCLEO_COM1_TX_GPIO_PORT             GPIOA
#define NUCLEO_COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()   
#define NUCLEO_COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()  
#define NUCLEO_COM1_TX_AF                    GPIO_AF7_USART2

#define NUCLEO_COM1_RX_PIN                   GPIO_PIN_3
#define NUCLEO_COM1_RX_GPIO_PORT             GPIOA
#define NUCLEO_COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()   
#define NUCLEO_COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()  
#define NUCLEO_COM1_RX_AF                    GPIO_AF7_USART2

#define NUCLEO_COM1_IRQn                     USART2_IRQn

#define NUCLEO_COMx_CLK_ENABLE(__INDEX__)            do { if((__INDEX__) == COM1) {NUCLEO_COM1_CLK_ENABLE();} } while(0)
#define NUCLEO_COMx_CLK_DISABLE(__INDEX__)           (((__INDEX__) == 0) ? NUCLEO_COM1_CLK_DISABLE() : 0)

#define NUCLEO_COMx_TX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == COM1) {NUCLEO_COM1_TX_GPIO_CLK_ENABLE();} } while(0)
#define NUCLEO_COMx_TX_GPIO_CLK_DISABLE(__INDEX__)   (((__INDEX__) == 0) ? NUCLEO_COM1_TX_GPIO_CLK_DISABLE() : 0)

#define NUCLEO_COMx_RX_GPIO_CLK_ENABLE(__INDEX__)    do { if((__INDEX__) == COM1) {NUCLEO_COM1_RX_GPIO_CLK_ENABLE();} } while(0)
#define NUCLEO_COMx_RX_GPIO_CLK_DISABLE(__INDEX__)   (((__INDEX__) == 0) ? NUCLEO_COM1_RX_GPIO_CLK_DISABLE() : 0)

/**
 * @}
 */
	
#endif // __NUCLEO_COM_H__
