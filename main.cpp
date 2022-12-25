#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <windows.h>
#include <psapi.h>
#include <typeinfo>
#include <vector>
#include "LimitSingleInstance.H"
#include <sstream>
#include <time.h>
#include <fstream>
#include "json.h"
#include <cstdio>
#include <cstring>
#include <string>
#include "rapidxml.hpp"
#include <curl/curl.h>
#include <tchar.h>

#define JSON_DLL JSON_API
#define INFO_BUFFER_SIZE 32767

using namespace std;
using namespace rapidxml;

void printError(TCHAR *msg);

bool is_file_exist(const char *fileName);

int CurlSender(string jsonData);

void handlerProcessAll(int typeOfSoft, int teminate, string pathFolder);

BOOL SelfDelete();

/* create one instance */
TCHAR strMutexNameIns[] = TEXT("Global\\{9DA0BEED-7248-450a-B27C-C0409BDC377D}");
CLimitSingleInstance g_SingleInstanceObj(strMutexNameIns);

int main(int argc, char *argv[]) {
    if (g_SingleInstanceObj.IsAnotherInstanceRunning())
        return 0;

//    HWND window;
//    AllocConsole();
//    window = FindWindowA("ConsoleWindowClass", NULL);
//    ShowWindow(window, 0);
    char drive[MAX_PATH];
    TCHAR infoBuf[INFO_BUFFER_SIZE];
    // Get and display the system directory.
    if (!GetSystemDirectory(infoBuf, INFO_BUFFER_SIZE)) {
        TCHAR directoryFold[] = TEXT("GetSystemDirectory");
        printError(directoryFold);
    }
    cout << "GetSystemDirectory " << infoBuf << endl;
    _splitpath(infoBuf, drive, NULL, NULL, NULL);
    string dirPath = drive;
    cout << "Path: " << dirPath + "\\" << endl;
    while (true) {
        cout << "Start " << endl;

        Json::Value data;
        //json
        string pathDirFolderJSON = dirPath + "\\nwBizibox";
        string fullPath = dirPath + "\\nwBizibox\\spider_config.json";
        char *writable = new char[fullPath.size() + 1];
        copy(fullPath.begin(), fullPath.end(), writable);
        writable[fullPath.size()] = '\0'; // don't forget the terminating 0

        bool isJsonExist = is_file_exist(writable);
        cout << "isJsonExist " << isJsonExist << endl;

        if (isJsonExist == 1) {
            ifstream ifs(writable);
            Json::Reader reader;
            Json::Value obj;
            reader.parse(ifs, obj); // reader can also read strings
            cout << "spider_id: " << obj << endl;
            data["spider_id"] = obj["spiderId"].asString();
        } else {
            cout << "spider_id: null" << endl;
            data["spider_id"] = "null";
        }

        cout << "Json Values: " << data << endl;

        //xml
        string pathDirFolderEX = dirPath + "\\bizibox\\Agent\\BiziBox.Exporter";
        fullPath = dirPath + "\\bizibox\\Agent\\BiziBox.Exporter\\BiziBoxSettings.xml";
        writable = new char[fullPath.size() + 1];
        copy(fullPath.begin(), fullPath.end(), writable);
        writable[fullPath.size()] = '\0'; // don't forget the terminating 0

        bool isXMLExist = is_file_exist(writable);
        cout << "isXMLExist: " << isXMLExist << endl;
        if (isXMLExist == 1) {
            xml_document<> doc;
            xml_node<> *root_node;
            // Read the xml file into a vector
            ifstream theFile(writable);
            vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
            buffer.push_back('\0');
            // Parse the buffer using the xml file parsing library into doc
            doc.parse<0>(&buffer[0]);
            root_node = doc.first_node("Bizi");
            xml_node<> *brewery_node = root_node->first_node("ExporterID");
           // std::cout << brewery_node->value() << std::endl;
            data["exporter_id"] = brewery_node->value();
        } else {
            data["exporter_id"] = "null";
        }
        // don't forget to free the string after finished using it
        delete[] writable;
        cout << "XML Values: " << data << endl;

        Json::FastWriter fastWriter;
        string jsonData = fastWriter.write(data);
        cout << "jsonDataToSendPost: " << jsonData << endl;

        int response = CurlSender(jsonData);
        if (response == 11) {
            cout << "responseServer err 11: " << response << endl;
        } else if (response == 22) {
            cout << "responseServer err 22: " << response << endl;
        } else if (response == 33) {
            cout << "responseServer err 33: " << response << endl;
        } else {
            cout << "responseServer: " << response << endl;
            int killAndDeletedJSON = 0;
            int killAndDeletedXML = 0;
            bool delItSelf = false;
            if (response == 0) {
                killAndDeletedJSON = 0;
                killAndDeletedXML = 0;
            } else if (response == 1) {
                killAndDeletedJSON = 1;
                killAndDeletedXML = 0;
            } else if (response == 2) {
                killAndDeletedJSON = 0;
                killAndDeletedXML = 1;
            } else if (response == 3) {
                killAndDeletedJSON = 1;
                killAndDeletedXML = 1;
            } else if (response == 4) {
                killAndDeletedJSON = 1;
                killAndDeletedXML = 1;
                delItSelf = true;
            }
            cout << "killAndDeletedJSON: " << killAndDeletedJSON << endl;
            cout << "killAndDeletedXML: " << killAndDeletedXML << endl;
            cout << "delItSelf: " << delItSelf << endl;

            if (isJsonExist == 1) {
                cout << "isJsonExist: " << isJsonExist << endl;
                string pathEXEFull = pathDirFolderJSON + "\\nwBizibox.exe";
                char *writableChar = new char[pathEXEFull.size() + 1];
                copy(pathEXEFull.begin(), pathEXEFull.end(), writableChar);
                writableChar[pathEXEFull.size()] = '\0'; // don't forget the terminating 0
                bool isEXEExist = is_file_exist(writableChar);
                delete[] writableChar;
                cout << "isnwBiziboxExist: " << isEXEExist << endl;
                if (isEXEExist == 1 || killAndDeletedJSON == 1) {
                    handlerProcessAll(1, killAndDeletedJSON, pathDirFolderJSON);
                }
            }
            if (isXMLExist == 1) {
                cout << "isXMLExist: " << isXMLExist << endl;
                string pathEXEFull = pathDirFolderEX + "\\BiziBox.Exporter";
                char *writableChar = new char[pathEXEFull.size() + 1];
                copy(pathEXEFull.begin(), pathEXEFull.end(), writableChar);
                writableChar[pathEXEFull.size()] = '\0'; // don't forget the terminating 0
                bool isEXEExist = is_file_exist(writableChar);
                delete[] writableChar;
                cout << "isExporterExist: " << isEXEExist << endl;
                if (isEXEExist == 1 || killAndDeletedXML == 1) {
                    handlerProcessAll(2, killAndDeletedXML, pathDirFolderEX);
                }
            }
            if (delItSelf) {
                cout << "Start delItSelf now: " << delItSelf << endl;
                SelfDelete();
                cout << "End Program " << endl;
                break;
            }
        }
        cout << "Sleep 30000 " << endl;
        Sleep(30000);
    }
    return 0;
}

bool is_file_exist(const char *fileName) {
    ifstream infile(fileName);
    return infile.good();
}

void printError(TCHAR *msg) {
    DWORD eNum;
    TCHAR sysMsg[256];
    TCHAR *p;

    eNum = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, eNum,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  sysMsg, 256, NULL);

    // Trim the end of the line and terminate it with a null
    p = sysMsg;
    while ((*p > 31) || (*p == 9))
        ++p;
    do { *p-- = 0; }
    while ((p >= sysMsg) &&
           ((*p == '.') || (*p < 33)));

    // Display the message
    _tprintf(TEXT("\n\t%s failed with error %d (%s)"), msg, eNum, sysMsg);
}
