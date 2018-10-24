#ifndef _MYTYPES_H
#define _MYTYPES_H


#define ASCENDING 1
#define DESCENDING 0

typedef struct _node {
  int presence;
  char* name;
  int temperature;
  struct _node *next;
  struct _node *previous;
} nodeType;

typedef nodeType *pNode;
typedef nodeType *List;

#endif
