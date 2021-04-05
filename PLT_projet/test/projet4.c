//
// Created by 田沛林 on 2020/12/22.
//

#include "../4_execute/execute.h"

pLAC tableLAC;
pVM tableVM;
pPROC tablePROC;
pSTRING tableSTRING;
pSTACK pDataStack;
pSTACK pReturnStack;

int main() {

    /// execute
    char *fileName = "test_exe.lac"; // "factorielle.lac"; //
    executeFileLAC(fileName);

    return 0;
}