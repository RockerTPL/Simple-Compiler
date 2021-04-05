//
// Created by 田沛林 on 2020/12/20.
//

#ifndef PLT_PROJET_TABLE_H
#define PLT_PROJET_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tools/Stack.h"
#include "../2_syn/calculate.h"

#define MAX_TABLE_LEN 1000
#define ERROR_DATA 6
#define ERROR_FUNC 7
#define ERROR_EXE 8

typedef void (*baseFunc)(void);

typedef struct LAC {
    int tableLen;
    int funcNum;
    int lastFuncIdx;
    int lacArr[MAX_TABLE_LEN];
} *pLAC;

typedef struct VM {
    int tableLen;
    int funcNum;
    int vmArr[MAX_TABLE_LEN];
} *pVM;

typedef struct PROC {
    int tableLen;
    baseFunc procArr[MAX_TABLE_LEN];
} *pPROC;

typedef struct STRING {
    int tableLen;
    int strArr[MAX_TABLE_LEN];
} *pSTRING;

/// base functions
void plus();         // "+"
void minus();        // "-"
void multiply();     // "*"
void point();        // "."
void fin();          // "(fin)"
void lit();          // "(lit)"
void ifLac();        // "if"
void elseLac();      // "else"
void equal();        // "="
void dupLac();       // "dup"
void dropLac();      // "drop"
void swapLac();      // "swap"
void countLac();     // "count"
void typeLac();      // "type"
void calcLac();      // "calculate"

void stacksInit();
void stacksClear();
void stacksFree();

void addFuncToLAC(char *baseFuncName);
void addBaseFuncToVM();
void declareBaseFunc(char *baseFuncName);
void tablesBuild();
void tablesFree();

int findFuncCfa(const char *funcName);
void procBaseFunc(const char *funcName);

#endif //PLT_PROJET_TABLE_H
