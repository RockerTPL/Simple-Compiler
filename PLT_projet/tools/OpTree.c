//
// Created by 田沛林 on 2020/12/5.
//

#include "OpTree.h"

pOpTreeNode opTreeNodeNew() {
    /* create a new tree node */
    pOpTreeNode pNode = (pOpTreeNode)malloc(sizeof(struct OpTreeNode));
    pNode->op = 0;
    pNode->priority = 0;
    pNode->value = 0;
    pNode->father = pNode->lChild = pNode->rChild = NULL;
    return pNode;
}

void opTreeFree(pOpTreeNode root) {
    /* free a tree */
    if (root != NULL) {
        pOpTreeNode lChild = root->lChild;
        pOpTreeNode rChild = root->rChild;
        free(root);
        opTreeFree(lChild);
        opTreeFree(rChild);
    }
}