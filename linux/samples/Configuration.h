/**
 * @file Configuration.h
 *
 * @brief Configuration header for The Samples
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

//#define ONEM2M_V1_12

#ifdef ONEM2M_V1_12

#define MQTT_HOST                           "(TBD.)"
#define MQTT_PORT                           1883
#define MQTT_SECURE_PORT                    8883
#define MQTT_KEEP_ALIVE                     120
#define MQTT_ENABLE_SERVER_CERT_AUTH        0

#define ONEM2M_NODEID                       ""
#define ONEM2M_AE_RESOURCENAME              "ae-middleware"
#define ONEM2M_SERVICENAME                  "middleware"
#define ONEM2M_CB                           "(TBD.)"
#define ONEM2M_TO                           "(TBD.)"
#define ONEM2M_RI                           "12345"

#define ACCOUNT_USER                        "(TBD.)"
#define ACCOUNT_PASSWORD                    "(TBD.)"

#define APP_AEID                            "(TBD.)"

#define NAME_NODE                           "nod-middleware"
#define NAME_REMOTECSE                      "csr-middleware"
#define NAME_CONTAINER                      "cnt-sensor01"
#define NAME_MGMTCMD                        "mgc-reset"
#define NAME_LOCATIONPOLICY                 "lcp-middleware"
#define NAME_ACCESSCONTROLPOLICY            "acp-middleware"
#else // oneM2M V1

//#define MQTT_HOST                           "mqtt.sktiot.com"
#define MQTT_HOST                           "ssl://mqtt.sktiot.com" // TLS

#define MQTT_PORT                           1883
#define MQTT_SECURE_PORT                    8883
#define MQTT_KEEP_ALIVE                     300
#define MQTT_ENABLE_SERVER_CERT_AUTH        1

#define ACCOUNT_USER						"(TBD.)" //ThingPlug user id
#define ACCOUNT_PASSWORD					"(TBD.)" //ThingPlug user key (uKey)

#define ONEM2M_CSEBASE                      "ThingPlug"
#define ONEM2M_NODEID                       "(TBD.)"
#define ONEM2M_TO                           "/ThingPlug/v1_0"
#define ONEM2M_PASSCODE                     "(TBD.)"
#define ONEM2M_RI                           "12345"

#define NAME_CONTAINER                      "%s_container_01"
#define NAME_MGMTCMD                        "%s_%s"
#define NAME_AREANWKINFO                    "%s_areaNwkInfo_01"
#define NAME_LOCATIONPOLICY                 "%s_locationPolicy_01"
#define NAME_AE                             "%s_AE_01"
#endif // ONEM2M_V1_12

// common
#define CMT_DEVRESET						"DevReset"
#define CMT_REPPERCHANGE					"RepPerChange"
#define CMT_REPIMMEDIATE					"RepImmediate"
#define CMT_TAKEPHOTO						"TakePhoto"
#define CMT_LEDCONTROL						"LEDControl"

#define TOPIC_PUBLISH                       "/oneM2M/req/%s/%s"

#endif // _CONFIGURATION_H_

