#include <iostream>
#include "cipher_plugin.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

std::vector<CipherPlugin> loadedCiphers;

bool LoadCipher(const std::string &libraryPath)
{
#ifdef _WIN32
    HMODULE library = LoadLibraryA(libraryPath.c_str());
    if (!library) {
        std::cout << "Ошибка загрузки библиотеки >> " << libraryPath << std::endl;
        return false;
    }
    
    auto getName = (const char*(*)())GetProcAddress(library, "GetCipherName");
    auto validateKey = (bool(*)(const std::string&))GetProcAddress(library, "ValidateKey");
    auto encrypt = (void(*)(const uint8_t*, uint8_t*, size_t, const std::string&))GetProcAddress(library, "EncryptData");
    auto decrypt = (void(*)(const uint8_t*, uint8_t*, size_t, const std::string&))GetProcAddress(library, "DecryptData");
#else
    void *library = dlopen(libraryPath.c_str(), RTLD_LAZY);
    if (!library) {
        std::cout << "Ошибка загрузки библиотеки >> " << dlerror() << std::endl;
        return false;
    }
    
    auto getName = (const char*(*)())dlsym(library, "GetCipherName");
    auto validateKey = (bool(*)(const std::string&))dlsym(library, "ValidateKey");
    auto encrypt = (void(*)(const uint8_t*, uint8_t*, size_t, const std::string&))dlsym(library, "EncryptData");
    auto decrypt = (void(*)(const uint8_t*, uint8_t*, size_t, const std::string&))dlsym(library, "DecryptData");
#endif

    if (!getName || !validateKey || !encrypt || !decrypt) {
        std::cout << "Библиотека не содержит все необходимые функции!" << std::endl;
#ifdef _WIN32
        FreeLibrary(library);
#else
        dlclose(library);
#endif
        return false;
    }

    CipherPlugin plugin;
    plugin.libraryHandle = library;
    plugin.GetCipherName = getName;
    plugin.ValidateKey = validateKey;
    plugin.EncryptData = encrypt;
    plugin.DecryptData = decrypt;

    loadedCiphers.push_back(plugin);
    std::cout << "Загружен шифр: " << getName() << std::endl;
    return true;
}

void UnloadAllCiphers()
{
    for (auto &plugin : loadedCiphers) {
#ifdef _WIN32
        FreeLibrary((HMODULE)plugin.libraryHandle);
#else
        dlclose(plugin.libraryHandle);
#endif
    }
    loadedCiphers.clear();
}
