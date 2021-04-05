//
// Created by 田沛林 on 2020/12/5.
//

#ifndef PLT_PROJET_OPTREE_H
#define PLT_PROJET_OPTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct OpTreeNode {
    char op;       // for operators: '+', '-', 'x' (0 for none)
    int priority;  // priority of operator
    int value;     // for natural numbers
    struct OpTreeNode *father, *lChild, *rChild;
} *pOpTreeNode;

pOpTreeNode opTreeNodeNew();
void opTreeFree(pOpTreeNode root);

#endif //PLT_PROJET_OPTREE_H
