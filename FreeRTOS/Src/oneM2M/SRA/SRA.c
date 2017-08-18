
/**
 * @file SRA.c
 *
 * @brief ServiceReadyAgent Process
 *
 * Copyright (C) 2016. SKT, All Rights Reserved.
 * Written 2016,by SKT
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "SRA.h"
#include "SMA.h"
#include "TTVManager.h"
#include "TTVBuilder.h"
#include "NTPClient.h"

#define TIME_TTV_TYPE 0x03
#define TIME_TTV_DATATYPE 0x0d

void SRAGetTTV(char **out_buf, char type, char datatype, char *value )
{
    char *ndata;
    int nlen;
    nlen = TTVConvertData(type, datatype, value, &ndata);
    void *ttv, *node;
    ttv = CreateTTV();
    node = CreateTTVNode();
    SetTTVNode(type, nlen, datatype, ndata, node);
    PushTTVNode( node, ttv);
    free(ndata);
    MakeTTVInstance(ttv);
    *out_buf = (char *)calloc(1, GetTTVInstanceSize(ttv)+1);
    memcpy((*out_buf), GetTTVInstance(ttv), GetTTVInstanceSize(ttv));
    DestroyTTV(ttv);
}


void SRAGetTTVTime(char **out_buf)
{
	unsigned int tmp = time(NULL) + get_npt_offset();
	SRAGetTTV(out_buf, TIME_TTV_TYPE, TIME_TTV_DATATYPE, (char*)&tmp);
}
