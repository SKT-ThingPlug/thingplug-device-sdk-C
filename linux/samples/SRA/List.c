
/**
 * @file List.c
 *
 * @brief DeviceMiddleware List API
 *
 * Copyright (C) 2016. SK Telecom, All Rights Reserved.
 * Written 2016,by SK Telecom 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "List.h"


void DMList_Init(DMList *list, Releaser relfn)
{
    list->count = 0;
    list->free = relfn;
    list->head = NULL;
    list->tail = NULL;
    pthread_mutex_init(&list->mutex, NULL);
}

void DMList_Close(DMList *list)
{
    DMListNode *tmp,*next;
    if( list == NULL || list->head == NULL || list->tail == NULL || list->count == 0) return;

    pthread_mutex_lock(&list->mutex);
    tmp = list->head;
    do{
        next = tmp->next;
        list->free(tmp->data);
        free(tmp);
        tmp = next;
    }while(tmp != list->head);
    memset(list, 0, sizeof(DMList));
    pthread_mutex_unlock(&list->mutex);
}

void DMList_Add(DMList *list, void *data)
{
    DMListNode *node = (DMListNode *)malloc(sizeof(DMListNode));
    node->data = data;

    pthread_mutex_lock(&list->mutex);
    if( list->head != NULL) {
        // list is not empty
        node->prev = list->tail;
        node->next = list->head;
        list->tail->next = node;
        list->tail = node;
        list->head->prev = node;
    } else { 
        // list is empty
        list->head = node;
        list->tail = node;
        node->next = node;
        node->prev = node;
    }
    list->count++;
    pthread_mutex_unlock(&list->mutex);
}

DMListNode* DMList_Remove(DMList *list)
{
    if( list == NULL || list->head == NULL || list->tail == NULL || list->count == 0) return NULL;

    DMListNode *delNode = list->tail;

    pthread_mutex_lock(&list->mutex);
    if( list->count == 1) {
        list->head = NULL;
        list->tail = NULL;
    } else {
        list->tail = delNode->prev;
        list->tail->next = delNode->next;
        list->head->prev = list->tail;
    }
    list->count--;
    pthread_mutex_unlock(&list->mutex);

    return delNode;
}


DMListNode* DMList_GetNode(DMList *list, void *data, CompareTo compfn)
{
    DMListNode *tmp;
    if( list == NULL || data == NULL || compfn == NULL || list->head == NULL || list->tail == NULL || list->count == 0) {
        return NULL;
    }

    tmp = list->head;
    do{
        if(compfn(tmp->data,data)) {
            return tmp;
        }
        tmp = tmp->next;
    }while(tmp != list->head);

    return NULL;
}

void DMList_Print(DMList *list, Printer printfn)
{
    DMListNode *tmp;
    if( list == NULL || printfn == NULL) return;
        
    tmp = list->head;
    do{
        printfn(tmp->data);
        tmp = tmp->next;
    }while(tmp != list->head);
}

