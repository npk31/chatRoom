#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <handle.h>

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

void updateMessages(RoomData* pData) {
    clearScreen();
    Sleep(50);

    cout << "\x1b[1;33m================ Chat History ===============\x1b[1;39m" << endl;
    vector<string> messages = splitByNull(pData->buffer, sizeof(pData->buffer));

    for (const string& message : messages) {
        cout << message << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file_mapping_name>" << endl;
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

    wcout << fileMappingName << endl;

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

    updateMessages(pData);

    while (true) {
        if (pData->new_message) {
            updateMessages(pData);
            pData->new_message = false;
            FlushViewOfFile(pData, sizeof(RoomData));
        }
        Sleep(100);
    }

    if (pData) UnmapViewOfFile(pData);
    if (hMapFile) CloseHandle(hMapFile);

    return 0;
}