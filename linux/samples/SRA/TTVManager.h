
/**
 * @file TTVManager.h
 *
 * @brief Define Structures & Functions to handle TTV
 *
 * Copyright (C) 2017. SK Telecom, All Rights Reserved.
 * Written 2017,by SK Telecom
 */


#ifndef _TTVMANAGER_H_
#define _TTVMANAGER_H_

#include "List.h"

typedef struct tdv_t {
    char mTag;
    char mDataType;
    short mLength;
    char *mValue;
} TTV_NODE_T;

typedef struct tdv_context_t {
    unsigned short mCount;
    unsigned short mValueLength;
    char *mInstance;
    char mUpdate;
    DMList mTTVList;
} TTV_CTX_T;

void TTVRelease(void *node);
void* CreateTTV(void);
void DestroyTTV(void *TTV);

void* CreateTTVNode(void);
void SetTTVNode(const char tag, const short len, const char datatype, const char *value, void *TTVNode);
char GetTTVTag(void *TTVNode);
char GetTTVDataType(void *TTVNode);
short GetTTVLength(void *TTVNode);
char* GetTTVValue(void *TTVNode);
void DestroyTTVNode(void *TTVNode);

void PushTTVNode(void *TTVNode, void *TTV);
void* PopTTVNode(void *TTV);

void MakeTTVInstance(void *TTV);
char* GetTTVInstance(void *TTV);
int GetTTVInstanceSize(void *TTV);

void PrintTTV(void *TTV);

#endif//_TTVMANAGER_H_


