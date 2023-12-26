#include <string>
#include <vector>
#include <fstream>

std::vector<std::string> readFile(std::string filename) {
    std::ifstream file(filename);
    std::string str;
    std::vector<std::string> file_contents;
    while (std::getline(file, str)) {
        file_contents.push_back(str);
    }
    return file_contents;
}