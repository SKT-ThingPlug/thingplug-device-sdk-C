
/**
 * @file oneM2MConfig.h
 *
 * @brief Configuration header for The oneM2M
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#ifndef _ONEM2MCONFIG_H_
#define _ONEM2MCONFIG_H_

#define DEBUG_ENABLE            // whether print debug log

#define IMPORT_TLS_LIB          // whether import TLS library

#define MAC_ADDRESS                         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }      // MAC address for NIC

#define MQTT_SUBSCRIBE_TOPIC_MAX_CNT        5               // subscribe topic max count
#define MQTT_TOPIC_MAX_LENGTH               128             // topic message max length

#define MQTT_UNIQUE_ID_PREFIX               "threadX_id_"

#define MQTT_MAX_PACKET_SIZE                1024
#define MQTT_RETRY_CNT                      3
#define MQTT_RETRY_INTERVAL                 100

#endif // _ONEM2MCONFIG_H_
