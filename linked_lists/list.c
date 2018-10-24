#include <stdlib.h>
#include <stdio.h>
#include "list.h"

void Insert(List *list, int pres, char* name, int temp)
{
  pNode new, curent;

  /* Create a new node */
  new = (pNode)malloc(sizeof(nodeType));
  new->presence = pres;
  new->name = name;
  new->temperature =temp;

  /* Place current node in the first position of the list */
  current = *list;
  if(current) while(current->previous) current = current->previous;
  /* If the list is empty or the first node's name is greater than the previous node's name */
  if(!current || current->name > name) {
    /* Add the list after the node */
    new->next = current;
    new->previous = NULL;
    if(current) current->previous = new;
    if(!*list) *list = new;
  }
  else {
    /* Push until end of list is reached or name's condition is true */
    while(current->next && current->next->name <= name)
      current = current->next;
    /* Insert the new node after the previous node */
    new->next = current->next;
    current->next = new;
    new->previous = current;
    if(new->next) new->next->previous = new;
  }
}

void Delete(list *list, char* name)
{
  pNode node;

  /* Search for the node named v */
  node = *list;
  while(node && node->name < name) node = node->next;
  while(node && node->name > name) node = node->previous;

  /* The name is not on the list*/
  if(!node || node->name != name) return;

  /* Delete the node */
  /* If list was pointing to the node in deletion, point it to another*/
  if(node == *list)
    if(node->previous) *list = node->previous;
    else *list = node->next;

  if(node->previous) /* is not the first element */
    node->previous->next = node->next;
  if(node->next) /* is not the last element */
    node->next->previous = node->previous;
  free(node);
}

void DeleteList(list *list)
{
  pNode node, current;

  current = *list;
  while(current->previous) current = current->previous;

  while(current) {
    node = current;
    current = current->next;
    free(node);
  }
  *list = NULL;
}

void DisplayList(list list, int order)
{
  pNode node = list;

  if(!list) printf("list emty");

  node = list;
  if(order == ASCENDING) {
    while(node->previous) node = node->previous;
    printf("Ascending order: ");
    while(node) {
      printf("%s-%d-%d -> ", node->name,node->temperature,node->presence);
      node = node->next;
    }
  }
  else {
    while(node->next) node = node->next;
    printf("Descending order: ");
    while(node) {
      printf("%s-%d-%d -> ", node->name,node->temperature,node->presence);
      node = node->previous;
    }
  }

  printf("\n");
}
