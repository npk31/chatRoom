#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <format>

using namespace std;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


int main() {
    filesystem::path file_path = filesystem::absolute(__FILE__);
    filesystem::path __DIR__ = file_path.parent_path();
    filesystem::path dist = __DIR__ / "dist";

    char command[256];
    string commands[] = {
        (snprintf(command, sizeof(command), "g++ %s -o %s", (__DIR__ / "server.cpp").string().c_str(), (dist / "sv").string().c_str()), string(command)),
        (snprintf(command, sizeof(command), "g++ %s -o %s", (__DIR__ / "client.cpp").string().c_str(), (dist / "cl").string().c_str()), string(command)),
        (snprintf(command, sizeof(command), "g++ -I %s %s -o %s", __DIR__.string().c_str(), (__DIR__ / "conversation.cpp").string().c_str(), (dist / "cvs").string().c_str()), string(command)),
        (snprintf(command, sizeof(command), "g++ -I %s %s -o %s", __DIR__.string().c_str(), (__DIR__ / "sendMessage.cpp").string().c_str(), (dist / "sm").string().c_str()), string(command)),
    };

    int i = 0;
    size_t size = sizeof(commands) / sizeof(commands[0]);

    for (string command: commands){
        cout << "Compiling... " << i + 1 << "/" << size  << endl;
        int result = system(command.c_str());
        if (result != 0) {
            cerr << "Error compiling: " << command << endl;
            return 1;
        }
        clearScreen();
        i++;
    }

    cout << "Initialization complete." << endl;

    return 0;
}