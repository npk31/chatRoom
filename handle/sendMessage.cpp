#include <iostream>
#include <windows.h>
#include <cstring>
#include <handle.h>
#include <string>

using namespace std;

struct RoomData {
    char buffer[1024] = {};
    bool new_message;
    char sender[128];
    char message[256];
    volatile LONG process_count;
    bool join;
};

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <file_mapping_name> <sender_name>" << endl;
        return 1;
    }

    HANDLE hMapFile;
    RoomData* pData;

    // Chuyển đổi argv[1] từ char* sang LPCWSTR
    int wchars_num = MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, NULL, 0);
    wchar_t* wstr = new wchar_t[wchars_num];
    MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, wstr, wchars_num);
    wstring fileMappingName(wstr);

    hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, fileMappingName.c_str());

    delete[] wstr; // Giải phóng bộ nhớ sau khi sử dụng

    if (hMapFile == NULL) {
        cerr << "Could not open file mapping object (" << GetLastError() << ")" << endl;
        return 1;
    }

    pData = (RoomData*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(RoomData));

    if (pData == NULL) {
        cerr << "Could not map view of file (" << GetLastError() << ").\n";
        CloseHandle(hMapFile);
        return 1;
    }

    InterlockedIncrement(&pData->process_count);
    FlushViewOfFile(pData, sizeof(RoomData));

    char senderName[100] = {0};
    char message[225] = {0};
    
    strcpy_s(senderName, sizeof(senderName), argv[2]);

    cout << "\nName: " << senderName << endl;
    cout << "====================" << endl;

    string nickname;

    if (strlen(senderName) > 10)
        nickname = string(senderName).substr(0, 10) + "...";
    else{
        nickname = string(senderName);
    }

    while (true) {
        cout << "\x1b[1;94m[" + nickname + "]\x1b[1;39m \x1b[1;95m->\x1b[1;39m ";
        cin.getline(message, sizeof(message));

        if (strlen(message) == 0) {
            clearScreen();
            cout << "\x1b[1;91mMessage cannot be empty!\x1b[1;39m" << endl;
            Sleep(1000);
            clearScreen();
            continue;
        } else if (strlen(message) > 200) {
            clearScreen();
            cout << "\x1b[1;91mMessage is too long!\x1b[1;39m" << endl;
            Sleep(1000);
            clearScreen();
            continue;
        }

        if (strlen(message) > 0 && strlen(senderName) > 0) {
            strncpy_s(pData->message, sizeof(pData->message), message, sizeof(pData->message) - 1);
            pData->message[sizeof(pData->message) - 1] = '\0';

            strncpy_s(pData->sender, sizeof(pData->sender), senderName, sizeof(pData->sender) - 1);
            pData->sender[sizeof(pData->sender) - 1] = '\0';

            pData->new_message = true;
            FlushViewOfFile(pData, sizeof(RoomData));

            char combinedMessage[256];
            snprintf(combinedMessage, sizeof(combinedMessage), "\x1b[1;96m%s\x1b[1;39m \x1b[1;92m->\x1b[1;39m %s\n", senderName, message);
            appendString(pData->buffer, sizeof(pData->buffer), combinedMessage);

            clearScreen();
        }
    }

    InterlockedDecrement(&pData->process_count);
    FlushViewOfFile(pData, sizeof(RoomData));

    if (pData) UnmapViewOfFile(pData);
    if (hMapFile) CloseHandle(hMapFile);

    return 0;
}