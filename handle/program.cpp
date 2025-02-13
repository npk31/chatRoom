#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <atomic>
#include <conio.h>
#include <windows.h>

using namespace std;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #endif
}

int main(){
    filesystem::path file_path = filesystem::absolute(__FILE__);
    filesystem::path __DIR__ = file_path.parent_path();
    filesystem::path dist = __DIR__ / "dist";

    string execute_file[] = {
        "sv.exe",
        "cl.exe",
        "cvs.exe",
        "sm.exe"
    };

    if (!filesystem::exists(dist)) filesystem::create_directory(dist);

    for (string fn: execute_file){
        filesystem::path file = dist / fn;
        if (!filesystem::exists(file)){
            filesystem::path init = __DIR__ / "dist" / "init.exe";

            if (filesystem::exists(init)){
                cout << "Start initialization...." << endl;

                int result = system(init.string().c_str());
                    if (result){
                        cout << result << endl;
                        return 1;
                    }

                clearScreen();
            } else {
                char command[256];
                snprintf(command, sizeof(command), "g++ -std=c++20 %s -o %s", (__DIR__ / "init.cpp").string().c_str(), (__DIR__ / "dist" / "init").string().c_str());
                int result = system(command);
                    if (result){
                        cout << result << endl;
                        return 1;
                    }


                result = system(init.string().c_str());
                    if (result){
                        cout << result << endl;
                        return 1;
                    }

                clearScreen();
            }

        }
    }

    bool exitF = false;

    cout << "Enter your choice: " << endl;
    cout << "1. Create Room" << endl;
    cout << "2. Join Room" << endl;
    cout << "3. Exit" << endl;

    while(!exitF){
        if (_kbhit()) {
            char key = _getch(); 
            if (key == '1') {
                system((dist / "sv.exe").string().c_str());
                exitF = true;
            } else if (key == '2') {
                system((dist / "cl.exe").string().c_str());
                exitF = true;
            } else if (key == '3') {
                exitF = true;
            }
        }
    }

    return 0;
}