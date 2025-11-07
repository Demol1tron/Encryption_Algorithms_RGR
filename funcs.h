#ifndef FUNCS_H
#define FUNCS_H

#include <functional>
#include "./ciphers/api.h"
#include "./utils/file_utils.h"

struct CipherPlugin {
    void* libraryHandle;
    std::function<const char*()> GetCipherName;
    std::function<bool(const uint8_t*)> ValidateKey;
    std::function<void(const uint8_t*, uint8_t*, size_t, const uint8_t*)> EncryptData;
    std::function<void(const uint8_t*, uint8_t*, size_t, const uint8_t*)> DecryptData;
};

bool LoadCipher(const std::string &libraryPath);
void UnloadAllCiphers();
void ShowMainMenu();
void ProcessTextEncryption();
void ProcessFileEncryption();
void ShowKeyGenerator();

#endif