#include <stdio.h>
#include "Tree.h"

Tree *TreeNew(char *type, char *value) {
  ASSERT(type != NULL &&  value != NULL);                               
  Tree *rzTree = ObjNew(Tree, 1);                                      // allocate Tree type* 1 count memory
  rzTree->type = (void*) newStr(type);
  rzTree->value = (void*) newStr(value);                               // allocate type & value
  rzTree->childs = NULL;
//  printf("TreeNew(%s,%s)\n", type, value);
//  printf("address of tree->type=%d, tree->value=%d\n", rzTree->type, rzTree->value);
  return rzTree;
}

void TreeFree(Tree *tree) {
  if (tree == NULL) return;
  freeMemory(tree->type);
  freeMemory(tree->value);                                            // free type & value
  if (tree->childs != NULL) {
    int i;
    for (i=0; i<tree->childs->count; i++)
      TreeFree(tree->childs->item[i]);                                // free 每個子節點memory
    ArrayFree(tree->childs, NULL);                                    // free Array childs
  }
  ObjFree(tree);                                                      // free tree
}
// 將子樹加入父節點
void TreeAddChild(Tree *tree, Tree *child) {                                                     
  ASSERT(tree != NULL && child != NULL);
  if (tree->childs == NULL)                                           // if child為NULL
    tree->childs = ArrayNew(1);                                       // allocate Array type(item=1)
  ArrayAdd(tree->childs, child);                                      // add child into Array
}
// 將所有子節點的level和type印出
void TreePrintLevel(Tree *tree, int level) {             
  ASSERT(tree != NULL && tree->type != NULL);
// printf("address of tree->type=%d, tree->value=%d\n", tree->type, tree->value);
  printf("%s+%s\t = %s\n", strSpaces(level), tree->type, tree->value);
  if (tree->childs != NULL) {                                         // 子節點不為NULL
    int i;
    for (i=0; i<tree->childs->count; i++)                             // 對於每個子節點
      TreePrintLevel(tree->childs->item[i], level + 1);               // 遞迴方式呼叫自己
    printf("%s-%s\n", strSpaces(level), tree->type);                  // print level(空格代表level) & type
  }
}

void TreePrint(Tree *tree) {
  TreePrintLevel(tree, 0);
}
