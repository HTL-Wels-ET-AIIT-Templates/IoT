/**
 ******************************************************************************
 * @file    main.c
 * @author  Central LAB
 * @version	3.0.0
 * @date	20-July-2018
 * @brief  IoT platform service
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
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
#include <stdio.h>

#ifdef STM32F429xx
#include "cmsis_os.h"
#include "iotDevice_LWIP.h"
#endif /* STM32F429xx */


/** @addtogroup FFT_DEMO
 * @{
 */


/**
 * @mainpage FP-CLD-WATSON1  firmware package
 *
 * <b>SW Diagram</b>
 *
 * @image html FP-CLD-WATSON1_sw_diagram.png
 *
 * <b>Introduction</b> <br>
 * The FP-CLD-WATSON1 is based on the STM32CubeHAL hardware abstraction layer for the STM32 microcontroller. The
 * package extends STM32Cube by providing a board support package (BSP) for the NFC and sensor expansion
 * boards. The drivers abstract low-level details of the hardware and allow the middleware components and applications
 * to access sensor data in a hardware-independent manner, to read/write information from/to NFC/RFID tag for
 * configuring Cloud platform service.

 * <b>IOT Platform example applications:</b><br>
 * This application reads the sensor data values from the accelerometer, magnetometer and Gyroscope, which are transmitted to the IoT Cloud through Ethernet.
 * It also integrates Fast Fourier Transform (FFT) algorithm for vibration analysis for processing accelerometers data, which
 * can be used to detect vibration from devices such as motors, fans and pumps.
 * The URL of the web page where sensors and FFT data can be visualized is also written in NFC tag (ST25DV04K).
 * The application can be configured in Quickstart as well as Simple-Registration mode to control the
 * device from IOT Platform service based application.(the latter mode requires an account on IBM Watson).
 *
 * <br><b>Hardware Platform</b>
 *
 * @image html QuickStartGuide_V-ODE-BLUEMIX1_v0.2.png

 * <b>Toolchain Support</b><br>
 * The library has been developed and tested on following toolchains:
 *        - IAR Embedded Workbench for ARM (EWARM) toolchain V8.20.1 + ST-Link
 *        - Keil Microcontroller Development Kit (MDK-ARM) toolchain V5.16a + ST-LINK
 *        - System Workbench for STM32 (SW4STM32) V2.4.0 + ST-LINK
 */


/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{
	/* Working application */
	osThreadDef(IoT_Thread, iotThread, osPriorityNormal, 0, 2*4096 );

	osThreadCreate (osThread(IoT_Thread), NULL);

	/* Start scheduler */
	osKernelStart();

	/* We should nevner get here as control is now taken by the scheduler */
	for( ;; );
}

#ifdef configCHECK_FOR_STACK_OVERFLOW
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	printf("Stack overflow\r\n");
}
#endif /* configCHECK_FOR_STACK_OVERFLOW */

#ifdef configUSE_MALLOC_FAILED_HOOK
void vApplicationMallocFailedHook( void )
{
	printf("Heap  Alloc failed\r\n");
}
#endif /* configUSE_MALLOC_FAILED_HOOK */

/**
 * @}
 */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
