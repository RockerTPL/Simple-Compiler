//
// Created by 田沛林 on 2020/12/20.
//

#include "table.h"

extern pLAC tableLAC;
extern pVM tableVM;
extern pSTRING tableSTRING;
extern pPROC tablePROC;
extern pSTACK pDataStack;
extern pSTACK pReturnStack;

void plus() {
    /* pop and sum two elements at the top of dataStack, result in-stack */
    int a, b;
    if (!(stackPop(pDataStack, &b) && stackPop(pDataStack, &a))) {
        printf("Data stack error in + !\n");
        exit(ERROR_DATA);
    }
    int res = a + b;
    stackPush(pDataStack, &res);
}

void minus() {
    /* pop and minus two elements at the top of dataStack, result in-stack,
     * or return the opposite of the only element in dataStack */
    int a, b;
    if (!stackPop(pDataStack, &b)) {
        printf("Data stack error in - !\n");
        exit(ERROR_DATA);
    } else if (!stackPop(pDataStack, &a)) {
        int res = -b;
        stackPush(pDataStack, &res);
    } else {
        int res = a - b;
        stackPush(pDataStack, &res);
    }
}

void multiply() {
    /* pop and multiply two elements at the top of dataStack, result in-stack */
    int a, b;
    if (!(stackPop(pDataStack, &b) && stackPop(pDataStack, &a))) {
        printf("Data stack error in * !\n");
        exit(ERROR_DATA);
    }
    int res = a * b;
    stackPush(pDataStack, &res);

}

void point() {
    /* pop and print the element at the top of dataStack */
    int a;
    if (!stackPop(pDataStack, &a)) {
        printf("Data stack error in . !\n");
        exit(ERROR_DATA);
    }
    printf("%d\n", a);
}

void fin() {
    /* signify the end of code (not usable on interpreter mode) */
    int a;
    if (!stackPop(pReturnStack, &a)) {
        printf("Return stack error in (fin) !\n");
        stacksClear();
        exit(ERROR_EXE);
    }
}

void lit() {
    /* read a number in the definition of a LAC function (not usable on interpreter mode) */
    int ad;
    if (!stackPop(pReturnStack, &ad)) {
        printf("Return stack error in (lit) !\n");
        stacksClear();
        exit(ERROR_EXE);
    }
    ad += 1;
    stackPush(pReturnStack, &ad);
    int num = tableVM->vmArr[ad];
    stackPush(pDataStack, &num);
}

void ifLac() {
    /* handle if in Lac */
    int cond, ad;
    if (!stackPop(pDataStack, &cond)) {
        printf("Data stack error in if !\n");
        exit(ERROR_DATA);
    }
    if (!stackPop(pReturnStack, &ad)) {
        printf("Return stack error in if !\n");
        stacksClear();
        exit(ERROR_EXE);
    }
    if (cond) {
        ad += 1;
        stackPush(pReturnStack, &ad);
    } else {
        ad = tableVM->vmArr[ad+1];
        stackPush(pReturnStack, &ad);
    }
}

void elseLac() {
    /* handle else in Lac */
    int ad;
    if (!stackPop(pReturnStack, &ad)) {
        printf("Return stack error in else !\n");
        stacksClear();
        exit(ERROR_EXE);
    }
    ad = tableVM->vmArr[ad+1];
    stackPush(pReturnStack, &ad);
}

void equal() {
    /* whether the two elements at the top of dataStack is equal */
    int e1, e2;
    if (!(stackPop(pDataStack, &e1) && stackPop(pDataStack, &e2))) {
        printf("Data stack error in = !\n");
        exit(ERROR_DATA);
    }
    int res = (e1 == e2);
    stackPush(pDataStack, &res);
}

void dupLac() {
    /* duplicate the element at the top of dataStack */
    int top;
    if (!stackTop(pDataStack, &top)) {
        printf("Data stack error in dup !\n");
        exit(ERROR_DATA);
    }
    stackPush(pDataStack, &top);
}

void dropLac() {
    /* remove the element at the top of dataStack */
    int a;
    if (!stackPop(pDataStack, &a)) {
        printf("Data stack error in drop !\n");
        exit(ERROR_DATA);
    }
}

void swapLac() {
    /* swap the two elements at the top of dataStack */
    int e1, e2;
    if (!(stackPop(pDataStack, &e1) && stackPop(pDataStack, &e2))) {
        printf("Data stack error in swap !\n");
        exit(ERROR_DATA);
    }
    stackPush(pDataStack, &e1);
    stackPush(pDataStack, &e2);
}

void countLac() {
    /* take the string address (index) at the top of dataStack,
     * replace it by the beginning index of the string (without length of string),
     * add the length of string into dataStack */
    int idx;
    if (!stackPop(pDataStack, &idx)) {
        printf("Data stack error in count !\n");
        exit(ERROR_DATA);
    }
    int strLen = tableSTRING->strArr[idx];
    idx += 1;
    stackPush(pDataStack, &idx);
    stackPush(pDataStack, &strLen);
}

void typeLac() {
    /* take the length of string at the top of dataStack;
     * take the beginning index of string at the top of dataStack,
     * print the string in tableSTRING at the beginning index (taken) of length (taken) */
    int strLen, idx;
    if (!(stackPop(pDataStack, &strLen) && stackPop(pDataStack, &idx))) {
        printf("Data stack error in type !\n");
        exit(ERROR_DATA);
    }
    for (int i = 0; i < strLen; i++) {
        printf("%c", (char)tableSTRING->strArr[idx+i]);
    }
}

void calcLac() {
    /* take the string address (index) at the top of dataStack,
     * calculate the result of this string (considered as math expression),
     * put the result into dataStack */
    int idx;
    if (!stackPop(pDataStack, &idx)) {
        printf("Data stack error in calculate !\n");
        exit(ERROR_DATA);
    }
    int strLen = tableSTRING->strArr[idx];
    char *expr;
    expr = (char *)calloc(strLen + 1, sizeof(char));
    for (int i = 0; i < strLen; i++) {
        expr[i] = (char)tableSTRING->strArr[idx + 1 + i];
    }
    int res = calculateC(expr);
    stackPush(pDataStack, &res);
    free(expr);
}

void stacksInit() {
    /* initiate dataStack & returnStack */
    pDataStack = stackInit(sizeof(int));
    pReturnStack = stackInit(sizeof(int));
}

void stacksClear() {
    /* clear 2 stacks */
    stackClear(pDataStack);
    stackClear(pReturnStack);
}

void stacksFree() {
    /* free 2 stacks */
    free(pDataStack);
    free(pReturnStack);
}


void addFuncToLAC(char *baseFuncName) {
    /* add a function to table LAC */
    /// last function index
    if (tableLAC->funcNum == 0) {
        tableLAC->lacArr[tableLAC->tableLen] = 0;
    } else {
        tableLAC->lacArr[tableLAC->tableLen] = tableLAC->lastFuncIdx;
    }
    tableLAC->tableLen += 1;

    /// update table properties
    tableLAC->lastFuncIdx = tableLAC->tableLen;
    tableLAC->funcNum += 1;

    /// length of function name
    int funcNameLen = (int)strlen(baseFuncName);
    tableLAC->lacArr[tableLAC->tableLen] = funcNameLen;
    tableLAC->tableLen += 1;

    /// name of function
    for (int i = 0; i < funcNameLen; i++) {
        tableLAC->lacArr[tableLAC->tableLen] = (int)baseFuncName[i];
        tableLAC->tableLen += 1;
    }

    /// cfa of function
    tableLAC->lacArr[tableLAC->tableLen] = tableVM->tableLen;
    tableLAC->tableLen += 1;
}

void addBaseFuncToVM() {
    /* add a base function to table VM */
    tableVM->vmArr[tableVM->tableLen] = -1;
    tableVM->vmArr[tableVM->tableLen+1] = tableVM->funcNum;
    tableVM->tableLen += 2;
    tableVM->funcNum += 1;
}

void declareBaseFunc(char *baseFuncName) {
    /* add a base function to LAC table and VM table */
    addFuncToLAC(baseFuncName);
    addBaseFuncToVM();
}

void tablesBuild() {
    /* build tables LAC, VM and PROC of base functions,
     * also initiate the table STRING */
    /// init tables
    tableLAC = (pLAC)malloc(sizeof(struct LAC));
    tableVM = (pVM)malloc(sizeof(struct VM));
    tablePROC = (pPROC)malloc(sizeof(struct PROC));
    tableSTRING = (pSTRING)malloc(sizeof(struct STRING));

    /// init properties
    tableLAC->tableLen = tableVM->tableLen = tablePROC->tableLen = tableSTRING->tableLen = 0;
    tableLAC->funcNum = tableVM->funcNum = 0;
    tableLAC->lastFuncIdx = 0;

    /// build with base functions
    declareBaseFunc("+");
    tablePROC->procArr[tablePROC->tableLen] = &plus;
    tablePROC->tableLen += 1;
    declareBaseFunc("-");
    tablePROC->procArr[tablePROC->tableLen] = &minus;
    tablePROC->tableLen += 1;
    declareBaseFunc("*");
    tablePROC->procArr[tablePROC->tableLen] = &multiply;
    tablePROC->tableLen += 1;
    declareBaseFunc(".");
    tablePROC->procArr[tablePROC->tableLen] = &point;
    tablePROC->tableLen += 1;
    declareBaseFunc("(lit)");
    tablePROC->procArr[tablePROC->tableLen] = &lit;
    tablePROC->tableLen += 1;
    declareBaseFunc("(fin)");
    tablePROC->procArr[tablePROC->tableLen] = &fin;
    tablePROC->tableLen += 1;
    declareBaseFunc("if");
    tablePROC->procArr[tablePROC->tableLen] = &ifLac;
    tablePROC->tableLen += 1;
    declareBaseFunc("else");
    tablePROC->procArr[tablePROC->tableLen] = &elseLac;
    tablePROC->tableLen += 1;
    declareBaseFunc("=");
    tablePROC->procArr[tablePROC->tableLen] = &equal;
    tablePROC->tableLen += 1;
    declareBaseFunc("dup");
    tablePROC->procArr[tablePROC->tableLen] = &dupLac;
    tablePROC->tableLen += 1;
    declareBaseFunc("drop");
    tablePROC->procArr[tablePROC->tableLen] = &dropLac;
    tablePROC->tableLen += 1;
    declareBaseFunc("swap");
    tablePROC->procArr[tablePROC->tableLen] = &swapLac;
    tablePROC->tableLen += 1;
    declareBaseFunc("count");
    tablePROC->procArr[tablePROC->tableLen] = &countLac;
    tablePROC->tableLen += 1;
    declareBaseFunc("type");
    tablePROC->procArr[tablePROC->tableLen] = &typeLac;
    tablePROC->tableLen += 1;
    declareBaseFunc("calculate");
    tablePROC->procArr[tablePROC->tableLen] = &calcLac;
    tablePROC->tableLen += 1;
}

void tablesFree() {
    /* free 3 tables */
    free(tableLAC);
    free(tableVM);
    free(tablePROC);
    free(tableSTRING);
}


int findFuncCfa(const char *funcName) {
    /* find a function in LAC table,
     * return the cfa of the function (-1 if not found) */
    int pos = tableLAC->lastFuncIdx;
    while (pos > 0) {
        int match = 1;
        if (strlen(funcName) != tableLAC->lacArr[pos]) {
            // function name length not match
            match = 0;
        } else {
            // function name not match
            for (int i = 0; i < tableLAC->lacArr[pos]; i++) {
                if (tableLAC->lacArr[pos+i+1] != funcName[i]) {
                    match = 0;
                    break;
                }
            }
        }
        if (match == 1) {
            return tableLAC->lacArr[pos + tableLAC->lacArr[pos] + 1];
        }
        pos = tableLAC->lacArr[pos-1];
    }
    return -1;
}

void procBaseFunc(const char *funcName) {
    /* process a base function */
    int cfa = findFuncCfa(funcName);
    if (cfa == -1) {
        printf("Function \"%s\" not found!\n", funcName);
        exit(ERROR_FUNC);
    }
    int codeProcFunc =  tableVM->vmArr[cfa+1];
    tablePROC->procArr[codeProcFunc]();
}