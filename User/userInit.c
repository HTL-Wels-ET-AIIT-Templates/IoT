/*
 * test_devicemgmt.c
 *
 *  Created on: 03-May-2016
 *      Author: HariPrasadReddy
 */
/* Unit Tests to test source code of devicemanagementclient.c */
#include <iotclient.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "msg.h"
#include "sensors_data.h"
#include "iot_flash_config.h"
#include "cloud.h"
#include "user.h"

/* define for manage device request */

extern volatile uint8_t button_flags;

/* Global variables ---------------------------------------------------------*/
extern const user_config_t lUserConfig;

/* Private variables ---------------------------------------------------------*/
static bool reboot_callback=false;

/* Private function prototypes -----------------------------------------------*/
void HAL_Delay(uint32_t Delay);
void HAL_NVIC_SystemReset(void);

#define BM_INDIV_DEV_STRING_MAX 64
typedef struct
{
	char   url[BM_INDIV_DEV_STRING_MAX];
	uint16_t port;
	char   device_id[BM_INDIV_DEV_STRING_MAX];
} device_info_t;

/**
 * @brief  Set the device info structure according to the connection string in the FLASH memory.
 * @param  Out: dev_info       pointer to the device info structure
 * @retval 0:    the connection string in Flash has been parsed successfully and the output structure fields have been set
 *        -1:    in case of failure
 */
int setDevInfo( device_info_t * dev_info )
{
	int ret = 0;
	// TODO: clean up & remove
	/* initialising the output structure */
	memset(dev_info, 0, sizeof(device_info_t));
	//  strcpy(dev_info->url,IBM_IOT_URL);
	strcpy(dev_info->url, MQTT_SERVER_URL);
	strcpy(dev_info->device_id, MQTT_CLIENT_ID);
	dev_info->port = MQTT_SERVER_PORT;

	return ret;
}

// this is the central function for publishing sensor data
void TestSensorPublish(device_info_t *dev)
{
	// Todo: clean up this function
	initialize(dev->url, dev->port, dev->device_id);
	if (connectiotf())
	{
		msg_error("Unable to connect \n");
	}

	printf("\nThe data is published to %s : %i\n", dev->url, dev->port);
#ifdef SENSOR
	printf("When user button is pushed shortly, application publishes the sensor values,");
	printf(" a 0/1 toggle value (the green Led switches accordingly) and a timestamp\n\r");
#else
	printf("When user button is pushed shortly, application publishes a 0/1 toggle value (the green Led switches accordingly)\n\r");
#endif

	printf("On double button push, application enters in a loop and publishes automatically every second. Next double push returns to previous mode\n\n\r");


	do
	{
		userTask();

		yield(100);

		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
	while (!reboot_callback);

	msg_info("outside the do-while loop.\n");

	if (disconnect())
	{
		msg_error("while disconnecting\n");
	}

	if(reboot_callback == true)
	{
		msg_info("Calling HAL_NVIC_SystemReset()\n");
		platform_deinit();
		HAL_NVIC_SystemReset();
	}

}

/**
 * @brief  start sending data to server
 * @param  none
 * @retval 0:    the connection string in Flash has been parsed succesfully and the output structure fields have been set
 *        -1:    in case of failure
 */
int cyclicDataPublish(void)
{
	device_info_t device;
	int ret = 0;

	ret = setDevInfo(&device);

	if (ret == 0)
	{
		TestSensorPublish(&device);
	}

	return ret;
}
