#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

vector<string> splitByNull(const char* str, size_t size) {
    vector<string> result;
    size_t i = 0;

    while (i < size) {
        // Bỏ qua các ký tự `\0`
        while (i < size && str[i] == '\0') {
            i++;
        }

        // Nếu gặp ký tự không phải `\0`, bắt đầu một chuỗi mới
        size_t start = i;
        while (i < size && str[i] != '\0') {
            i++;
        }

        // Nếu có chuỗi hợp lệ, thêm vào vector
        if (start < i) {
            result.emplace_back(str + start, i - start);
        }
    }

    return result;
}


void appendString(char* buffer, size_t bufferSize, const char* newString) {
    size_t currentLength = strlen(buffer);
    size_t newStringLength = strlen(newString);

    // Nếu chuỗi mới không vừa với phần còn lại của buffer
    if (currentLength + newStringLength >= bufferSize) {
        size_t charsToRemove = currentLength + newStringLength - bufferSize + 1;

        // Xác định vị trí xóa
        size_t removeIndex = 0;
        while (charsToRemove > 0 && removeIndex < currentLength) {
            if (buffer[removeIndex] == '\0') {
                charsToRemove--;
            }
            removeIndex++;
        }

        // Nếu vẫn còn ký tự cần xóa, tiếp tục xóa các chuỗi cũ
        while (charsToRemove > 0 && removeIndex < currentLength) {
            removeIndex++;
            charsToRemove--;
            if (buffer[removeIndex] == '\0') {
                removeIndex++;
                charsToRemove--;
            }
        }

        // Dịch chuyển chuỗi hiện tại sang trái
        memmove(buffer, buffer + removeIndex, currentLength - removeIndex + 1);
        currentLength -= removeIndex;

        // Đảm bảo buffer có ký tự kết thúc
        buffer[currentLength] = '\0';
    }

    // Thêm chuỗi mới một cách an toàn
    strncat(buffer, newString, bufferSize - currentLength - 1);
}