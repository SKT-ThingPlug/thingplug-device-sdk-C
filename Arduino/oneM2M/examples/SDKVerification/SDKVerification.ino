/**
 * @file SDKVerification.ino
 *
 * @brief Simple SDK verification application
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */

#include <Arduino.h>

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <TimeLib.h>
#include <LoRaHandle.h>
#include <DMSensor.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "oneM2M.h"
#include "Configuration.h"
#ifdef ONEM2M_V1_12
#include "include/oneM2M_V1_12.h"
#else
#include "include/oneM2M_V1.h"
#endif



int _led = 4;
int _button = 6;
int _light = A2;
int _temp = A1;
int _dist = 8;

LoRaHandle lora;
DMSensor sensor(_led,_button,_light,_temp,_dist);
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 102); // time-b.timefreq.bldrdoc.gov
// IPAddress timeServer(132, 163, 4, 103); // time-c.timefreq.bldrdoc.gov

const int timeZone = 0;

EthernetUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

#ifdef ONEM2M_V1_12
#define TOPIC_SUBSCRIBE_REQ                 "/oneM2M/req/%s/%s" // FIXME
#define TOPIC_SUBSCRIBE_RES                 "/oneM2M/resp/%s/%s"
#define TOPIC_SUBSCRIBE_SIZE                2
// #define TOPIC_PUBLISH                       "/oneM2M/req/%s/%s"

#define TO_AE                               "%s/%s"
#define TO_CONTAINER                        "%s/%s/%s"
#define TO_CONTENTINSTANCE					"%s/%s/%s/cin-%s"
#define TO_MGMTCMDRESULT                    "%s/mgc-%s/exin-%s"
#else
#define TO_REMOTECSE                        "%s/remoteCSE-%s"
#define TO_NODE                             "%s/node-%s"
#define TO_CONTAINER                        "%s/remoteCSE-%s/container-%s"
#define TO_CONTENTINSTANCE                  "%s/remoteCSE-%s/container-%s/contentInstance-%s"
#define TO_AREANWKINFO                      "%s/node-%s/areaNwkInfo-%s"
#define TO_AE                               "%s/remoteCSE-%s/AE-%s"
#define TO_LOCATIONPOLICY                   "%s/locationPolicy-%s"
#define TO_MGMTCMD                          "%s/mgmtCmd-%s"
#define TO_MGMTCMDRESULT                    "%s/mgmtCmd-%s/execInstance-%s"

#define TOPIC_SUBSCRIBE_REQ                 "/oneM2M/req/+/%s"
#define TOPIC_SUBSCRIBE_RES                 "/oneM2M/resp/%s/+"
#define TOPIC_SUBSCRIBE_SIZE                2

#define NAME_MGA                            "MQTT|%s"
#endif

enum VERIFICATION_STEP
{
    VERIFICATION_CSEBASE_RETRIEVE = 0,
	VERIFICATION_NODE_CREATE,
	VERIFICATION_REMOTECSE_CREATE,
	VERIFICATION_CONTAINER_CREATE,
	VERIFICATION_MGMTCMD_CREATE,
	VERIFICATION_CONTENTINSTANCE_CREATE,
	VERIFICATION_LOCATIONPOLICY_CREATE,
	VERIFICATION_AE_CREATE,
	VERIFICATION_AREANWKINFO_CREATE,

	VERIFICATION_AREANWKINFO_UPDATE = 9,
    VERIFICATION_AE_UPDATE,
	VERIFICATION_LOCATIONPOLICY_UPDATE,
	VERIFICATION_MGMTCMD_UPDATE,
	VERIFICATION_CONTAINER_UPDATE,
	VERIFICATION_REMOTECSE_UPDATE,
	VERIFICATION_NODE_UPDATE,
	
    VERIFICATION_AREANWKINFO_DELETE = 16,
    VERIFICATION_AE_DELETE,
    VERIFICATION_LOCATIONPOLICY_DELETE,
    VERIFICATION_CONTENTINSTANCE_DELETE,
    VERIFICATION_MGMTCMD_DELETE,
    VERIFICATION_CONTAINER_DELETE,
    VERIFICATION_REMOTECSE_DELETE,
    VERIFICATION_NODE_DELETE,

	VERIFICATION_ACCESSCONTROLPOLICY_CREATE,
	VERIFICATION_ACCESSCONTROLPOLICY_UPDATE,
	VERIFICATION_ACCESSCONTROLPOLICY_DELETE,
	
    VERIFICATION_END

};

static enum VERIFICATION_STEP mStep = VERIFICATION_CSEBASE_RETRIEVE;
#ifdef ONEM2M_V1_12
static char mAEID[128] = "";
static char mNodeLink[23] = "";
#else
static char mDeviceKey[128] = "";

static char mNodeLink[23] = "";
static char mHostCSELink[23] = "";
static char mRemoteCSEResourceName[128] = "";
static char mContentInstanceResourceName[128] = "";
static char mAEResourceName[128] = "";
static char mLocationPolicyResourceName[128] = "";
static char mMgmtCmdResourceName[128] = "";
static char mAreaNwkInfoResourceName[128] = "";
static char mContainerResourceName[128] = "";
#endif
static char mClientID[24] = "";

void RequestOneM2M(int resourceType, int operation, char* fr, char* to, char* ri, void* pc) 
{
    if(mStep < VERIFICATION_END) {
#ifdef ONEM2M_V1_12
        tp_oneM2M_V1_12_Request(resourceType, operation, fr, to, ri, pc);
#else
        if(pc != NULL) {
            tp_oneM2M_V1_Request(resourceType, operation, to, ri, pc);
        } 
#endif
    }
}

/**
 * @brief do verification step
 */
void DoVerificationStep() {
    int resourceType;
    int operation;
#ifdef ONEM2M_V1_12
	char poa[128] = "";
#endif
    char to[512] = "";
    char buffer[128] = "";
    char* ri = (char*)ONEM2M_RI;
    char* fr = NULL;
    void* pc = NULL;

    Serial.print(">>>>>>>>>>>>>>>>> step = ");
    Serial.print(mStep);
    Serial.println(" <<<<<<<<<<<<<<<<<<<<<<");
    
    switch(mStep) {
#ifdef ONEM2M_V1_12
    // RETRIEVE
    case VERIFICATION_CSEBASE_RETRIEVE:
        resourceType = CSEBase;
        operation = RETRIEVE;
        fr = (char*)APP_AEID;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
  	// CREATE
    case VERIFICATION_NODE_CREATE:
        resourceType = (int)node;
        operation = CREATE;
        fr = (char*)APP_AEID;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_v1_12_node node_create;
        memset(&node_create, 0, sizeof(node_create));
        node_create.rn = (char*)NAME_NODE;
        node_create.ni = (char*)"node_01";
        pc = (void *)&node_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;		
    case VERIFICATION_REMOTECSE_CREATE:
        resourceType = (int)remoteCSE;
        operation = CREATE;
	    fr = (char*)""; // registered CSE-ID
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_v1_12_remoteCSE remoteCSE_create;
        remoteCSE_create.rn = (char*)NAME_REMOTECSE;
        remoteCSE_create.cb = (char*)ONEM2M_CB;
        remoteCSE_create.cst = (char*)"2";
        remoteCSE_create.csi = (char*)"/"; // registered CSE-ID with slash
        remoteCSE_create.rr = (char*)"true";
        pc = (void *)&remoteCSE_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_AE_CREATE:
        oneM2M_v1_12_AE AE_create;
        resourceType = (int)AE;
        operation = CREATE;
        fr = (char*)"S";
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        memset(&AE_create, 0, sizeof(AE_create));
        AE_create.rn = (char*)ONEM2M_AE_RESOURCENAME;
        AE_create.rr = (char*)"true";
        snprintf(poa, sizeof(poa), "mqtt:///oneM2M/req/%s/%s", ONEM2M_SERVICENAME, ONEM2M_AE_RESOURCENAME);
        AE_create.poa = poa;
        AE_create.api = (char*)"middleware"; // pre-registered device name
        pc = (void *)&AE_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_CONTAINER_CREATE:
        oneM2M_v1_12_container container_create;
        resourceType = (int)container;
        operation = CREATE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, ONEM2M_AE_RESOURCENAME);
        memset(&container_create, 0, sizeof(container_create));
        container_create.rn = (char*)NAME_CONTAINER;
        pc = (void *)&container_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_CONTENTINSTANCE_CREATE:
        resourceType = (int)contentInstance;
        operation = CREATE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_CONTAINER);
        oneM2M_v1_12_contentInstance contentInstance_create;
        memset(&contentInstance_create, 0, sizeof(contentInstance_create));
        contentInstance_create.cnf = (char*)"text";
        contentInstance_create.con = (char*)getLoRaInstance();
        pc = (void *)&contentInstance_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_MGMTCMD_CREATE:
        resourceType = (int)mgmtCmd;
        operation = CREATE;
        fr = (char*)APP_AEID;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_v1_12_mgmtCmd mgmtCmd_create;
        memset(&mgmtCmd_create, 0, sizeof(mgmtCmd_create));
        mgmtCmd_create.rn = (char*)NAME_MGMTCMD;
        mgmtCmd_create.cmt = (char*)"1";
        mgmtCmd_create.ext = mNodeLink;
        pc = (void *)&mgmtCmd_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_LOCATIONPOLICY_CREATE:
        resourceType = (int)locationPolicy;
        operation = CREATE;
        fr = mAEID;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_v1_12_locationPolicy locationPolicy_create;
		locationPolicy_create.rn = (char*)NAME_LOCATIONPOLICY;
        locationPolicy_create.lon = (char*)"cnt-location";
        locationPolicy_create.lit = (char*)"1";
        locationPolicy_create.los = (char*)"2";
        pc = (void *)&locationPolicy_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_ACCESSCONTROLPOLICY_CREATE:
        resourceType = (int)accessControlPolicy;
        operation = CREATE;
		fr = mAEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, ONEM2M_AE_RESOURCENAME);
        oneM2M_v1_12_accessControlPolicy accessControlPolicy_create;
        accessControlPolicy_create.rn = (char*)NAME_ACCESSCONTROLPOLICY;        
        snprintf(buffer, sizeof(buffer), "<acr><acor>%s</acor><acop>63</acop></acr>", mAEID);
        accessControlPolicy_create.pv = buffer;
        //memset(buffer, 0, sizeof(buffer));
        accessControlPolicy_create.pvs = buffer;
        pc = (void *)&accessControlPolicy_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    // UPDATE
    case VERIFICATION_NODE_UPDATE:
        resourceType = (int)node;
        operation = UPDATE;
        fr = (char*)APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_NODE);
        oneM2M_v1_12_node node_update;
        pc = (void *)&node_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;    
  case VERIFICATION_REMOTECSE_UPDATE:
        resourceType = (int)remoteCSE;
        operation = UPDATE;
        fr = (char*)""; // registered CSE-ID
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_REMOTECSE);
        oneM2M_v1_12_remoteCSE remoteCSE_update;
        remoteCSE_update.acpi = (char*)"";
        remoteCSE_update.nl = (char*)"";
        pc = (void *)&remoteCSE_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_AE_UPDATE:
        resourceType = (int)AE;
        operation = UPDATE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, ONEM2M_AE_RESOURCENAME);
        oneM2M_v1_12_AE AE_update;
        AE_update.poa = (char*)"http://";
        pc = (void *)&AE_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_CONTAINER_UPDATE:
        resourceType = (int)container;
        operation = UPDATE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_CONTAINER);
        oneM2M_v1_12_container container_update;
        container_update.lbl = (char*)"lbl";
        pc = (void *)&container_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_MGMTCMD_UPDATE:
        resourceType = (int)mgmtCmd;
        operation = UPDATE;
        fr = (char*)APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_MGMTCMD);
        oneM2M_v1_12_mgmtCmd mgmtCmd_update;
        mgmtCmd_update.dc = (char*)"test";
        pc = (void *)&mgmtCmd_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_LOCATIONPOLICY_UPDATE:
        resourceType = (int)locationPolicy;
        operation = UPDATE;
        fr = (char*)APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_LOCATIONPOLICY);
        oneM2M_v1_12_locationPolicy locationPolicy_update;
        locationPolicy_update.los = (char*)"1"; // locationSource : Network_based
        pc = (void *)&locationPolicy_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_ACCESSCONTROLPOLICY_UPDATE:
        resourceType = (int)accessControlPolicy;
        operation = UPDATE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_ACCESSCONTROLPOLICY);
        oneM2M_v1_12_accessControlPolicy accessControlPolicy_update;
        snprintf(buffer, sizeof(buffer), "<acr><acor>%s</acor><acop>63</acop></acr>", mAEID);
        accessControlPolicy_update.pv = buffer;
        //memset(buffer, 0, sizeof(buffer));
        accessControlPolicy_update.pvs = buffer;
        pc = (void *)&accessControlPolicy_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    // DELETE
    case VERIFICATION_NODE_DELETE:
        resourceType = (int)node;
        operation = DELETE;
        fr = (char*)APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_NODE);
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;        
    case VERIFICATION_REMOTECSE_DELETE:
        resourceType = (int)remoteCSE;
        operation = DELETE;
        fr = (char*)""; // registered CSE-ID
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_REMOTECSE);
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_AE_DELETE:
        resourceType = (int)AE;
        operation = DELETE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, ONEM2M_AE_RESOURCENAME);
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_CONTAINER_DELETE:
        resourceType = (int)container;
        operation = DELETE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_CONTAINER);
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_MGMTCMD_DELETE:
        resourceType = (int)mgmtCmd;
        operation = DELETE;
        fr = (char*)APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_MGMTCMD);
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_LOCATIONPOLICY_DELETE:
        resourceType = (int)locationPolicy;
        operation = DELETE;
        fr = (char*)APP_AEID;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, NAME_LOCATIONPOLICY);
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_ACCESSCONTROLPOLICY_DELETE:
        resourceType = accessControlPolicy;
        operation = DELETE;
        fr = mAEID;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, ONEM2M_AE_RESOURCENAME, NAME_ACCESSCONTROLPOLICY);
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
#else	
    // RETRIEVE
    case VERIFICATION_CSEBASE_RETRIEVE:
        resourceType = CSEBase;
        operation = RETRIEVE;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_CSEBase CSEBase;
        memset(&CSEBase, 0, sizeof(CSEBase));
        CSEBase.ni = (char*)ONEM2M_NODEID;
        pc = (void *)&CSEBase;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;

    // CREATE
    case VERIFICATION_NODE_CREATE:
        resourceType = (int)node;
        operation = CREATE;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_node node_create;
        memset(&node_create, 0, sizeof(node_create));
        node_create.ni = (char*)ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_MGA, mClientID);
        node_create.mga = buffer;
        pc = (void *)&node_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_REMOTECSE_CREATE:
        resourceType = (int)remoteCSE;
        operation = CREATE;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_remoteCSE remoteCSE_create;
        memset(&remoteCSE_create, 0, sizeof(remoteCSE_create));
        remoteCSE_create.cst = (char*)"3";
        remoteCSE_create.ni = (char*)ONEM2M_NODEID;
        remoteCSE_create.nm = (char*)ONEM2M_NODEID;
        remoteCSE_create.passCode = (char*)ONEM2M_PASSCODE;
        // snprintf(buffer, sizeof(buffer), NAME_POA, ONEM2M_NODEID);
        // remoteCSE_create.poa = buffer;
        remoteCSE_create.rr = (char*)"true";
        remoteCSE_create.nl = mNodeLink;
        pc = (void *)&remoteCSE_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_CONTAINER_CREATE:
        resourceType = (int)container;
        operation = CREATE;
        snprintf(to, sizeof(to), TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);
        oneM2M_container container_create;
        memset(&container_create, 0, sizeof(container_create));
        container_create.ni = (char*)ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_CONTAINER, ONEM2M_NODEID);
        container_create.nm = buffer;
        container_create.dKey = mDeviceKey;
        container_create.lbl = (char*)"con";
        pc = (void *)&container_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_MGMTCMD_CREATE:
        resourceType = (int)mgmtCmd;
        operation = CREATE;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_mgmtCmd mgmtCmd_create;
        memset(&mgmtCmd_create, 0, sizeof(mgmtCmd_create));
        mgmtCmd_create.ni = (char*)ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_MGMTCMD, ONEM2M_NODEID);
        mgmtCmd_create.nm = buffer;
        mgmtCmd_create.dKey = mDeviceKey;
        mgmtCmd_create.cmt = (char*)"sensor_1";
        mgmtCmd_create.exe = (char*)"false";
        mgmtCmd_create.ext = mNodeLink;
        mgmtCmd_create.lbl = (char*)ONEM2M_NODEID;
        pc = (void *)&mgmtCmd_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_CONTENTINSTANCE_CREATE:
        resourceType = (int)contentInstance;
        operation = CREATE;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName);
        oneM2M_contentInstance contentInstance_create;
        memset(&contentInstance_create, 0, sizeof(contentInstance_create));
        contentInstance_create.ni = (char*)ONEM2M_NODEID;
        contentInstance_create.dKey = mDeviceKey;
        contentInstance_create.cnf = (char*)"text";
        contentInstance_create.con = (char*)"45";
        pc = (void *)&contentInstance_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_LOCATIONPOLICY_CREATE:
        resourceType = (int)locationPolicy;
        operation = CREATE;
        memcpy(to, ONEM2M_TO, strlen(ONEM2M_TO));
        oneM2M_locationPolicy locationPolicy_create;
        memset(&locationPolicy_create, 0, sizeof(locationPolicy_create));
        locationPolicy_create.ni = (char*)ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_LOCATIONPOLICY, ONEM2M_NODEID);
        locationPolicy_create.nm = buffer;
        locationPolicy_create.dKey = mDeviceKey;
        locationPolicy_create.los = (char*)"2";
        pc = (void *)&locationPolicy_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_AE_CREATE:
        resourceType = (int)AE;
        operation = CREATE;
        snprintf(to, sizeof(to), TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);
        oneM2M_AE AE_create;
        memset(&AE_create, 0, sizeof(AE_create));
        AE_create.ni = (char*)ONEM2M_NODEID;
        AE_create.dKey = mDeviceKey;
        AE_create.api= (char*)"1.2.481.1.0001.002.1234";
        snprintf(buffer, sizeof(buffer), NAME_AE, ONEM2M_NODEID);
        AE_create.apn= buffer;
        pc = (void *)&AE_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_AREANWKINFO_CREATE:
        resourceType = mgmtObj;
        operation = CREATE;
        snprintf(to, sizeof(to), TO_NODE, ONEM2M_TO, ONEM2M_NODEID);
        oneM2M_areaNwkInfo areaNwkInfo_create;
        memset(&areaNwkInfo_create, 0, sizeof(areaNwkInfo_create));
        areaNwkInfo_create.ni = (char*)ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_AREANWKINFO, ONEM2M_NODEID);
        areaNwkInfo_create.nm = buffer;
        areaNwkInfo_create.dKey = mDeviceKey;
        areaNwkInfo_create.mgd = (char*)"1004";
        areaNwkInfo_create.ant = (char*)"type";
        areaNwkInfo_create.ldv = (char*)"";
        pc = (void *)&areaNwkInfo_create;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    // UPDATE
    case VERIFICATION_AREANWKINFO_UPDATE:
        resourceType = mgmtObj;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_AREANWKINFO, ONEM2M_TO, ONEM2M_NODEID, mAreaNwkInfoResourceName);
        oneM2M_areaNwkInfo areaNwkInfo_update;
        memset(&areaNwkInfo_update, 0, sizeof(areaNwkInfo_update));
        areaNwkInfo_update.ni = (char*)ONEM2M_NODEID;
        snprintf(buffer, sizeof(buffer), NAME_AREANWKINFO, ONEM2M_NODEID);
        areaNwkInfo_update.nm = buffer;
        areaNwkInfo_update.dKey = mDeviceKey;
        areaNwkInfo_update.ant = (char*)"type2";
        areaNwkInfo_update.ldv = (char*)"1";
        pc = (void *)&areaNwkInfo_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_AE_UPDATE:
        resourceType = (int)AE;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, mRemoteCSEResourceName, mAEResourceName);
        oneM2M_AE AE_update;
        memset(&AE_update, 0, sizeof(AE_update));
        AE_update.ni = (char*)ONEM2M_NODEID;
        AE_update.dKey = mDeviceKey;
        snprintf(buffer, sizeof(buffer), "%s_AE_02", ONEM2M_NODEID);
        AE_update.apn= buffer;
        pc = (void *)&AE_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_LOCATIONPOLICY_UPDATE:
        resourceType = (int)locationPolicy;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_LOCATIONPOLICY, ONEM2M_TO, mLocationPolicyResourceName);
        oneM2M_locationPolicy locationPolicy_update;
        memset(&locationPolicy_update, 0, sizeof(locationPolicy_update));
        locationPolicy_update.ni = (char*)ONEM2M_NODEID;
        locationPolicy_update.dKey = mDeviceKey;
        locationPolicy_update.los = (char*)"3";
        locationPolicy_update.lbl = (char*)ONEM2M_NODEID; 
        pc = (void *)&locationPolicy_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_MGMTCMD_UPDATE:
        resourceType = (int)mgmtCmd;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_MGMTCMD, ONEM2M_TO, mMgmtCmdResourceName);
        oneM2M_mgmtCmd mgmtCmd_update;
        memset(&mgmtCmd_update, 0, sizeof(mgmtCmd_update));
        mgmtCmd_update.ni = (char*)ONEM2M_NODEID;
//        mgmtCmd_update.dKey = mDeviceKey;
//        mgmtCmd_update.uKey = UKEY;
        mgmtCmd_update.exe = (char*)"true";
//        mgmtCmd_update.lbl = (char*)ONEM2M_NODEID;
        mgmtCmd_update.ext = mNodeLink;//mNodeLink;
        pc = (void *)&mgmtCmd_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_CONTAINER_UPDATE:
        resourceType = (int)container;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName);
        oneM2M_container container_update;
        memset(&container_update, 0, sizeof(container_update));
        container_update.ni = (char*)ONEM2M_NODEID;
        container_update.dKey = mDeviceKey;
        container_update.lbl = (char*)"event";
        pc = (void *)&container_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_REMOTECSE_UPDATE:
        resourceType = (int)remoteCSE;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);
        oneM2M_remoteCSE remoteCSE_update;
        memset(&remoteCSE_update, 0, sizeof(remoteCSE_update));
        remoteCSE_update.ni = (char*)ONEM2M_NODEID;
        remoteCSE_update.dKey = mDeviceKey;
        pc = (void *)&remoteCSE_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_NODE_UPDATE:
        resourceType = (int)node;
        operation = UPDATE;
        snprintf(to, sizeof(to), TO_NODE, ONEM2M_TO, ONEM2M_NODEID);
        oneM2M_node node_update;
        memset(&node_update, 0, sizeof(node_update));
        node_update.ni = (char*)ONEM2M_NODEID;
        node_update.dKey = mDeviceKey;
        node_update.hcl = mHostCSELink;
        pc = (void *)&node_update;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    // DELETE
    case VERIFICATION_AREANWKINFO_DELETE:
        resourceType = mgmtObj;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_AREANWKINFO, ONEM2M_TO, ONEM2M_NODEID, mAreaNwkInfoResourceName);
        oneM2M_areaNwkInfo areaNwkInfo_delete;
        memset(&areaNwkInfo_delete, 0, sizeof(areaNwkInfo_delete));
        areaNwkInfo_delete.ni = (char*)ONEM2M_NODEID;
        areaNwkInfo_delete.dKey = mDeviceKey;
        pc = (void *)&areaNwkInfo_delete;            
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_AE_DELETE:
        resourceType = (int)AE;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_AE, ONEM2M_TO, mRemoteCSEResourceName, mAEResourceName);
        oneM2M_AE AE_delete;
        memset(&AE_delete, 0, sizeof(AE_delete));
        AE_delete.ni = (char*)ONEM2M_NODEID;
        AE_delete.dKey = mDeviceKey;
        pc = (void *)&AE_delete;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_LOCATIONPOLICY_DELETE:
        resourceType = (int)locationPolicy;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_LOCATIONPOLICY, ONEM2M_TO, mLocationPolicyResourceName);
        oneM2M_locationPolicy locationPolicy_delete;
        memset(&locationPolicy_delete, 0, sizeof(locationPolicy_delete));
        locationPolicy_delete.ni = (char*)ONEM2M_NODEID;
        locationPolicy_delete.dKey = mDeviceKey;
        pc = (void *)&locationPolicy_delete;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_CONTENTINSTANCE_DELETE:
        resourceType = (int)contentInstance;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_CONTENTINSTANCE, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName, mContentInstanceResourceName);
        oneM2M_contentInstance contentInstance_delete;
        memset(&contentInstance_delete, 0, sizeof(contentInstance_delete));
        contentInstance_delete.ni = (char*)ONEM2M_NODEID;
        contentInstance_delete.dKey = mDeviceKey;
        pc = (void *)&contentInstance_delete;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_MGMTCMD_DELETE:
        resourceType = (int)mgmtCmd;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_MGMTCMD, ONEM2M_TO, mMgmtCmdResourceName);
        oneM2M_mgmtCmd mgmtCmd_delete;
        memset(&mgmtCmd_delete, 0, sizeof(mgmtCmd_delete));
        mgmtCmd_delete.ni = (char*)ONEM2M_NODEID;
        mgmtCmd_delete.dKey = mDeviceKey;
        pc = (void *)&mgmtCmd_delete;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_CONTAINER_DELETE:
        resourceType = (int)container;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_CONTAINER, ONEM2M_TO, mRemoteCSEResourceName, mContainerResourceName);
        oneM2M_container container_delete;
        memset(&container_delete, 0, sizeof(container_delete));
        container_delete.ni = (char*)ONEM2M_NODEID;
        container_delete.dKey = mDeviceKey;
        pc = (void *)&container_delete;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_REMOTECSE_DELETE:
        resourceType = (int)remoteCSE;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_REMOTECSE, ONEM2M_TO, mRemoteCSEResourceName);
        oneM2M_remoteCSE remoteCSE_delete;
        memset(&remoteCSE_delete, 0, sizeof(remoteCSE_delete));
        remoteCSE_delete.ni = (char*)ONEM2M_NODEID;
        remoteCSE_delete.dKey = mDeviceKey;
        pc = (void *)&remoteCSE_delete;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
    case VERIFICATION_NODE_DELETE:
        resourceType = (int)node;
        operation = DELETE;
        snprintf(to, sizeof(to), TO_NODE, ONEM2M_TO, ONEM2M_NODEID);
        oneM2M_node node_delete;
        memset(&node_delete, 0, sizeof(node_delete));
        node_delete.ni = (char*)ONEM2M_NODEID;
        pc = (void *)&node_delete;
        RequestOneM2M(resourceType,operation,fr,to,ri,pc);
        break;
#endif		
    default:
        mStep = VERIFICATION_END;
        break;
    }
}

static int SimpleXmlParser(char* payload, char* name, char* value, int isPC) {
    int rc = 0;
    char start[10];
    char end[10];
    char* pl = payload;
    memset(start, 0, sizeof(start));
    memset(end, 0, sizeof(end));

    if(isPC) {
        pl = strstr(payload, "<pc>");
    }

    snprintf(start, sizeof(start), "<%s>", name);
    snprintf(end, sizeof(end), "</%s>", name);
    char* s = strstr(pl, start) + strlen(start);
    char* e = strstr(pl, end);

    if(s && e) {
        memcpy(value, s, e-s);
        Serial.print("name : ");
        Serial.println(name);
        Serial.print("value : ");
        Serial.println(value);
    }    
    return rc;
}


void MQTTConnected(int result) {
    Serial.print("MQTTConnected : ");    
    Serial.println(result);
}

void MQTTSubscribed(int result) {
    Serial.print("MQTTSubscribed : ");    
    Serial.println(result);
    DoVerificationStep();
}

void MQTTDisconnected(int result) {
    Serial.print("MQTTDisconnected : ");    
    Serial.println(result);
}

void MQTTConnectionLost(char* cause) {
    Serial.print("MQTTConnectionLost: ");    
    Serial.println(cause);
}

void MQTTMessageDelivered(int token) {
    Serial.print("MQTTMessageDelivered : ");    
    Serial.println(token);
}

void MQTTMessageArrived(char* topic, char* msg, int msgLen) {
    Serial.println("MQTTMessageArrived");
    char payload[1024] = "";
    memcpy(payload, msg, msgLen);
    Serial.print("payload : ");
    Serial.println(payload);
    char rsc[10] = "";
    SimpleXmlParser(payload, (char*)ATTR_RSC, rsc, 0);
    char rsm[128] = "";
    SimpleXmlParser(payload, (char*)ATTR_RSM, rsm, 0);

#ifdef ONEM2M_V1_12
    switch(mStep) {
        case VERIFICATION_CSEBASE_RETRIEVE:
            mStep = VERIFICATION_NODE_CREATE;
            break;
    		case VERIFICATION_NODE_CREATE:
            SimpleXmlParser(payload, (char*)ATTR_RI, mNodeLink, 1);
            if(strlen(mNodeLink) == 0) {
                mStep = VERIFICATION_END;
            } else {
                mStep = VERIFICATION_AE_CREATE;
            }
            break;
       case VERIFICATION_AE_CREATE:
            SimpleXmlParser(payload, (char*)ATTR_AEI, mAEID, 1);
            if(strlen(mAEID) == 0) {
                mStep = VERIFICATION_END;
            } else {
                mStep = VERIFICATION_CONTAINER_CREATE;
            }
            break;
        case VERIFICATION_CONTAINER_CREATE:
            mStep = VERIFICATION_MGMTCMD_CREATE;
            break;
		    case VERIFICATION_MGMTCMD_CREATE:
            mStep = VERIFICATION_ACCESSCONTROLPOLICY_CREATE;			
            break;
		    case VERIFICATION_LOCATIONPOLICY_CREATE:
            mStep = VERIFICATION_CONTENTINSTANCE_CREATE;			
            break;
		    case VERIFICATION_ACCESSCONTROLPOLICY_CREATE:
            mStep = VERIFICATION_LOCATIONPOLICY_CREATE;			
            break;			
		    case VERIFICATION_CONTENTINSTANCE_CREATE:
            mStep = VERIFICATION_NODE_UPDATE;
            break;
        case VERIFICATION_NODE_UPDATE:
            mStep = VERIFICATION_AE_UPDATE;
            break;
        case VERIFICATION_REMOTECSE_UPDATE:
            // TODO
            break;
        case VERIFICATION_AE_UPDATE:
            mStep = VERIFICATION_CONTAINER_UPDATE;
            break;    
        case VERIFICATION_CONTAINER_UPDATE:
            mStep = VERIFICATION_MGMTCMD_UPDATE;
            break;
        case VERIFICATION_MGMTCMD_UPDATE:
            mStep = VERIFICATION_LOCATIONPOLICY_UPDATE;
            break;
        case VERIFICATION_LOCATIONPOLICY_UPDATE:
            mStep = VERIFICATION_ACCESSCONTROLPOLICY_UPDATE;
            break;    
        case VERIFICATION_ACCESSCONTROLPOLICY_UPDATE:
            mStep = VERIFICATION_LOCATIONPOLICY_DELETE;
            break;
        case VERIFICATION_NODE_DELETE:
            mStep = VERIFICATION_END;
            break;
        case VERIFICATION_REMOTECSE_DELETE:
            break;
        case VERIFICATION_AE_DELETE:
            mStep = VERIFICATION_NODE_DELETE;
            break;
        case VERIFICATION_CONTAINER_DELETE:
            mStep = VERIFICATION_ACCESSCONTROLPOLICY_DELETE;
            break;
        case VERIFICATION_MGMTCMD_DELETE:
            mStep = VERIFICATION_CONTAINER_DELETE;
            break;
        case VERIFICATION_LOCATIONPOLICY_DELETE:
            mStep = VERIFICATION_MGMTCMD_DELETE;
            break;
        case VERIFICATION_ACCESSCONTROLPOLICY_DELETE:
            mStep = VERIFICATION_AE_DELETE;
            break;
        default:
            break;
    }
    Serial.println("=============================================================================");    
    DoVerificationStep();
#else
    switch(mStep) {
        case VERIFICATION_NODE_CREATE:
            SimpleXmlParser(payload, (char*)ATTR_RI, mNodeLink, 1);
            SimpleXmlParser(payload, (char*)ATTR_HCL, mHostCSELink, 1);
            if(strlen(mNodeLink) == 0) {
                mStep = VERIFICATION_END;
            }
            break;
            
        case VERIFICATION_REMOTECSE_CREATE:
            SimpleXmlParser(payload, (char*)ATTR_DKEY, mDeviceKey, 0);
            SimpleXmlParser(payload, (char*)ATTR_RN, mRemoteCSEResourceName, 0);
            if(strlen(mDeviceKey) == 0 || strlen(mRemoteCSEResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            break;
        case VERIFICATION_LOCATIONPOLICY_CREATE:
            SimpleXmlParser(payload, (char*)ATTR_RN, mLocationPolicyResourceName, 1);
            if(strlen(mLocationPolicyResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            break;
        case VERIFICATION_AE_CREATE:
            SimpleXmlParser(payload, (char*)ATTR_RN, mAEResourceName, 1);
            if(strlen(mAEResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            break;
        case VERIFICATION_CONTENTINSTANCE_CREATE:
            SimpleXmlParser(payload, (char*)ATTR_RN, mContentInstanceResourceName, 1);
            if(strlen(mContentInstanceResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            break;
        case VERIFICATION_MGMTCMD_CREATE:
            SimpleXmlParser(payload, (char*)ATTR_RN, mMgmtCmdResourceName, 1);
            if(strlen(mMgmtCmdResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            break;
        case VERIFICATION_AREANWKINFO_CREATE:
            SimpleXmlParser(payload, (char*)ATTR_RN, mAreaNwkInfoResourceName, 1);
            if(strlen(mAreaNwkInfoResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            break;
        case VERIFICATION_CONTAINER_CREATE:
            SimpleXmlParser(payload, (char*)ATTR_RN, mContainerResourceName, 1);
            if(strlen(mContainerResourceName) == 0) {
                mStep = VERIFICATION_END;
            }
            break;
        default:
            break;
    }
    Serial.println("=============================================================================");    
    mStep = static_cast<VERIFICATION_STEP>((int)mStep + 1);
    DoVerificationStep();
#endif    

}

/**
 * @brief setup
 */
void setup()
{
    byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x30, 0xDF}; // ex : 
	Serial.begin(9600);
	// while (!Serial) {
	//   ; // wait for serial port to connect. Needed for native USB port only
	// }
	Ethernet.begin(mac);
	
    int rc;
    lora.setPrinter (Serial);
    sensor.InitSensor();
    Udp.begin(localPort);
    setSyncProvider(getNtpTime);
    // set callbacks
    rc = tpMQTTSetCallbacks(MQTTConnected, MQTTSubscribed, MQTTDisconnected, MQTTConnectionLost, MQTTMessageDelivered, MQTTMessageArrived);

	// create
	char subscribeTopic[2][128];
	char publishTopic[128] = "";
	memset(subscribeTopic, 0, sizeof(subscribeTopic));
    snprintf(mClientID, sizeof(mClientID), "%s_%s", ACCOUNT_USER, ONEM2M_CLIENTID);
#ifdef ONEM2M_V1_12
    snprintf(subscribeTopic[0], sizeof(subscribeTopic[0]), TOPIC_SUBSCRIBE_REQ, ONEM2M_SERVICENAME, ONEM2M_AE_RESOURCENAME);
    snprintf(subscribeTopic[1], sizeof(subscribeTopic[1]), TOPIC_SUBSCRIBE_RES, ONEM2M_AE_RESOURCENAME, ONEM2M_SERVICENAME);
    snprintf(publishTopic, sizeof(publishTopic), TOPIC_PUBLISH, ONEM2M_AE_RESOURCENAME, ONEM2M_SERVICENAME);
#else
    snprintf(subscribeTopic[0], sizeof(subscribeTopic[0]), TOPIC_SUBSCRIBE_REQ, mClientID);
    snprintf(subscribeTopic[1], sizeof(subscribeTopic[1]), TOPIC_SUBSCRIBE_RES, mClientID);
    snprintf(publishTopic, sizeof(publishTopic), TOPIC_PUBLISH, mClientID, ONEM2M_CSEBASE);
#endif
	char* st[] = {subscribeTopic[0], subscribeTopic[1]};

    rc = tpSDKCreate((char*)MQTT_HOST, MQTT_PORT, MQTT_KEEP_ALIVE, (char*)ACCOUNT_USER, (char*)ACCOUNT_PASSWORD, MQTT_ENABLE_SERVER_CERT_AUTH, st, TOPIC_SUBSCRIBE_SIZE, publishTopic, mClientID);

    Serial.print("tpSDKCreate result : ");
    Serial.println(rc);
}

void loop()
{
	if(mStep >= VERIFICATION_END )
		tpSDKDestory();

	tpSDKYield(1000);
}

unsigned int dummy_battery[] = { 0xf0, 0xf8, 0xfd};
unsigned int dummy_humidity[] = { 0x81, 0x89, 0x8f};
int battery_index = 0;
int humidity_index = 0;
/*-------- LoRa code ----------*/
char* getLoRaInstance() 
{
    char *s;
    unsigned char data[3];

    data[0] = 0x12;
    data[1] = 0x34;
    data[2] = 0x56;

    float light_val;
    float temp_val;
    int tmp;

    light_val = sensor.getLight();
    temp_val = sensor.getTemp();

    // GPS 1
    data[0] = 0xC1;
    data[1] = 0xC3;
    data[2] = 0x7C;
    lora.push ((char)0x21,(char)0x03,(char*)data);

    // GPS 2
    data[0] = 0x39;
    data[1] = 0x52;
    data[2] = 0x69;
    lora.push ((char)0x20,(char)0x03,(char*)data);

    // humidity
    data[0] = (char)(dummy_humidity[humidity_index] & 0x000000ff);
    //data[0] = 0x98;
    data[1] = 0x0;
    data[2] = 0x0;
    humidity_index = (humidity_index+1)%3;
    lora.push ((char)0x26,(char)0x01,(char*)data);

    // light
    Serial.print("Light value is ");
    Serial.println(light_val);
    tmp = (int) (light_val*100 + 0.5);
    data[0] = (tmp >> 8) & 0x000000ff;
    data[1] = tmp & 0x000000ff;
    data[2] = 0x0;
    lora.push ((char)0x25,(char)0x02,(char*)data);

    // temperature 
    Serial.print("Temperature value is ");
    Serial.println(temp_val);
    tmp = (int) (temp_val*100 + 0.5);
    data[0] = (tmp >> 8) & 0x000000ff;
    data[1] = tmp  & 0x000000ff;
    data[2] = 0x0;
    lora.push ((char)0x23,(char)0x02,(char*)data);

    // time
    data[0] = (char)(hour() & 0x000000ff);
    data[1] = (char)(minute() & 0x000000ff);
    data[2] = (char)(second() & 0x000000ff);
    lora.push ((char)0x03,(char)0x03,(char*)data);

    // date
    data[0] = (char)((year() - 2000) & 0x000000ff);
    data[1] = (char)(month() & 0x000000ff);
    data[2] = (char)(day() & 0x000000ff);
    lora.push ((char)0x02,(char)0x03,(char*)data);

    // battery
    data[0] = (char)(dummy_battery[battery_index] & 0x000000ff);
    //data[0] = 0x12;
    data[1] = 0x0;
    data[2] = 0x0;
    battery_index = (battery_index+1)%3;
    lora.push ((char)0x01,(char)0x01,(char*)data);

    s = lora.getInstance();
    Serial.print("Lora: ");
    Serial.println(s);

    lora.pop();
    lora.pop();
    lora.pop();
    lora.pop();
    lora.pop();
    lora.pop();
    lora.pop();
    lora.pop();

    return s;
}

/*-------- NTP code ----------*/
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

