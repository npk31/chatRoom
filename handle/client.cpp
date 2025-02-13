#define NOMINMAX
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <windows.h>
#include <string>
#include <filesystem>

using namespace std;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

string wstringToString(const wstring& wstr) {
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    string str(bufferSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], bufferSize, NULL, NULL);
    return str;
}

int main() {
    filesystem::path file_path = filesystem::absolute(__FILE__);
    filesystem::path __DIR__ = file_path.parent_path();
    filesystem::path I_UI = __DIR__ / "interactionUI.ps1";
    filesystem::path CVS_EXE = __DIR__ / "dist" / "cvs.exe";
    filesystem::path SM_EXE = __DIR__ / "dist" / "sm.exe";

    wstring roomId;
    string username;
    char choice;

    cout << "Enter Username: ";
    cin >> username;

    if (username.empty()) {
        cerr << "Username cannot be empty!" << endl;
        return 1;
    } else if (username.length() > 75) {
        cerr << "Username is too long!" << endl;
        return 1;
    }

    cout << "Enter Room ID: ";
    wcin >> roomId;

    clearScreen();
    wcout << "Room ID: " << roomId << endl;
    cout << "Username: " << username << endl;

    HANDLE hMapFile = OpenFileMappingW(FILE_MAP_READ, FALSE, roomId.c_str());
    if (hMapFile == NULL) {
        cerr << "This room does not exist! " << endl;
        return 1;
    }

    cout << "====================" << endl;
    cout << "Do you want to join this room? (Y/N): ";
    cin >> choice;

    if (choice == 'Y' || choice == 'y') {
        cout << "Joining room..." << endl;

        string roomIdStr = wstringToString(roomId);
        char command[512];
        snprintf(command, sizeof(command),
        "powershell -ExecutionPolicy Bypass -File \"%s\" -RID %s -UN %s -P1 \"%s\" -P2 \"%s\"",
        I_UI.generic_string().c_str(), roomIdStr.c_str(), username.c_str(),
        CVS_EXE.generic_string().c_str(), SM_EXE.generic_string().c_str());

        int result = system(command);
        if (result != 0) {
            cerr << "Command execution failed with error code: " << result << endl;
            }
    } else {
        cout << "Exiting..." << endl;
    }

    if (hMapFile) CloseHandle(hMapFile);

    return 0;
}