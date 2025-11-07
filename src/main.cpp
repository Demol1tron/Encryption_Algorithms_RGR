#include <iostream>
#include <random>
#include "funcs.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

std::vector<CipherPlugin> loadedCiphers;

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    LoadCipher("../dlls/atbash.dll");
    LoadCipher("../dlls/hill.dll");
    LoadCipher("../dlls/permutation.dll");

#else
    LoadCipher("../libs/atbash.so");
    LoadCipher("../libs/hill.so");
    LoadCipher("../libs/permutation.so");
#endif

    bool running = true;
    while (running) {
        ShowMainMenu();
        
        int choice;
        std::cin >> choice;
        std::cin.ignore();
        
        switch (choice) {
            case 1:
                ProcessTextEncryption();
                break;
            case 2:
                ProcessFileEncryption();
                break;
            case 3:
                ShowKeyGenerator();
                break;
            case 0:
                running = false;
                break;
            default:
                std::cout << "Неверный выбор!" << std::endl;
        }
    }
    
    UnloadAllCiphers();
    std::cout << "Выход из программы." << std::endl;
}

bool LoadCipher(const std::string &libraryPath)
{
#ifdef _WIN32
    HMODULE library = LoadLibraryA(libraryPath.c_str());
    if (!library) {
        std::cout << "Ошибка загрузки библиотеки: " << libraryPath << std::endl;
        return false;
    }
    
    auto getName = (const char*(*)())GetProcAddress(library, "GetCipherName");
    auto validateKey = (bool(*)(const uint8_t*))GetProcAddress(library, "ValidateKey");
    auto encrypt = (void(*)(const uint8_t*, uint8_t*, size_t, const uint8_t*))GetProcAddress(library, "EncryptData");
    auto decrypt = (void(*)(const uint8_t*, uint8_t*, size_t, const uint8_t*))GetProcAddress(library, "DecryptData");
#else
    void *library = dlopen(libraryPath.c_str(), RTLD_LAZY);
    if (!library) {
        std::cout << "Ошибка загрузки библиотеки: " << dlerror() << std::endl;
        return false;
    }
    
    auto getName = (const char*(*)())dlsym(library, "GetCipherName");
    auto validateKey = (bool(*)(const uint8_t*))dlsym(library, "ValidateKey");
    auto encrypt = (void(*)(const uint8_t*, uint8_t*, size_t, const uint8_t*))dlsym(library, "EncryptData");
    auto decrypt = (void(*)(const uint8_t*, uint8_t*, size_t, const uint8_t*))dlsym(library, "DecryptData");
#endif

    if (!getName || !validateKey || !encrypt || !decrypt) {
        std::cout << "Библиотека не содержит все необходимые функции" << std::endl;
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

void ShowMainMenu()
{
    std::cout << "\n*********************************" << std::endl;
    std::cout << "1. Шифрование/дешифрование текста" << std::endl;
    std::cout << "2. Шифрование/дешифрование файла" << std::endl;
    std::cout << "3. Генератор ключей" << std::endl;
    std::cout << "0. Выход" << std::endl;
    std::cout << "Выберите действие: ";
}

void ProcessTextEncryption()
{
    if (loadedCiphers.empty()) {
        std::cout << "Нет загруженных шифров!" << std::endl;
        return;
    }

    std::cout << "\n***Доступные шифры***" << std::endl;
    for (size_t i = 0; i < loadedCiphers.size(); ++i)
        std::cout << i + 1 << ". " << loadedCiphers[i].GetCipherName() << std::endl;

    std::cout << "Выберите шифр: ";
    
    int cipherChoice;
    std::cin >> cipherChoice;
    std::cin.ignore();
    
    if (cipherChoice < 1 || cipherChoice > loadedCiphers.size()) {
        std::cout << "Неверный выбор шифра!" << std::endl;
        return;
    }
    
    CipherPlugin &cipher = loadedCiphers[cipherChoice - 1];

    std::cout << "\nВыбор операции:" << std::endl;
    std::cout << "1. Шифрование" << std::endl;
    std::cout << "2. Дешифрование" << std::endl;
    std::cout << "Выберите операцию: ";
    
    int operation;
    std::cin >> operation;
    std::cin.ignore();
    
    if (operation != 1 && operation != 2) {
        std::cout << "Неверный выбор операции!" << std::endl;
        return;
    }

    std::cout << "Введите ключ: ";
    std::string keyStr;
    std::getline(std::cin, keyStr);
    
    // прямая работа с ключом без конвертации
    if (!cipher.ValidateKey(reinterpret_cast<const uint8_t*>(keyStr.c_str()))) {
        std::cout << "Неверный ключ для выбранного шифра!" << std::endl;
        return;
    }

    std::cout << "Введите текст: ";
    std::string text;
    std::getline(std::cin, text);
    
    // память для результата
    std::vector<uint8_t> output(text.size());
    
    // напрямую с данными строки
    if (operation == 1) {
        cipher.EncryptData(reinterpret_cast<const uint8_t*>(text.data()), output.data(),
                            text.size(), reinterpret_cast<const uint8_t*>(keyStr.c_str()));
        std::cout << "Зашифрованный текст: ";
    } else {
        cipher.DecryptData(reinterpret_cast<const uint8_t*>(text.data()), output.data(),
                            text.size(), reinterpret_cast<const uint8_t*>(keyStr.c_str()));
        std::cout << "Расшифрованный текст: ";
    }
    
    // вывод в байтах
    std::cout << "Результат: ";
    for (uint8_t &byte : output)
        std::cout << static_cast<char>(byte);

    std::cout << std::endl;
}

void ProcessFileEncryption()
{
    if (loadedCiphers.empty()) {
        std::cout << "Нет загруженных шифров!" << std::endl;
        return;
    }

    std::cout << "\n***Доступные шифры***" << std::endl;
    for (size_t i = 0; i < loadedCiphers.size(); ++i)
        std::cout << i + 1 << ". " << loadedCiphers[i].GetCipherName() << std::endl;

    std::cout << "Выберите шифр: ";
    
    int cipherChoice;
    std::cin >> cipherChoice;
    std::cin.ignore();
    
    if (cipherChoice < 1 || cipherChoice > loadedCiphers.size()) {
        std::cout << "Неверный выбор шифра!" << std::endl;
        return;
    }
    
    CipherPlugin &cipher = loadedCiphers[cipherChoice - 1];

    std::cout << "\nВыбор операции:" << std::endl;
    std::cout << "1. Шифрование" << std::endl;
    std::cout << "2. Дешифрование" << std::endl;
    std::cout << "Выберите операцию: ";
    
    int operation;
    std::cin >> operation;
    std::cin.ignore();
    
    if (operation != 1 && operation != 2) {
        std::cout << "Неверный выбор операции!" << std::endl;
        return;
    }

    std::cout << "Введите ключ: ";
    std::string keyStr;
    std::getline(std::cin, keyStr);
    
    if (!cipher.ValidateKey(reinterpret_cast<const uint8_t*>(keyStr.c_str()))) {
        std::cout << "Неверный ключ для выбранного шифра!" << std::endl;
        return;
    }

    // ввод путей к файлам
    std::cout << "Введите путь к исходному файлу: ";
    std::string inputPath;
    std::getline(std::cin, inputPath);
    
    std::cout << "Введите путь для выходного файла: ";
    std::string outputPath;
    std::getline(std::cin, outputPath);

    try {
        // чтение
        std::vector<uint8_t> inputData = ReadFile(inputPath);
        std::vector<uint8_t> outputData(inputData.size());
        
        if (operation == 1) {
            cipher.EncryptData(inputData.data(), outputData.data(), inputData.size(),
                                reinterpret_cast<const uint8_t*>(keyStr.c_str()));
            std::cout << "Файл зашифрован." << std::endl;
        } else {
            cipher.DecryptData(inputData.data(), outputData.data(), inputData.size(),
                                reinterpret_cast<const uint8_t*>(keyStr.c_str()));
            std::cout << "Файл расшифрован." << std::endl;
        }
        
        // запись
        if (WriteFile(outputPath, outputData))
            std::cout << "Результат сохранен в: " << outputPath << std::endl;
        else
            std::cout << "Ошибка сохранения файла!" << std::endl;
        
    } catch (const std::exception &e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }
}

void ShowKeyGenerator()
{
    std::cout << "\n***Генератор ключей***" << std::endl;
    std::cout << "1. Атбаш" << std::endl;
    std::cout << "2. Хилл" << std::endl;
    std::cout << "3. Перестановка" << std::endl;
    std::cout << "Выберите шифр для генерации ключа: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    // Инициализируем генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist1(1, 255);
    std::uniform_int_distribution<int> dist0(0, 255);
    
    switch (choice) {
        case 1:
            std::cout << "Атбаш не использует ключ, можно ввести любой текст" << std::endl;
            break;
        case 2: {
            // случайная матрица с неч det
            int a, b, c, d;
            int attempts = 0;
            do {
                a = dist1(gen);  // от 1 до 255
                b = dist0(gen);
                c = dist0(gen);
                d = dist1(gen);  // от 1 до 255
                attempts++;
                
                // упрощение при долгом переборе
                if (attempts > 100) {
                    a = 1; b = 0; c = 0; d = 1; // E
                    break;
                }
            } while ((a * d - b * c) % 2 == 0); // det % 2 == 0
            
            std::string key = std::to_string(a) + "," + std::to_string(b) + "," +
                            std::to_string(c) + "," + std::to_string(d);
            
            std::cout << "Сгенерированный ключ для Хилла: " << key << std::endl;
            std::cout << "Матрица: [[" << a << "," << b << "], [" << c << "," << d << "]]" << std::endl;
            std::cout << "Определитель: " << (a * d - b * c) << " (нечетный)" << std::endl;
            break;
        }
        case 3: {
            // случайная перестановка
            std::vector<int> numbers {1, 2, 3, 4};
            
            for (int i = 3; i > 0; --i) {
                std::uniform_int_distribution<int> dist(0, i);
                int j = dist(gen);
                std::swap(numbers[i], numbers[j]);
            }
            
            std::string key = std::to_string(numbers[0]) + "," + std::to_string(numbers[1]) + 
                            "," + std::to_string(numbers[2]) + "," + std::to_string(numbers[3]);
            std::cout << "Сгенерированный ключ для Перестановки: " << key << std::endl;
            std::cout << "Перестановка: [1,2,3,4] -> [" << numbers[0] << "," 
                      << numbers[1] << "," << numbers[2] << "," << numbers[3] << "]" << std::endl;
            break;
        }
        default:
            std::cout << "Неверный выбор!" << std::endl;
    }
    
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.get();
}