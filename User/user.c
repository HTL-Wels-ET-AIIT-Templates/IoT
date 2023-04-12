/**
   @file user.c
   @brief User specific code for IOT project
   @author Bernhard Breinbauer
   @author add your name here
 */

// Includes ------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "iotclient.h"
#include "user.h"
#include "sensor.h"

#define PAYLOADSIZE     512

// global Variables ------------------------------------------------------------------


// local Variables ------------------------------------------------------------------
static volatile uint8_t buttonFlags = 0;
static char  PayloadBuffer[PAYLOADSIZE];

// Function Declarations ------------------------------------------------------
uint32_t HAL_GetTick(void);
void Led_On(void);
void Led_Off(void);

// Functions ------------------------------------------------------------------

/**
 * Initialization Code for module user
 * @param None
 * @retval  0 in case of success
 *         -1 in case of failure
 */
int userInit(void) {


	return 0;
}

/**
 * Task function of module user. It is called every second.
 * If user button has been pressed, new data should be published
 * If user button has been pressed twice, data should be published periodically
 * @param None
 * @retval None
 */
void userTask(void){
	// TODO: What has to be done in our main loop?
	ButtonPush_t command;
	bool bSinglePublish = false;

	command = Button_WaitForPush(10);
	if (command == BP_SINGLE_PUSH) {      /* If short button push, publish once. */
		bSinglePublish = true;
	}

	if (bSinglePublish == true) {
		if (PrepareMqttPayload(PayloadBuffer, PAYLOADSIZE)) {
			printf("ERROR While formatting sensor json payload\n");
		} else {
			// printf for debug
			printf("publishing sensor data\n");
			printf("PayloadBuffer: %s\n", PayloadBuffer);
			publishData("ABCopenTopicXYZ/SensorData", PayloadBuffer);
		}
	}
}

/**
 * @brief ISR for User Button
 */
void Button_ISR(void)
{
	static uint32_t filterDelay = 0;
	if (HAL_GetTick() > filterDelay) {
		buttonFlags++;
		filterDelay = HAL_GetTick() + 20;
	}
}

/**
 * @brief Check for button to be pushed
 * @param delay Time in ms to wait for button event
 * @retval either BP_NOT_PUSHED, BP_SINGLE_PUSH or BP_MULTIPLE_PUSH
 */
ButtonPush_t Button_WaitForPush(uint32_t delay)
{
	uint32_t time_out = HAL_GetTick() + delay;
	do
	{
		if (buttonFlags >= 2) {
			buttonFlags = 0;
			return BP_MULTIPLE_PUSH;
		} else if (buttonFlags == 1) {
			buttonFlags = 0;
			return BP_SINGLE_PUSH;
		}
	}
	while( HAL_GetTick() < time_out);
	return BP_NOT_PUSHED;
}

/**
 * @brief  fill the payload with the sensor values
 * @param PayloadBuffer is the char pointer for the Payload buffer to be filled
 * @param PayloadSize size of the above buffer
 * @retval 0 in case of success
 *         -1 in case of failure
 */
int PrepareMqttPayload(char * PayloadBuffer, int PayloadSize)
{
	static int toggle = 0;
	int snprintfreturn = 0;

	toggle = toggle ^ 0x01;
	if (toggle)
		Led_On();
	else
		Led_Off();

	/* TODO: get sensor values, relevant functions are:
	 *   SensorGetHumidity(), SensorGetPressure(), SensorGetTemperature()
	 *   SensorGetAcceleration(), SensorGetGyro(), SensorGetMagneto()
	 */
	snprintfreturn = snprintf( PayloadBuffer, PayloadSize, "{"
			"\"toggle\": %d,"
			"}",
			toggle
	);
	/* Check total size to be less than buffer size
	 * if the return is >=0 and <n, then
	 * the entire string was successfully formatted; if the return is
	 * >=n, the string was truncated (but there is still a null char
	 * at the end of what was written); if the return is <0, there was
	 * an error.
	 */
	if (snprintfreturn >= 0 && snprintfreturn < PayloadSize)
	{
		return 0;
	}
	else if(snprintfreturn >= PayloadSize)
	{
		//      msg_error("Data Pack truncated\n");
		return 0;
	}
	else
	{
		//      msg_error("Data Pack Error\n");
		return -1;
	}


}

