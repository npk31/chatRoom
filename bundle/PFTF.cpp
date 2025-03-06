#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <filesystem>
#include <stdlib.h>
#include <iomanip>

using namespace std;
namespace fs = filesystem;

string replaceString(string str, string from, string to){
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

unordered_map<string, string> handleArgv(int argc, char* argv[]) {
    unordered_map<string, string> new_argv;

    unordered_map<string, int> commands = {
        {"-src", 1},
        {"-cb", 2},
        {"-o", 3}
    };

    for (int i = 0; i < argc; i++) {
        string arg = argv[i];

        if (commands.count(arg) > 0) {  // Kiểm tra nếu arg có trong commands
            switch (commands[arg]) {
                case 1:
                case 2:
                case 3:
                    if (i + 1 < argc) {  // Kiểm tra tránh truy cập ngoài phạm vi
                        new_argv[replaceString(arg, "-", "")] = argv[i + 1];  // Loại bỏ dấu '-'
                    }
                    break;
                default:
                    break;
            }
        }
    }

    return new_argv;
}

string getContentFile(filesystem::path path){
    ifstream file(path);

    if (file.is_open()){
        stringstream buffer;
        buffer << file.rdbuf();
        string content = buffer.str();

        file.close();
        return content;
    } else {
        cerr << "Error: Unable to open file" << endl;
        return "";
    }

}

bool writeFile(string path, string content){
    ofstream file(path);

    if (file.is_open()){
        file << content;
        file.close();
        return true;
    } else {
        cerr << "Error: Unable to open file" << endl;
        return false;
    }
}

string escapeForCppString(const string& input) {
    string output;
    for (char c : input) {
        if (c == '\\') output += "\\\\";  // Thoát dấu '\'
        else if (c == '\"') output += "\\\"";  // Thoát dấu '"'
        else if (c == '\n') output += "\\n";  // Thay newline thành "\\n"
        else output += c;
    }
    return output;
}



string indentText(const string& input) {
    string output;
    for (char c : input) {
        if (c == '\n') output += "\n    ";
        else output += c;
    }
    return output;
}

string convertFileToFunctionCreateFile(string filename, string content) {
    content = escapeForCppString(content); 

    string func = "bool __C_" + filename + "(filesystem::path __DIST__){\n";
    func += "    filesystem::path currentFilePath = __DIST__ / \"" + replaceString(filename, "_", ".") + "\";\n";
    func += "    string content = \"" + content + "\";\n"; 
    func += "    ofstream file(currentFilePath);\n";
    func += "    if (file.is_open()){\n";
    func += "        file << content;\n";
    func += "        file.close();\n";
    func += "        return true;\n";
    func += "    } else {\n";
    func += "        cerr << \"Error: Unable to open file\" << endl;\n";
    func += "        return false;\n";
    func += "    }\n";
    func += "}\n";

    return func;
}



map<string, string> getFilesOfFolder(string path){
    map<string, string> files;

    for (const auto& entry : fs::directory_iterator(path)){
        if (fs::is_regular_file(entry.path())){
            string content = getContentFile(entry.path());
            files[entry.path().filename().string()] = content;
        }
    }


    return files;
}

string getContentNewFile(string path, string folderName, string cb_content){
    string content = R"(
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

using namespace std;
namespace fs = filesystem;




    )";

    map<string, string> filelist = getFilesOfFolder(path);
    string execute_func = "\n";

    for (const auto& file : filelist){
        content += convertFileToFunctionCreateFile(replaceString(file.first, ".", "_"), file.second);
        execute_func += "    __C_" + replaceString(file.first, ".", "_") + "(__DIST__);\n";
    }

    execute_func += "\n\n\n    " + indentText(cb_content) + "\n\n\n";

    content += R"(



int main(){
    cout << "Proceed to unpack..." << endl;
    fs::path __DIR__ = fs::current_path();
)";

    content += "    fs::path __DIST__ = __DIR__ / \"" + folderName + "\";\n";
    content += R"(
    if (!fs::exists(__DIST__)){
        fs::create_directory(__DIST__);
        std::cout << "Created directory: " << __DIST__ << std::endl;
    }



    )";

    content += execute_func;
    content += R"(


    return 0;
}
    )";

    return content;

}


void packToNewFile(string name, fs::path folderPath, string cb_content){    
    string folderName = folderPath.filename().string();
    string content = getContentNewFile(folderPath.string(), folderName, cb_content);
    string fn = name + ".cpp";
    bool result = writeFile(fn, content);

    if (result){
        system(("g++ -std=c++20 " + fn + " -o " + name + " -static").c_str());
        
        if (fs::exists(fn)) {  // Check if the file exists
            cout << "Success: File " <<  replaceString(fn, ".cpp", ".exe") << "  created!" << endl;
            if (fs::remove(fn)) { // Delete the file
            } else {
                cerr << "Error: Unable to remove temporary file" << fn << endl;
            }
        } else {
            cerr << "Error: Temporary file" << fn << "does not exist." << endl;
        }
    } else {
        cerr << "Error: Unable to create file" << fn << endl;
    }
}




int main (int argc, char* argv[]){
    unordered_map<string, string> args = handleArgv(argc, argv);
    string src;

    if (args.empty()){
        cerr << "Invalid Command." << endl;
        return 1;
    } else {
        if (args.find("src") == args.end()){
            cerr << "Invalid Command." << endl;
            return 1;
        } else {
            src = args.find("src")->second;
        }
    }

    
    fs::path folderPath = src;


    if (!fs::exists(folderPath)){
        cerr << "Error: Folder not found!" << endl;
        return 1;
    } else {
        if (!fs::is_directory(folderPath)){
            cerr << "Error: Path is not a folder!" << endl;
            return 1;
        }
    }


    string output_name = args.find("o") != args.end() ? args.find("o")->second : "pack";
    string callback_content = args.find("cb") != args.end() ? getContentFile(args.find("cb")->second) : "";



    packToNewFile(output_name, folderPath, callback_content);

    return 0;
}