//
// Created by 田沛林 on 2020/12/20.
//

#include "../3_table/table.h"

pLAC tableLAC;
pVM tableVM;
pPROC tablePROC;
pSTACK tableSTRING;
pSTACK pDataStack;
pSTACK pReturnStack;


int main() {
    /// init
    stacksInit();
    tablesBuild();

    int a1 = 2, a2 = 3, a3 = 4, a4 = 5, a5 = 6;
    stackPush(pDataStack, &a1);
    stackPush(pDataStack, &a2);
    stackPush(pDataStack, &a3);
    procBaseFunc("*");
    procBaseFunc("-");
    stackPush(pDataStack, &a4);
    stackPush(pDataStack, &a5);
    procBaseFunc("+");
    procBaseFunc("*");
    procBaseFunc(".");

    /// free
    tablesFree();
    stacksFree();

    return 0;
}