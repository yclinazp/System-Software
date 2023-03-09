#ifndef TREE_H
#define TREE_H

#include "Array.h"

typedef struct {
  char *type;                             //樹節點的類型
  char *value;                            //樹節點的值
  Array* childs;                          //樹節點的子節點
} Tree;

Tree* TreeNew(char *pType, char *pValue);
void TreeFree(Tree *tree);
void TreeAddChild(Tree *tree, Tree *child);
void TreePrint(Tree *tree);

#endif
