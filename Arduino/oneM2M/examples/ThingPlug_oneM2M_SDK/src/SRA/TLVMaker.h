/**
 * @file TLVMaker.h
 *
 * @brief Define Structures & Functions to TLV
 *
 * Copyright (C) 2016. SK Telecom,All Rights Reserved.
 * Written 2016,by SK Telecom
 */

#ifndef __TLVMAKER_H__
#define __TLVMAKER_H_

#include "List.h"

typedef struct _tlv_chunk_t {
    char mTag;
    char mLength;
    char *mValue;
} TLV_CNK_T;

typedef struct _tlv_context_t {
    unsigned short mCount;
    unsigned short mValueLength;
    char *mInstance;
    char mUpdate;
    DMList mTLVList;
} TLV_CTX_T;

void* CreateTLV(void);
void DestroyTLV(void *tlv);

void* CreateChunk(void);
void SetChunk(const char tag, const char length, const char *value, void *TLVchunk);
char GetTag(void *TLVchunk);
char GetLength(void *TLVchunk);
char* GetValue(void *TLVchunk);
void DestroyChunk(void *TLVchunk);

void PushChunk(void *TLVchunk, void *tlv);
void* PopChunk(void *tlv);

void MakeTLV(void *tlv);
char* GetTLV(void *tlv);
int GetTLVSize(void *tlv);

void PrintTLV(void *tlv);

#endif//__TLVMAKER_H__

