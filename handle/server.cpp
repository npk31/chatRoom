#define NOMINMAX
#include <iostream>
#include <windows.h>
#include <cstring>
#include <cstdlib>
#include <atomic>
#include <conio.h>


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
        system("cls");  // Windows
    #else
        system("clear"); // Linux/macOS
    #endif
}

int main() {
    HANDLE hMapFile;
    RoomData* pData;
    bool isNewMemory = false;
    wstring roomId;

    clearScreen();
    cout << "[SERVER] Enter Room ID to initialization: ";
    wcin >> roomId;

    // 🟢 Thử mở shared memory đã tồn tại
    hMapFile = OpenFileMappingW(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        roomId.c_str()
    );

    if (hMapFile) {
        cout << "[INFO] Shared Memory exists. Opening..." << endl;
    } else {
        // 🔵 Nếu chưa có, tạo mới shared memory
        hMapFile = CreateFileMappingW(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            sizeof(RoomData),
            roomId.c_str()
        );

        if (hMapFile == NULL) {
            cerr << "[ERROR] Failed to create File Mapping! (" << GetLastError() << ")" << endl;
            return 1;
        }

        isNewMemory = true;
        wcout << "[INFO] Room ID: " << roomId << endl;
        cout << "[SUCCESS] New Shared Memory created!" << endl;
    }

    // 🔥 Ánh xạ shared memory vào không gian tiến trình
    pData = (RoomData*)MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0, 0, sizeof(RoomData)
    );

    if (pData == NULL) {
        cerr << "[ERROR] Failed to map shared memory! (" << GetLastError() << ").\n";
        CloseHandle(hMapFile);
        return 1;
    }

    // 🔵 Nếu đây là shared memory mới, reset process_count
    if (isNewMemory) {
        InterlockedExchange(&pData->process_count, 0);
        pData->join = false;
        FlushViewOfFile(pData, sizeof(RoomData));  // 🔥 Đồng bộ shared memory ngay lập tức
    }

    // 🚀 Tăng số tiến trình tham gia bằng atomic operation
    InterlockedIncrement(&pData->process_count);
    FlushViewOfFile(pData, sizeof(RoomData));

    int lastProcessCount = pData->process_count;
    cout << "[INFO] Current Visit Count: " << lastProcessCount << endl;
    cout << "[SERVER] Shared Memory Initialized. Press 'q' to quit." << endl;

    // 📌 Vòng lặp chính: Không bị chặn bởi cin.getline()
    bool exitFlag = false;

    while (!exitFlag) {
        // 🔥 Kiểm tra nếu số process thay đổi
        if (pData->process_count != lastProcessCount) {
            clearScreen();
            lastProcessCount = pData->process_count;
            wcout << "[SERVER] Room ID: " << roomId << endl;
            cout << "[INFO] Visit Count Updated: " << lastProcessCount << endl;
            cout << "[SERVER] Shared Memory Initialized. Press 'q' to quit." << endl;
        }

        if (pData->new_message) {
            cout << "[NEW MESSAGE]  " << pData->sender << " -> " << pData->message << endl;
            Sleep(500);
            memset(pData->message, 0, sizeof(pData->message));
            memset(pData->sender, 0, sizeof(pData->sender));
            pData->new_message = false;
            FlushViewOfFile(pData, sizeof(RoomData));  // Đảm bảo thay đổi được đồng bộ hóa vào bộ nhớ chia sẻ
        }

        // 🔥 Kiểm tra nếu có phím bấm mà không chặn chương trình
        if (_kbhit()) {
            char key = _getch();  // Đọc phím từ bàn phím
            if (key == 'q') {
                exitFlag = true;
            }
        }

        Sleep(100);  // 🔥 Giữ chương trình chạy mà không tốn CPU quá nhiều
    }

cleanup:
    // 🔥 Giảm số process khi chương trình thoát
    InterlockedDecrement(&pData->process_count);
    FlushViewOfFile(pData, sizeof(RoomData));
    cout << "[INFO] Process Count after exit: " << pData->process_count << endl;

    if (pData) UnmapViewOfFile(pData);
    if (hMapFile) CloseHandle(hMapFile);

    cout << "[INFO] Shared Memory has been released!" << endl;
    return 0;
}