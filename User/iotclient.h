/*******************************************************************************
 * Copyright (c) 2015 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Jeffrey Dare            - initial implementation and API implementation
 *    Sathiskumar Palaniappan - Added support to create multiple Iotfclient
 *                              instances within a single process
 *    Lokesh Haralakatta      - Added SSL/TLS support
 *    Lokesh Haralakatta      - Added Client Side Certificates support
 *    Lokesh Haralakatta      - Separated out device client and gateway client specific code.
 *                            - Retained back common code here.
 *******************************************************************************/

#ifndef IOTCLIENT_H_
#define IOTCLIENT_H_

#include "MQTTClient.h"
#include "iotf_network_tls_wrapper.h"

#define BUFFER_SIZE 1024
#define HOSTNAME_STRING_SIZE    64
#define CLIENTID_STRING_SIZE    128
#define CONFIG_STRING_SIZE		64

enum errorCodes { CONFIG_FILE_ERROR = -3, MISSING_INPUT_PARAM = -4, QUICKSTART_NOT_SUPPORTED = -5 };

extern unsigned short keepAliveInterval;
extern char *sourceFile;

//configuration file structure
struct iotf_config
{
       char domain[CONFIG_STRING_SIZE];
       char id[CONFIG_STRING_SIZE];
       int port;
};

typedef struct iotf_config Config;

// TODO: move this structure directly into IoTDevice
//iotfclient
typedef struct
{
       Network n;
       Client c;
       Config cfg;
       unsigned char buf[BUFFER_SIZE];
       unsigned char readbuf[BUFFER_SIZE];
} iotfclient;

//structure for device information
//string variables needs to be allocated with enough memory based on the requirement
struct DeviceInfo{
	char serialNumber[20];
	char manufacturer[20];
	char model[20];
	char deviceClass[20];
	char description[30];
	char fwVersion[10];
	char hwVersion[10];
	char descriptiveLocation[20];
};

struct IotDevice{
	struct DeviceInfo deviceInfo;
	iotfclient deviceClient;
};
typedef struct IotDevice IotDevice_t;
extern  IotDevice_t iotDevice;

/**
* Function used to initialize IoT client
* @param domain - Your domain Name
* @param port - TCP port
* @param devId - device ID
*
* @return int return code
*/
int initialize(char* domainName, uint16_t port, char *devId);

/**
* Function used to initialize the IoT client
*
* @return int return code
*/
int connectiotf(void);

/**
* Function used to publish the given data to the topic with the given QoS
* @Param topic - Topic to publish
* @Param payload - Message payload
*
* @return int - Return code from MQTT Publish Call
**/
int publishData(char *topic, char *payload);

/**
* Function used to check if the client is connected
*
* @return int return code
*/
int isConnected(void);

/**
* Function used to Yield for commands.
* @param time_ms - Time in milliseconds
* @return int return code
*/
int yield(int time_ms);

/**
* Function used to disconnect from the IoT service
*
* @return int return code
*/
int disconnect(void);

/**
* Function used to set the time to keep the connection alive with IoT service
* @param keepAlive - time in secs
*
*/
void setKeepAliveInterval(unsigned int keepAlive);

int retry_connection(void);

int get_config(char * filename, Config * configstr);

#endif /* IOTCLIENT_H_ */
