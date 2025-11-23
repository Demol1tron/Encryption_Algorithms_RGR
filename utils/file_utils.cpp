#include "file_utils.h"
#include <fstream>
#include <iostream>
#include <system_error>

#ifdef _WIN32
    #include <direct.h>
    #include <windows.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

std::vector<uint8_t> ReadFile(const std::string &filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Нельзя открыть файл >> " + filePath);
    
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    
    return buffer;
}

bool WriteFile(const std::string &filePath, const std::vector<unsigned char> &data)
{
    try {
        CreateDirectoryIfNeeded(filePath);
        std::ofstream file(filePath, std::ios::binary);
        if (!file)
            return false;

        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        return true;
    } catch (...) {
        return false;
    }
}

bool FileExists(const std::string &filePath)
{
    std::ifstream file(filePath);
    return file.good();
}

bool CreateDirectoryIfNeeded(const std::string &filePath)
{
    size_t pos = filePath.find_last_of("/\\");
    if (pos == std::string::npos)
        return true; // нет директории в пути
    
    std::string directory = filePath.substr(0, pos);
    if (directory.empty())
        return true;
    
    #ifdef _WIN32
        // создаем все папки рекурсивно
        return _mkdir(directory.c_str()) == 0 || errno == EEXIST;
    #else
        std::string command = "mkdir -p \"" + directory + "\"";
        return system(command.c_str()) == 0;
    #endif
}