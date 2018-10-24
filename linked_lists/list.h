#ifndef _LIST_H
#define _LIST_H

#include "mytypes.h"

void Insert(List *l, int pres, char* name, int temp);
void Delete(List *l, char* name);

void DeleteList(Lista *);
void DisplayList(Lista l, int order);

#endif
