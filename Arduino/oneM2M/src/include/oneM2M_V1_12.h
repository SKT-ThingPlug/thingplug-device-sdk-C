/**
 * @file oneM2M_V1_12.h
 *
 * @brief The header file for oneM2M ver.1.12
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */
#ifndef _oneM2M_V1_12_H_
#define _oneM2M_V1_12_H_

#include "oneM2MCode.h"

#ifdef __cplusplus
extern "C" {
#endif

//SKT oneM2M V1.12 definition
#define ATTR_PPT            "ppt"
#define ATTR_MGA            "mga"
#define ATTR_LIT			"lit"	// locationInformationType

// by smartcity standard
#define XML_HEADER_V1_12   "<?xml version=\"1.0\" encoding=\"UTF-8\"?><m2m:rqp xmlns:m2m=\"http://www.onem2m.org/xml/protocols\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.onem2m.org/xml/protocols CDT-requestPrimitive-v1_0_0.xsd\">"
#define XML_FOOTER_V1_12   "</m2m:rqp>"

#define RESOURCE_STR_V1_12(resourceType)                \
    (resourceType == contentInstance ? ATTR_CIN:       \
    (resourceType == execInstance ? ATTR_EXIN:         \
    (resourceType == container ? ATTR_CNT:             \
    (resourceType == mgmtCmd ? ATTR_MGC:               \
    (resourceType == node ? ATTR_NOD:                  \
    (resourceType == AE ? ATTR_AE:                     \
    (resourceType == locationPolicy ? ATTR_LCP:        \
    (resourceType == accessControlPolicy ? ATTR_ACP:   \
    (resourceType == remoteCSE ? ATTR_CSR:NULL)))))))))

//typedef struct
//{
//    /** from **/
//    char* fr;
//} oneM2M_CSEBase;

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
    /** nodeID **/
    char* ni;
    /** mga **/
    char* mga;
    /** hostCSELink **/
//    char* hcl;
} oneM2M_v1_12_node;

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
    /** CSEtype **/
    char* cst;
    /** pointOfAccess **/
    char* poa;
    /** CSEBase **/
    char* cb;
    /** CSE-ID **/
    char* csi;
    /** nodelink**/
    char* nl;
	/** requestRechability **/
    char* rr;
    /** property **/
    char* ppt;
} oneM2M_v1_12_remoteCSE;

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
    char* ppt;
    /** requestRechability **/
    char* rr;
} oneM2M_v1_12_AE;


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
    char* ppt;
} oneM2M_v1_12_container;

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
} oneM2M_v1_12_mgmtCmd;

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
} oneM2M_v1_12_execInstance;

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
} oneM2M_v1_12_contentInstance;

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
} oneM2M_v1_12_locationPolicy;

typedef struct
{
    /** resourceName **/
    char* rn;
    /** expirationTime **/
    char* et;
    /** labels **/
    char* lbl;
    /** privileges **/
    char* pv;
    /** selfPrivileges **/
    char* pvs;
} oneM2M_v1_12_accessControlPolicy;

int tp_oneM2M_V1_12_Request(int resourceType, int operation, char* fr, char* to, char* rqi, void* pc);

int tp_v1_12_RegisterDevice(int resourceType, char* fr, char* to, char* rqi, char* rn, char* ni, char* mga, char* poa, char* api);

int tp_v1_12_RegisterContainer(char* fr, char* to, char* rqi, char* rn);

int tp_v1_12_RegisterMgmtCmd(char* fr, char* to, char* rqi, char* rn, char* cmt, char* ext);

int tp_v1_12_AddData(char* data, unsigned char length);

int tp_v1_12_Report(char* fr, char* to, char* rqi, char* cnf, char* con, unsigned char useAddedData);

int tp_v1_12_Result(char* fr, char* to, char* rqi, char* exr, char* exs);


#ifdef __cplusplus
}
#endif
#endif 

