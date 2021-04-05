//
// Created by 田沛林 on 2020/12/3.
//

#ifndef PLT_PROJET_STACK_H
#define PLT_PROJET_STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct Stack {
    int maxLen;
    int len;
    int elemSize;
    void *stackArr;
} *pSTACK;

pSTACK stackInit(int elemSize); //pSTACK pStack,
void stackGrow(pSTACK pStack);
void stackClear(pSTACK pStack);
void stackFree(pSTACK pStack);

void stackPush(pSTACK pStack, void *pElem);
int stackPop(pSTACK pStack, void *pElem);
int stackTop(pSTACK pStack, void *pElem);


#endif //PLT_PROJET_STACK_H
