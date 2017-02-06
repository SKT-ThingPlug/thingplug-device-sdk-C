/**
 * @file oneM2M_V1.h
 *
 * @brief The header file for oneM2M ver.1
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016, by SK Telecom
 */
#ifndef _oneM2M_V1_H_
#define _oneM2M_V1_H_

#include "oneM2MCode.h"

//SKT oneM2M V1 definition
#define ATTR_PASSCODE   "passCode"  // passcode
#define ATTR_DKEY       "dKey"      // deviceKey
#define ATTR_UKEY       "uKey"      // userKey
#define ATTR_MGA        "mga"

#define XML_HEADER_V1   "<m2m:req xmlns:m2m=\"http://www.onem2m.org/xml/protocols\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.onem2m.org/xml/protocols CDT-requestPrimitive-v1_0_0.xsd\"><cty>application/vnd.onem2m-prsp+xml</cty>"
#define XML_FOOTER_V1   "</m2m:req>"

#define RESOURCE_STR(resourceType)                      \
    (resourceType == contentInstance ? ATTR_CIN:       \
    (resourceType == execInstance ? ATTR_EXIN:       \
    (resourceType == container ? ATTR_CNT:             \
    (resourceType == mgmtCmd ? ATTR_MGC:               \
    (resourceType == node ? ATTR_NOD:                  \
    (resourceType == AE ? ATTR_AE:                     \
    (resourceType == locationPolicy ? ATTR_LCP:        \
    (resourceType == mgmtObj ? ATTR_ANI:               \
    (resourceType == remoteCSE ? ATTR_CSR:NULL)))))))))

typedef struct
{
    /** node id **/
    char* ni;
} oneM2M_CSEBase;

typedef struct
{
    /** node id **/
    char* ni;
    /** deviceKey **/
    char* dKey;
    /** hostCSELink **/
    char* hcl;
    /** mga **/
    char* mga;
} oneM2M_node;

typedef struct
{
    /** node id **/
    char* ni;
    /** name **/
    char* nm;
    /** deviceKey **/
    char* dKey;
    /** passcode **/
    char* passCode;
    /** cse type **/
    char* cst;
    /** point of access **/
    char* poa;
    /** request rechability **/
    char* rr;
    /** node link**/
    char* nl;
} oneM2M_remoteCSE;

typedef struct
{
    /** node id **/
    char* ni;
    /** deviceKey **/
    char* dKey;
    /** App-ID **/
    char* api;
    /** appName **/
    char* apn;
} oneM2M_AE;


typedef struct
{
    /** node id **/
    char* ni;
    /** name **/
    char* nm;
    /** deviceKey **/
    char* dKey;
    /** lbaels **/
    char* lbl;
} oneM2M_container;

typedef struct
{
    /** node id **/
    char* ni;
    /** name **/
    char* nm;
    /** deviceKey **/
    char* dKey;
    /** userKey **/
    char* uKey;
    /** cmdType **/
    char* cmt;
    /** execEnable **/
    char* exe;
    /** execTarget **/
    char* ext;
    /** labels **/
    char* lbl;
} oneM2M_mgmtCmd;

typedef struct
{
    /** node id **/
    char* ni;
    /** deviceKey **/
    char* dKey;
    /** execResult **/
    char* exr;
    /** execStatus **/
	char* exs;
} oneM2M_mgmtCmdResult;

typedef struct
{
    /** node id **/
    char* ni;
    /** deviceKey **/
    char* dKey;
    /** content info **/
    char* cnf;
    /** content **/
    char* con;
    /** labels **/
    char* lbl;
} oneM2M_contentInstance;

typedef struct
{
    /** node id **/
    char* ni;
    /** name **/
    char* nm;
    /** deviceKey **/
    char* dKey;
    /** locationSource **/
    char* los;
    /** labels **/
    char* lbl;
} oneM2M_locationPolicy;

typedef struct
{
    /** node id **/
    char* ni;
    /** name **/
    char* nm;
    /** deviceKey **/
    char* dKey;
    /** mgmtDefinition **/
    char* mgd;
    /** areaNwkType **/
    char* ant;
    /** listOfDevices **/
    char* ldv;
} oneM2M_areaNwkInfo;

int tp_oneM2M_V1_Request(int resourceType, int operation, char* to, char* ri, void* pc);

int tpRegisterDevice(int resourceType, char* fr, char* to, char* ri, char* mga, char* nm, char* cst, char* passcode, char* poa, char* nl);

int tpRegisterContainer(char* fr, char* to, char* ri, char* nm, char* dKey, char* lbl);

int tpRegisterMgmtCmd(char* fr, char* to, char* ri, char* nm, char* dKey, char* cmt, char* exe, char* ext, char* lbl);

int tpAddData(char* data, unsigned char length);

int tpReport(char* fr, char* to, char* ri, char* dKey, char* cnf, char* con, unsigned char useAddedData);

int tpResult(char* fr, char* to, char* ri, char* dKey, char* exr, char* exs);


#endif 
