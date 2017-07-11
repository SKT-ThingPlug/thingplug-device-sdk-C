/**
 * @file Configuration.h
 *
 * @brief Configuration V1.14 header for The Samples
 *
 * Copyright (C) 2017. SK Telecom, All Rights Reserved.
 * Written 2017, by SK Telecom
 */

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#define MAC_ADDRESS            { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 }          // MAC address for NIC

#define MQTT_HOST                           "mqtt.thingplug.net"
#define MQTT_SECURE_HOST                    "ssl://mqtt.thingplug.net"
#define MQTT_PORT                           1883
#define MQTT_SECURE_PORT                    8883
#define MQTT_KEEP_ALIVE                     120
#define MQTT_ENABLE_SERVER_CERT_AUTH        1

#define ONEM2M_AE_NAME                      "(Enter Device ID here)"
#define ONEM2M_SERVICE_ID                   "(Enter Service ID here)"
#define ONEM2M_TO                           "/~/%s/v1_0"
#define ONEM2M_MGA                          "mqtt://%s"
#define ONEM2M_POA                          "mqtt://oneM2M/req_msg/%s/%s"
#define ONEM2M_NU                           "mqtt://%s"

#define ACCOUNT_USER_ID                     "(Enter ThingPlug ID here)"
#define ACCOUNT_CREDENTIAL_ID               "(Enter ThingPlug Credential ID here)"

#define NAME_CONTAINER                      "TTV"
#define NAME_SUBSCRIPTION                   "SDK"

#define NAME_MGMTCMD                        "(Enter Device ID here)"
#define NAME_MGMTCMD_FIRMWARE               "(Enter Device ID here)_firmware"
#define CMT_MGMTCMD                         "1"
#define CMT_MGMTCMD_FIRMWARE                "902"

#endif // _CONFIGURATION_H_

