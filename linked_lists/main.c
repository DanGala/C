#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "mytypes.h"


int main()
{
  List list = NULL;
  pNode p;

  Insert(&list, 0, "A126",25);
  Insert(&list, 1, "A127", 27);
  Insert(&list, 0, "A128",40);
  Insert(&list, 1, "B105", 35);

  DisplayList(lista, ASCENDING);
  DisplayList(lista, DESCENDING);

  Delete(&list, "A127");
  Delete(&list, "A128");

  DisplayList(list, ASCENDING);
  DisplayList(list, DESCENDING);

  DeleteList(&list);

  return 0;
}
