#ifndef FUNCS_H
#define FUNCS_H

#include <string>

namespace Security {
    const std::string password = "abs426";
    const int maxAttempts = 3;
}

bool CheckPassword();
bool LoadCipher(const std::string &libraryPath);
void UnloadAllCiphers();
void ProcessTextEncryption();
void ProcessFileEncryption();
void ShowKeyGenerator();

#endif