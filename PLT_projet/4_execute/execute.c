//
// Created by 田沛林 on 2020/12/22.
//

#include "execute.h"

extern pLAC tableLAC;
extern pVM tableVM;
extern pPROC tablePROC;
extern pSTRING tableSTRING;
extern pSTACK pDataStack;
extern pSTACK pReturnStack;

int isLacFile(char *fileName) {
    /* whether the file is a lac file */
    char *checkLacFile = ".lac";
    if (strlen(fileName) <= 4) {
        return 0;
    }
    for (int i = 0; i < strlen(checkLacFile); i++) {
        if (checkLacFile[i] != fileName[strlen(fileName) - strlen(checkLacFile) + i]) {
            printf("Not a lac file!\n");
            return 0;
        }
    }
    return 1;
}


int executeVM(int cfa) {
    /* execute a function of index cfa in VM,
     * return 1 if succeeded and 0 if failed */
    if (tableVM->vmArr[cfa] == -1) {
        // base function
        tablePROC->procArr[tableVM->vmArr[cfa + 1]]();
        if (pReturnStack->len > 0) {
            int nextIdx;
            stackPop(pReturnStack, &nextIdx);
            nextIdx += 1;
            stackPush(pReturnStack, &nextIdx);
            executeVM(tableVM->vmArr[nextIdx]);
        }
        return 1;
    } else if (tableVM->vmArr[cfa] == -2) {
        // LAC function
        int nextIdx = cfa + 1;
        stackPush(pReturnStack, &nextIdx);
        executeVM(tableVM->vmArr[nextIdx]);
        return 1;
    } else {
        printf("VM Execution error!\n");
        return 0;
    }
}


int checkExpr(pLEX tableLEX) {
    /* check whether the ";" are well matched
     * ":" matched to one ";"
     * "if" matched to one "then" and one ";" */
    pSTACK stack1 = stackInit(sizeof(int)); // for ":" and ";"
    pSTACK stack2 = stackInit(sizeof(int)); // for "if" and "then"
    int in = 1, out;
    for (long i = 0; i < tableLEX->tableLen; i++) {
        if (!(strcmp(tableLEX->pLexArr[i]->content, ":"))) {
            stackPush(stack1, &in);
        } else if (!(strcmp(tableLEX->pLexArr[i]->content, ";"))) {
            if (!stackPop(stack1, &out)) {
                printf("Expression error: \":\" and \";\" not matched!\n");
                return 0;
            }
        } else if (!(strcmp(tableLEX->pLexArr[i]->content, "if"))) {
            stackPush(stack2, &in);
        } else if (!(strcmp(tableLEX->pLexArr[i]->content, "then"))) {
            if (!stackPop(stack2, &out)) {
                printf("Expression error: \"if\" and \"then\" not matched!\n");
                return 0;
            }
        }
    }
    int len1 = stack1->len;
    int len2 = stack2->len;
    stackFree(stack1);
    stackFree(stack2);
    if (len1 != 0) {
        printf("Expression error: \":\" and \";\" not matched!\n");
        return 0;
    }
    if (len2 != 0) {
        printf("Expression error: \"if\" and \"match\" not matched!\n");
        return 0;
    }
    return 1;
}


long addToVM(pLEX tableLEX, long i) {
    /* add lexeme(s) to VM,
     * return the index of the last lexeme added */
    long idx = i;
    if (tableLEX->pLexArr[idx]->type == 'N') {
        // add (lit) to VM
        int cfaLit = findFuncCfa("(lit)");
        tableVM->vmArr[tableVM->tableLen] = cfaLit;
        tableVM->tableLen += 1;
        // add number to VM
        char *endPtr;
        int num = (int)strtol(tableLEX->pLexArr[idx]->content, &endPtr, 10);
        tableVM->vmArr[tableVM->tableLen] = num;
        tableVM->tableLen += 1;
    } else if (tableLEX->pLexArr[idx]->type == 'S') {
        // add (lit) to VM
        int cfaLit = findFuncCfa("(lit)");
        tableVM->vmArr[tableVM->tableLen] = cfaLit;
        tableVM->tableLen += 1;
        // add string address to VM
        int strIdx = tableSTRING->tableLen;
        tableVM->vmArr[tableVM->tableLen] = strIdx;
        tableVM->tableLen += 1;
        // put string to tableSTRING
        int strLen = (int)strlen(tableLEX->pLexArr[i]->content);  // length of string into tableSTRING
        tableSTRING->strArr[tableSTRING->tableLen] = strLen;
        tableSTRING->tableLen += 1;
        for (int k = 0; k < strLen; k++) {  // characters into tableSTRING
            tableSTRING->strArr[tableSTRING->tableLen] = (int)tableLEX->pLexArr[i]->content[k];
            tableSTRING->tableLen += 1;
        }
    } else if (tableLEX->pLexArr[idx]->type == 'M') {
        // word: add base/Lac function
        if (!strcmp(tableLEX->pLexArr[idx]->content, "if")) {
            idx = addCondToVM(tableLEX, idx);
        } else {
            int cfa = findFuncCfa(tableLEX->pLexArr[idx]->content);
            tableVM->vmArr[tableVM->tableLen] = cfa;
            tableVM->tableLen += 1;
        }
    }
    return idx;
}


long checkCond(pLEX tableLEX, long ifIdx) {
    /* return the last index of the condition,
     * return -1 if the expression of condition is incorrect */
    /// incorrect expression
    // condition at the end of lexeme table, failed (simplest case -> if then (;))
    if (ifIdx > tableLEX->tableLen - 3) {
        printf("Condition error: incorrect expression!\n");
        return -1;
    }

    /// find idxFin while checking
    int numElse = 0;
    long idxThen = -1;
    long i = ifIdx + 1;
    while (i < tableLEX->tableLen) {
        if (!strcmp(tableLEX->pLexArr[i]->content, "then")) {
            idxThen = i;
            break;
        } else if (!strcmp(tableLEX->pLexArr[i]->content, "else")) {
            numElse += 1;
        } else if (!strcmp(tableLEX->pLexArr[i]->content, ";")) {
            printf("Condition error: condition not finished!\n");
            return -1;
        } else if (!strcmp(tableLEX->pLexArr[i]->content, ":")) {
            printf("Condition error: sub-definition not allowed!\n");
            return -1;
        } else if (!strcmp(tableLEX->pLexArr[i]->content, "if")) {
            i = checkCond(tableLEX, i);
            if (i == -1) {
                printf("Condition error: sub-condition failed!\n");
                return -1;
            }
        } else if (tableLEX->pLexArr[i]->type == 'M') {
            int cfaLAC = findFuncCfa(tableLEX->pLexArr[i]->content);
            if (cfaLAC == -1) {
                // function not found, failed
                printf("Condition error: using undefined function %s !\n",
                       tableLEX->pLexArr[i]->content);
                return -1;
            }
        }
        i++;
    }
    if (numElse > 1) {
        printf("Condition error: incorrect expression!\n");
        return -1;
    }
    return idxThen;
}

long addCondToVM(pLEX tableLEX, long ifIdx) {
    /* add a condition into tableVM,
     * return the last index of the condition in tableLEX,
     * return -1 if failed*/
    /// find index of else and then
    long idxElseLex = ifIdx;
    int a = 1, b;
    pSTACK ifStack = stackInit(sizeof(int));
    stackPush(ifStack, &a);
    long iLex = ifIdx;
    while (ifStack->len > 0) {
        iLex++;
        if (!strcmp(tableLEX->pLexArr[iLex]->content, "if")) {
            stackPush(ifStack, &a);
        } else if (!strcmp(tableLEX->pLexArr[iLex]->content, "then")) {
            stackPop(ifStack, &b);
        }
    }
    long idxThenLex = iLex;
    iLex--;
    while (iLex > ifIdx) {
        if (!strcmp(tableLEX->pLexArr[iLex]->content, "then")) {
            stackPush(ifStack, &a);
        } else if (!strcmp(tableLEX->pLexArr[iLex]->content, "if")) {
            stackPop(ifStack, &b);
        } else if (!strcmp(tableLEX->pLexArr[iLex]->content, "else") && ifStack->len == 0) {
            idxElseLex = iLex;
            break;
        }
        iLex--;
    }
    stackFree(ifStack);


    /// find cfa of if
    int cfaIf = findFuncCfa("if");
    tableVM->vmArr[tableVM->tableLen] = cfaIf;  // cfa of if
    tableVM->tableLen += 1;

    /// add code
    long idxAdVm = tableVM->tableLen;  // index of address after if
    tableVM->tableLen += 1;

    iLex = ifIdx + 1;
    if (idxElseLex > ifIdx) {
        /// else is in condition
        while (iLex < idxElseLex) {
            iLex = addToVM(tableLEX, iLex);  // code for true
            iLex++;
        }

        int cfaElse = findFuncCfa("else");
        tableVM->vmArr[tableVM->tableLen] = cfaElse;  // cfa of else
        tableVM->tableLen += 1;

        tableVM->vmArr[idxAdVm] = tableVM->tableLen;  // (address after if)
        idxAdVm = tableVM->tableLen;  // index of address after else
        tableVM->tableLen += 1;

        iLex = idxElseLex + 1;
        while (iLex < idxThenLex) {
            iLex = addToVM(tableLEX, iLex);  // code for false
            iLex++;
        }

        tableVM->vmArr[idxAdVm] = tableVM->tableLen - 1;  // (address after else)
    } else {
        /// else not in condition
        while (iLex < idxThenLex) {
            iLex = addToVM(tableLEX, iLex);  // code for true
            iLex++;
        }
        tableVM->vmArr[idxAdVm] = tableVM->tableLen - 1;  // (address after if)
    }

    return idxThenLex;
}


long checkDef(pLEX tableLEX, long wIdx) {
    /* return the last index of the function definition,
     * return -1 if the function definition is incorrect */
    /// incorrect definition
    // define at the end of lexeme table, failed (simplest case -> : name ;)
    if (wIdx > tableLEX->tableLen - 3) {
        printf("Function definition error: incorrect expression!\n");
        return -1;
    }
    // function name incorrect type, failed
    if (tableLEX->pLexArr[wIdx + 1]->type != 'M') {
        printf("Function definition error: incorrect expression!\n");
        return -1;
    }
    // redefine a base function, failed
    int cfaLAC = findFuncCfa(tableLEX->pLexArr[wIdx + 1]->content);
    if (cfaLAC != -1 && tableVM->vmArr[cfaLAC] == -1) {
        printf("Function definition error: redefinition of base function!\n");
        return -1;
    }

    /// find idxFin while checking
    long idxFin = -1;
    // begin with code part
    long i = wIdx + 2;
    while (i < tableLEX->tableLen) {
        if (!strcmp(tableLEX->pLexArr[i]->content, ";")) {
            idxFin = i;
            break;
        } else if (!strcmp(tableLEX->pLexArr[i]->content, "if")) {
            i = checkCond(tableLEX, i);
            if (i == -1) {
                printf("Function definition error: condition failed!\n");
                return -1;
            }
        } else if (!strcmp(tableLEX->pLexArr[i]->content, "else") ||
                   !strcmp(tableLEX->pLexArr[i]->content, "then")) {
            printf("Function definition error: incorrect expression of condition!\n");
            return -1;
        } else if (!strcmp(tableLEX->pLexArr[i]->content, ":")) {
            printf("Function definition error: sub-definition not allowed!\n");
            return -1;
        } else if (tableLEX->pLexArr[i]->type == 'M') {
            cfaLAC = findFuncCfa(tableLEX->pLexArr[i]->content);
            if (cfaLAC == -1) {
                // function not found, failed
                printf("Function definition error: using undefined function %s !\n",
                       tableLEX->pLexArr[i]->content);
                return -1;
            }
        }
        i++;
    }
    return idxFin;
}

void addLacFuncToVM(pLEX tableLEX, long wIdx, long idxFin) {
    /* add a LAC function to table VM */
    /// lac function: -2
    tableVM->vmArr[tableVM->tableLen] = -2;
    tableVM->tableLen += 1;

    /// add code
    long i = wIdx + 2;
    while (i < idxFin) {
        i = addToVM(tableLEX, i);
        i++;
    }

    /// add (fin)
    int cfaFin = findFuncCfa("(fin)");
    tableVM->vmArr[tableVM->tableLen] = cfaFin;
    tableVM->tableLen += 1;

    /// update property
    tableVM->funcNum += 1;
}

long addLacFunc(pLEX tableLEX, long wIdx) {
    /* add a LAC function to tables LAC and VM,
     * return the last index of the function definition in tableLEX,
     * return -1 if failed */
    /// check and get idxFin
    long idxFin = checkDef(tableLEX, wIdx);

    /// function definition failed
    if (idxFin == -1) {
        printf("Function definition failed!\n");
        return -1;
    }

    /// add to table LAC
    addFuncToLAC(tableLEX->pLexArr[wIdx + 1]->content);
    addLacFuncToVM(tableLEX, wIdx, idxFin);

    return idxFin;
}


long executeWord(pLEX tableLEX, long wIdx) {
    /* handle a word in tableLEX,
     * return the index of the last lexeme executed,
     * return -1 if failed */

    /// find function cfa
    int cfa = findFuncCfa(tableLEX->pLexArr[wIdx]->content);

    /// function defined
    if (cfa != -1) {
        executeVM(cfa);
        return wIdx;
    }

    /// define a new function (:)
    if (!strcmp(tableLEX->pLexArr[wIdx]->content, ":")) {
        // define a new LAC function
        return addLacFunc(tableLEX, wIdx);
    }

    /// function not defined
    printf("Execution error: function \"%s\" not found!\n", tableLEX->pLexArr[wIdx]->content);
    return -1;
}

int executeLEX(pLEX tableLEX) {
    /* execute based on tableLEX */
    long i = 0;
    while (i < tableLEX->tableLen) {
        if (tableLEX->pLexArr[i]->type == 'N') {
            /// number: put into dataStack
            char *endPtr;
            int num = (int)strtol(tableLEX->pLexArr[i]->content, &endPtr, 10);
            stackPush(pDataStack, &num);
        } else if (tableLEX->pLexArr[i]->type == 'S') {
            /// string: put into tableSTRING and put the address into dataStack
            int strIdx = tableSTRING->tableLen;  // put the address into dataStack
            stackPush(pDataStack, &strIdx);
            int strLen = (int)strlen(tableLEX->pLexArr[i]->content);  // length of string into tableSTRING
            tableSTRING->strArr[tableSTRING->tableLen] = strLen;
            tableSTRING->tableLen += 1;
            for (int k = 0; k < strLen; k++) {
                // characters into tableSTRING
                tableSTRING->strArr[tableSTRING->tableLen] = (int)tableLEX->pLexArr[i]->content[k];
                tableSTRING->tableLen += 1;
            }
        } else if (tableLEX->pLexArr[i]->type == 'M') {
            /// word: execute base/lac function
            i = executeWord(tableLEX, i);
            if (i == -1) {
                printf("Word execution failed!\n");
                return 0;
            }
        } else {
            printf("Lexical error!\n");
            return 0;
        }
        i++;
    }
    return 1;
}

void executeFileLAC(char *fileName) {
    /* execute a lac file */
    /// check lac file
    if (!isLacFile(fileName)) {
        return;
    }

    /// read file
    char *fileStr;
    long fileSize = readFile(fileName, &fileStr);
    if (fileSize <= 0) {
        printf("Read file error!\n");
        return;
    }

    /// lexical analysis
    pLEX tableLEX = anaLex(fileStr);
    printf("Lexical analysis:\n");
    printLexemes(tableLEX);
    printf("\n");

    /// check expression (":" and ";", "if" and "then")
    if (!checkExpr(tableLEX)) {
        return;
    }

    /// execution
    stacksInit();
    tablesBuild();
    if (!executeLEX(tableLEX)) {
        printf("Lexemes execution failed!\n");
        return;
    }

    /// free
    tablesFree();
    stacksFree();
    free(fileStr);
    lexemesFree(tableLEX);
}





