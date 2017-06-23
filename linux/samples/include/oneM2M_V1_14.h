/**
 * @file oneM2M_V1_14.h
 *
 * @brief The header file for oneM2M ver.1.14
 *
 * Copyright (C) 2017. SK Telecom, All Rights Reserved.
 * Written 2017, by SK Telecom
 */
#ifndef _oneM2M_V1_14_H_
#define _oneM2M_V1_14_H_

#include "oneM2MCode.h"

//SKT oneM2M V1.14 definition
// #define ATTR_PPT            "ppt"
#define ATTR_MGA            "mga"
#define ATTR_LIT			"lit"	// locationInformationType
#define PREFIX_RQI          100


// by smartcity standard
#define XML_HEADER_V1_14   "<?xml version=\"1.0\" encoding=\"UTF-8\"?><m2m:rqp xmlns:m2m=\"http://www.onem2m.org/xml/protocols\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.onem2m.org/xml/protocols CDT-requestPrimitive-v1_0_0.xsd\">"
#define XML_FOOTER_V1_14   "</m2m:rqp>"

#define RESOURCE_STR_V1_14(resourceType)               \
    (resourceType == contentInstance ? ATTR_CIN:       \
    (resourceType == execInstance ? ATTR_EXIN:         \
    (resourceType == container ? ATTR_CNT:             \
    (resourceType == mgmtCmd ? ATTR_MGC:               \
    (resourceType == subscription ? ATTR_SUB:          \
    (resourceType == AE ? ATTR_AE:                     \
    (resourceType == locationPolicy ? ATTR_LCP:        \
    (resourceType == accessControlPolicy ? ATTR_ACP:   \
    (resourceType == remoteCSE ? ATTR_CSR:NULL)))))))))

// typedef struct
// {
//     /** resourceName **/
//     char* rn;
//     /** expirationTime **/
//     char* et;
//     /** accessControlPolicyID **/
//     char* acpi;    
//     /** labels **/
//     char* lbl;
//     /** CSEtype **/
//     char* cst;
//     /** pointOfAccess **/
//     char* poa;
//     /** CSEBase **/
//     char* cb;
//     /** CSE-ID **/
//     char* csi;
//     /** nodelink**/
//     char* nl;
// 	/** requestRechability **/
//     char* rr;
//     /** property **/
//     char* ppt;
// } oneM2M_v1_14_remoteCSE;

typedef struct
{
    /** resourceName **/
    char* rn;
    /** expirationTime **/
    char* et;
    /** accessControlPolicyID **/
    char* acpi;   
    /** labels **/
    char* lbl;
    /** appName **/
    char* apn;    
    /** App-ID **/
    char* api;
    /** pointOfAccess **/
    char* poa;
    /** property **/
    // char* ppt;
    /** requestRechability **/
    char* rr;
    /** nodeId **/
    char* ni;
    /** mga **/
    char* mga;
} oneM2M_v1_14_AE;


typedef struct
{
    /** resourceName **/
    char* rn;
    /** expirationTime **/
    char* et;
    /** accessControlPolicyID **/
    char* acpi;   
    /** labels **/
    char* lbl;
    /** maxNrOfInstances **/
    char* mni;    
    /** maxByteSize **/
    char* mbs;
    /** maxInstanceAge **/
    char* mia;
    /** locationID **/
    char* li;
    /** property **/
    // char* ppt;
} oneM2M_v1_14_container;

typedef struct
{
    /** resourceName **/
    char* rn;
    /** expirationTime **/
    char* et;
    /** accessControlPolicyID **/
    char* acpi;   
    /** labels **/
    char* lbl;
    /** description **/
    char* dc;
    /** cmdType **/
    char* cmt;    
    /** execReqArgs **/
    char* exra;
    /** execTarget **/
    char* ext;
    /** execMode **/
    char* exm;
    /** execFrequency **/
    char* exf;
    /** execDelay **/
    char* exy;
    /** execNumber **/
    char* exn;
} oneM2M_v1_14_mgmtCmd;

typedef struct
{
    /** resourceName **/
    char* rn;
    /** accessControlPolicyID **/
    char* acpi;   
    /** labels **/
    char* lbl;
    /** execDisable **/
    char* exd;
    /** execResult **/
    char* exr;
	/** execStatus **/
    char* exs;
    /** execTarget **/
	char* ext;
} oneM2M_v1_14_execInstance;

typedef struct
{
    /** resourceName **/
    char* rn;
    /** expirationTime **/
    char* et;
    /** labels **/
    char* lbl;
    /** contentInfo **/
    char* cnf;
    /** content **/
    char* con;    
} oneM2M_v1_14_contentInstance;

typedef struct
{
    /** resourceName **/
    char* rn;
    /** expirationTime **/
    char* et;
    /** accessControlPolicyID **/
    char* acpi;   
    /** labels **/
    char* lbl;
    /** locationSource **/
    char* los;
    /** locationUpdatePeriod **/
    char* lou;
    /** locationTargetID **/
    char* lot;
    /** locationServer **/
    char* lor;
    /** locationContainerName **/
    char* lon;
	/** locationInformationType **/
	char* lit;
} oneM2M_v1_14_locationPolicy;

// typedef struct
// {
//     /** resourceName **/
//     char* rn;
//     /** expirationTime **/
//     char* et;
//     /** labels **/
//     char* lbl;
//     /** privileges **/
//     char* pv;
//     /** selfPrivileges **/
//     char* pvs;
// } oneM2M_v1_14_accessControlPolicy;

typedef struct
{
    /** resourceName **/
    char* rn;
    /** eventNotificationCriteria **/
    char* enc;
    /** notificationURI **/
    char* nu;
    /** notificationContentType **/
    char* nct;

} oneM2M_v1_14_subscription;

int tp_oneM2M_v1_14_Request(int resourceType, int operation, char* fr, char* to, void* pc);

int tp_v1_14_RegisterDevice(char* fr, char* to, char* rn, char* ni, char* api, char* serviceId, char* clientId);

int tp_v1_14_RegisterContainer(char* fr, char* to, char* rn);

int tp_v1_14_RegisterMgmtCmd(char* fr, char* to, char* rn, char* cmt, char* ext);

int tp_v1_14_AddData(char* data, unsigned char length);

int tp_v1_14_Report(char* fr, char* to, char* cnf, char* con, unsigned char useAddedData);

int tp_v1_14_Result(char* fr, char* to, char* exr, char* exs);


#endif 

