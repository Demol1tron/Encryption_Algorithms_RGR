#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

#include "funcs.h"

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    LoadCipher("./dlls/atbash.dll");
    LoadCipher("./dlls/hill.dll");
    LoadCipher("./dlls/permutations.dll");

#else
    LoadCipher("./libs/atbash.so");
    LoadCipher("./libs/hill.so");
    LoadCipher("./libs/permutations.so");
#endif

    bool running = true;
    while (running) {
        std::cout << "\n#################################" << std::endl;
        std::cout << "1. Шифрование/дешифрование текста" << std::endl;
        std::cout << "2. Шифрование/дешифрование файла" << std::endl;
        std::cout << "3. Генератор ключей" << std::endl;
        std::cout << "0. Выход" << std::endl;
        std::cout << "#################################" << std::endl;
        std::cout << ">> ";

        char choice;
        std::cin >> choice;
        switch (choice)
        {
            case '1':
                std::cin.ignore(32767, '\n');
                ProcessTextEncryption();
                break;
            case '2':
                std::cin.ignore(32767, '\n');
                ProcessFileEncryption();
                break;
            case '3':
                std::cin.ignore(32767, '\n');
                ShowKeyGenerator();
                break;
            case '0':
                running = false;
                break;
            default:
                std::cout << "Неверный выбор! Попробуйте снова." << std::endl;
                std::cin.ignore(32767, '\n');

        }
    }
    UnloadAllCiphers();
    std::cout << "Выход из программы." << std::endl;
}