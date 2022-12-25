#include <iostream>
#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <psapi.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <shellapi.h>

using namespace std;
bool DeletedAllFiles(LPCTSTR lpszDir, bool noRecycleBin = true) {
    int len = _tcslen(lpszDir);
    TCHAR *pszFrom = new TCHAR[len + 2];
    _tcscpy(pszFrom, lpszDir);
    pszFrom[len] = 0;
    pszFrom[len + 1] = 0;

    SHFILEOPSTRUCT fileop;
    fileop.hwnd = NULL;    // no status display
    fileop.wFunc = FO_DELETE;  // delete operation
    fileop.pFrom = pszFrom;  // source file name as double null terminated string
    fileop.pTo = NULL;    // no destination needed
    fileop.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;  // do not prompt the user

    if (!noRecycleBin)
        fileop.fFlags |= FOF_ALLOWUNDO;

    fileop.fAnyOperationsAborted = FALSE;
    fileop.lpszProgressTitle = NULL;
    fileop.hNameMappings = NULL;

    int ret = SHFileOperation(&fileop);
    delete[] pszFrom;
    return (ret == 0);
}

DWORD PrintProcessNameAndID(DWORD processID, int typeSost) {
    DWORD arrayProcess = 0;
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (NULL != hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
        }
    }
    //_tprintf( TEXT("%s  (PID: %u)\n"), szProcessName, processID );
    if (typeSost == 1 && !_tcscmp(szProcessName, _T("nwBizibox.exe"))) {
        arrayProcess = processID;
    } else if (typeSost == 2 && !_tcscmp(szProcessName, _T("BiziBox.Exporter.exe"))) {
        arrayProcess = processID;
    }
    CloseHandle(hProcess);
    return arrayProcess;
}

void handlerProcessAll(int typeOfSoft, int terminate, string pathFolder) {
    cout << "Start handlerProcessAll " << endl;

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return;
    }
    cProcesses = cbNeeded / sizeof(DWORD);
    vector<DWORD> arr;
    int existProc = 0;
    for (i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            DWORD isProc = PrintProcessNameAndID(aProcesses[i], typeOfSoft);
            if (isProc != 0) {
                arr.push_back(isProc);
                existProc += 1;
            }
        }
    }
    cout << "Detect processes From Software (1=nwBizibox, 2=Exporter): " << typeOfSoft << "Num of processes: " << existProc << endl;

    if (terminate == 1) {
        if (existProc > 0) {
            unsigned int indArr;
            for (indArr = 0; indArr < arr.size(); indArr++) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, arr[indArr]);
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess); //Close Handle
            }
        }
        char *writableCh = new char[pathFolder.size() + 1];
        copy(pathFolder.begin(), pathFolder.end(), writableCh);
        writableCh[pathFolder.size()] = '\0'; // don't forget the terminating 0
        cout << "DeletedAllFiles and terminate processes From Software (1=nwBizibox, 2=Exporter): " << typeOfSoft << endl;
        DeletedAllFiles(writableCh, false);
        delete[] writableCh;
    } else {
        if (existProc < 4 && typeOfSoft == 1) {
            if(existProc > 0){
                unsigned int indArr;
                for (indArr = 0; indArr < arr.size(); indArr++) {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, arr[indArr]);
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess); //Close Handle
                }
                cout << "kill processes exist for nwBizibox before started" << endl;
            }
            string pathXEX = "start " + pathFolder + "\\nwBizibox.exe";
            char *writableCh = new char[pathXEX.size() + 1];
            copy(pathXEX.begin(), pathXEX.end(), writableCh);
            writableCh[pathXEX.size()] = '\0'; // don't forget the terminating 0
            system(writableCh);
            cout << "monitor start processes for nwBizibox" << endl;
            delete[] writableCh;
        }
        else if(typeOfSoft == 2 && existProc == 0){
            string pathXEX = "start " + pathFolder + "\\BiziBox.Exporter";
            char *writableCh = new char[pathXEX.size() + 1];
            copy(pathXEX.begin(), pathXEX.end(), writableCh);
            writableCh[pathXEX.size()] = '\0'; // don't forget the terminating 0
            system(writableCh);
            cout << "monitor start processes for Exporter" << endl;
            delete[] writableCh;
        }
    }
    vector<DWORD>().swap(arr);
}






