#ifndef _LIST_H
#define _LIST_H

#define NAME_LEN 256
#define STR_LEN 1024
#define COMMAND_INS book bk

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include <mqueue.h>

struct book 
{
    mqd_t fl;
    char name[256];
}typedef book;

struct list
{
    void *ptr;
    COMMAND_INS;
}typedef list;

static list* head = 0;

list* getAt(int id);
void pushBack(void *ptr);
int remove_at(int num);
#endif