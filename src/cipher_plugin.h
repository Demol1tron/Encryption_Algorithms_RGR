#ifndef CIPHER_PLUGIN_H
#define CIPHER_PLUGIN_H

#include <cstdint>
#include <string>
#include <vector>

struct CipherPlugin {
    void* libraryHandle;
    const char*(*GetCipherName)();
    bool(*ValidateKey)(const std::string&);
    void(*EncryptData)(const uint8_t*, uint8_t*, size_t, const std::string&);
    void(*DecryptData)(const uint8_t*, uint8_t*, size_t, const std::string&);
};

#endif