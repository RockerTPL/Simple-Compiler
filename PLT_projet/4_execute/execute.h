//
// Created by 田沛林 on 2020/12/22.
//

#ifndef PLT_PROJET_EXECUTE_H
#define PLT_PROJET_EXECUTE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../1_lex/analex.h"
#include "../3_table/table.h"
#include "../tools/Stack.h"

int isLacFile(char *fileName);

int executeVM(int cfa);

int checkExpr(pLEX tableLEX);

long addToVM(pLEX tableLEX, long i);

long checkCond(pLEX tableLEX, long ifIdx);
long addCondToVM(pLEX tableLEX, long ifIdx);

long checkDef(pLEX tableLEX, long wIdx);
void addLacFuncToVM(pLEX tableLEX, long wIdx, long idxFin);
long addLacFunc(pLEX tableLEX, long wIdx);

long executeWord(pLEX tableLEX, long wIdx);
int executeLEX(pLEX tableLEX);
void executeFileLAC(char *fileName);


#endif //PLT_PROJET_EXECUTE_H
