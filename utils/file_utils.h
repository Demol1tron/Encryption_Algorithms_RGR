#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <vector>
#include <string>
#include <cstdint>

std::vector<uint8_t> ReadFile(const std::string &filePath);
bool WriteFile(const std::string &filePath, const std::vector<uint8_t> &data);
bool FileExists(const std::string &filePath);
bool CreateDirectoryIfNeeded(const std::string &filePath);

#endif