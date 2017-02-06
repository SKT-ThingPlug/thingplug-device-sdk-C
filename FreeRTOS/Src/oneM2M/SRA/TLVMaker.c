/**
 * @file TLVMaker.c
 *
 * @brief Implementation make tlv value
 *
 * Copyright (C) 2016. SK Telecom,All Rights Reserved.
 * Written 2016,by SK Telecom
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <fcntl.h>
#include <stdarg.h>

#include "TLVMaker.h"


/*
 ****************************************
 * TLV Implementations
 ****************************************
 */

#define __TLV_DEBUG__

void TLV_DEBUG(const char *format, ...)
{
#ifdef __TLV_DEBUG__
    FILE *gDebugStream = stdout;
	va_list argp;
	va_start(argp, format);
	vfprintf(gDebugStream, format, argp);
	va_end(argp);
	fflush(gDebugStream);
	printf("\r");
#endif
}

void TLVRelease(void *TLVchunk)
{
    TLV_CNK_T *chunk = (TLV_CNK_T*)TLVchunk;
    if( chunk->mValue != NULL) {
        free(chunk->mValue);
    }
    free(chunk);
}

void* CreateTLV(void)
{
    TLV_CTX_T *context = (TLV_CTX_T *)calloc(1, sizeof(TLV_CTX_T));
    context->mCount = 0;
    context->mValueLength = 0;
    context->mInstance = NULL;
    context->mUpdate = 0x0;
    DMList_Init(&context->mTLVList, TLVRelease);
    return (void*)context;
}

void DestroyTLV(void *tlv)
{
    TLV_CTX_T *ctx = (TLV_CTX_T *) tlv;

    if(ctx == NULL) return;

    if(ctx->mInstance != NULL) {
        TLV_DEBUG("[TLVMaker] DESTROY INSTANCE <%s>\n", ctx->mInstance);
        free(ctx->mInstance);
    }
    DMList_Close(&ctx->mTLVList);

    free(ctx);
}

void* CreateChunk(void)
{
    return calloc(1, sizeof(TLV_CNK_T));
}

void SetChunk(const char tag, const char length, const char* value, void* TLVchunk)
{
    TLV_CNK_T *chunk = (TLV_CNK_T *)TLVchunk;
    if( chunk->mValue != NULL ) free(chunk->mValue);

    chunk->mTag = tag;
    chunk->mLength = length;
    chunk->mValue = calloc(1, length);
    memcpy(chunk->mValue, value, length);
}

char GetTag(void *TLVchunk)
{
    TLV_CNK_T *chunk = (TLV_CNK_T *) TLVchunk;
    return chunk->mTag;
}

char GetLength(void *TLVchunk)
{
    TLV_CNK_T *chunk = (TLV_CNK_T *) TLVchunk;
    return chunk->mLength;
}

char* GetValue(void *TLVchunk)
{
    TLV_CNK_T *chunk = (TLV_CNK_T *) TLVchunk;
    return chunk->mValue;
}

void DestroyChunk(void *TLVchunk)
{
    TLV_CNK_T *chunk = (TLV_CNK_T *) TLVchunk;
    if(chunk == NULL) return;
    TLV_DEBUG("[TLVMaker] DESTROY TLV <TAG = 0x%x, LEN = 0x%x\n", chunk->mTag, chunk->mLength);
    if( chunk->mValue != NULL ) free(chunk->mValue);
    free(chunk);
}


void PushChunk(void *TLVchunk, void *tlv)
{
    TLV_CTX_T *ctx = (TLV_CTX_T *) tlv;
    TLV_CNK_T *chunk = (TLV_CNK_T *) TLVchunk;
    DMList_Add(&ctx->mTLVList, TLVchunk);
    ctx->mCount++;
    ctx->mValueLength += (unsigned short)chunk->mLength;
    ctx->mUpdate = 0x1;
    TLV_DEBUG("[TLVMaker] PUSH <TAG = 0x%x, LEN = 0x%x\n", chunk->mTag, chunk->mLength);
}

void* PopChunk(void *tlv)
{
    TLV_CTX_T *ctx = (TLV_CTX_T *) tlv;
    TLV_CNK_T *chunk;
    DMListNode *node;

    if( ctx->mTLVList.count == 0) {
        TLV_DEBUG("[TLVMaker] List is Empty!! \n");
        return NULL;
    }

    node = DMList_Remove(&ctx->mTLVList);
    chunk = (TLV_CNK_T *)node->data;
    ctx->mCount--;
    ctx->mValueLength -= (unsigned short)chunk->mLength;
    ctx->mUpdate = 0x1;

    return (void *)chunk;
}

void MakeTLV(void *tlv)
{
    char *instanceHex;
    char *instanceBinary;
    int instanceLength;
    int i,pos = 0;
    TLV_CTX_T *ctx = (TLV_CTX_T *) tlv;
    TLV_CNK_T *chunk;
    DMListNode *node;

    if(ctx->mTLVList.count == 0) {
        TLV_DEBUG("[TLVMaker] List is Empty!! \n");
        return;
    }

    if(ctx->mInstance != NULL) {
        free(ctx->mInstance);
    }

    instanceLength = ctx->mCount*2 + ctx->mValueLength;

    instanceBinary = (char *)malloc( sizeof(char) * instanceLength);
    instanceHex = (char *)malloc( 2 * sizeof(char) * instanceLength + 1);

    node = ctx->mTLVList.head;
    do{
        chunk = (TLV_CNK_T *)node->data;
        int i;
        TLV_DEBUG("[TLVMaker] INSTANCE <<==  <TAG = 0x%x, LEN = 0x%x\n",chunk->mTag, chunk->mLength);
        instanceBinary[pos++] = chunk->mTag;
        instanceBinary[pos++] = chunk->mLength;
        for (i = 0; i < (unsigned short)chunk->mLength; i++) {
            instanceBinary[pos++] = chunk->mValue[i];
        }
        node = node->next;
    }while( node != ctx->mTLVList.head );

    for( i = 0; i < instanceLength; i++) {
        snprintf(&instanceHex[i*2],3,"%02x",instanceBinary[i]);
    }

    free(instanceBinary);
    ctx->mInstance = instanceHex;
    ctx->mUpdate = 0x0;
}

char* GetTLV(void *tlv)
{
    TLV_CTX_T *ctx = (TLV_CTX_T *) tlv;
    return ctx->mInstance;
}

int GetTLVSize(void *tlv)
{
    TLV_CTX_T *ctx = (TLV_CTX_T *) tlv;
    if( ctx->mUpdate != 0x0 ) {
        TLV_DEBUG("[TLVMaker] You have to call function -> MakeInstance()\n");
        return 0;
    }
    return (int)2*(ctx->mCount*2 + ctx->mValueLength);
}

void PrintChunk(void *TLVchunk)
{
    TLV_CNK_T *chunk = (TLV_CNK_T *) TLVchunk;
    TLV_DEBUG("[TLVMaker] PRINT <TAG = 0x%x, LEN = 0x%x, VAL = %s>\n", chunk->mTag, chunk->mLength, chunk->mValue);
}

void PrintTLV(void *tlv)
{
    TLV_CTX_T *ctx = (TLV_CTX_T *) tlv;
    TLV_DEBUG("[TLVMaker] PRINT <ctx->mCount = %hu, ctx->mValueLength = %hu> \n", ctx->mCount, ctx->mValueLength);

    DMList_Print(&ctx->mTLVList, PrintChunk);
}

