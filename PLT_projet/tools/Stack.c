//
// Created by 田沛林 on 2020/12/3.
//

#include "Stack.h"

pSTACK stackInit(int elemSize) {
    pSTACK pStack = (pSTACK)malloc(sizeof(struct Stack));
    pStack->maxLen = 10;
    pStack->len = 0;
    pStack->elemSize = elemSize;
    pStack->stackArr = malloc(pStack->maxLen * elemSize);
    return pStack;
}

void stackGrow(pSTACK pStack) {
    pStack->maxLen *= 2;
    pStack->stackArr = realloc(pStack->stackArr, pStack->maxLen * pStack->elemSize);
}

void stackClear(pSTACK pStack) {
    pStack->maxLen = 10;
    pStack->len = 0;
    if (pStack->stackArr != NULL) {
        free(pStack->stackArr);
        pStack->stackArr = malloc(pStack->maxLen * pStack->elemSize);
    }
}

void stackFree(pSTACK pStack) {
    free(pStack->stackArr);
    free(pStack);
}

void stackPush(pSTACK pStack, void *pElem) {
    if (pStack->len == pStack->maxLen) {
        stackGrow(pStack);
    }
    void *target = (char *)(pStack->stackArr) + pStack->len * pStack->elemSize;
    memcpy(target, pElem, pStack->elemSize);
    pStack->len += 1;
}

int stackPop(pSTACK pStack, void *pElem) {
    if (pStack->len > 0) {
        void *source = (char *) (pStack->stackArr) + (pStack->len - 1) * pStack->elemSize;
        memcpy(pElem, source, pStack->elemSize);
        pStack->len -= 1;
        return 1;
    }
    return 0;
}

int stackTop(pSTACK pStack, void *pElem) {
    if (pStack->len > 0) {
        void *source = (char *) (pStack->stackArr) + (pStack->len - 1) * pStack->elemSize;
        memcpy(pElem, source, pStack->elemSize);
        return 1;
    }
    return 0;
}
