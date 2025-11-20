#include <iostream>
#include <random>
#include <optional>
#include "cipher_plugin.h"
#include "../utils/file_utils.h"

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
    std::cout << "Загружен модуль >> " << getName() << std::endl;
    return true;
}

void UnloadAllCiphers()
{
    for (auto &plugin : loadedCiphers) {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(plugin.libraryHandle));
#else
        dlclose(plugin.libraryHandle);
#endif
    }
    loadedCiphers.clear();
}

std::optional<char> CipherChoice()
{
    char cipherChoice;
    while (true) {
        std::cout << "\n<< ДОСТУПНЫЕ АЛГОРИТМЫ >>" << std::endl;
        for (size_t i = 0; i < loadedCiphers.size(); ++i)
            std::cout << i + 1 << ". " << loadedCiphers[i].GetCipherName() << std::endl;
        std::cout << "0. Назад" << std::endl;
        std::cout << ">> ";

        std::cin >> cipherChoice;
        if (cipherChoice == '0') {
            std::cout << "Возврат в главное меню." << std::endl;
            std::cin.ignore(32767, '\n');
            return std::nullopt;
        } else if (cipherChoice != '1' && cipherChoice != '2' && cipherChoice != '3') {
            std::cout << "Неверный выбор! Попробуйте снова." << std::endl;
            std::cin.ignore(32767, '\n');
            continue;
        }
        std::cin.ignore(32767, '\n');
        return cipherChoice;
    }
}

std::optional<char> OperationChoice()
{
    char operation;
    while (true) {
        std::cout << "\n<< ВЫБОР ПРЕОБРАЗОВАНИЯ >>" << std::endl;
        std::cout << "1. Шифровать" << std::endl;
        std::cout << "2. Дешифровать" << std::endl;
        std::cout << "0. Назад" << std::endl;
        std::cout << ">> ";

        std::cin >> operation;
        if (operation == '0') {
            std::cout << "Возврат в меню выбора алгоритма." << std::endl;
            std::cin.ignore(32767, '\n');
            return std::nullopt;
        } else if (operation != '1' && operation != '2') {
            std::cout << "Неверный выбор! Попробуйте снова." << std::endl;
            std::cin.ignore(32767, '\n');
            continue;
        }
        std::cin.ignore(32767, '\n');
        return operation;
    }
}

void ProcessTextEncryption()
{
    if (loadedCiphers.empty()) {
        std::cout << "Отсутствуют загруженные алгоритмы!" << std::endl;
        return;
    }
  
    while (true) {
        auto cipherChoice = CipherChoice();
        if (!cipherChoice)
            return;

        CipherPlugin &cipher = loadedCiphers[cipherChoice.value() - '0' - 1];

        auto operation = OperationChoice();
        if (!operation)
            continue;

        std::cout << "Ключ >> ";
        std::string key;
        std::getline(std::cin, key);

        if (!cipher.ValidateKey(key)) {
            std::cout << "Неверный ключ для выбранного алгоритма! Возврат в меню выбора алгоритма." << std::endl;
            continue;
        }

        std::cout << "Ввод текста >> ";
        std::string text;
        std::getline(std::cin, text);
        
        // результат (3 шифр может увеличить выходной текст)
        std::vector<uint8_t> output(text.size() + 9);
        
        // напрямую с данными строки
        if (operation.value() == '1') {
            cipher.EncryptData(reinterpret_cast<const uint8_t*>(text.data()), output.data(),
                                text.size(), key);
            std::cout << "Зашифрованный текст >> ";
        } else {
            cipher.DecryptData(reinterpret_cast<const uint8_t*>(text.data()), output.data(),
                                text.size(), key);
            std::cout << "Расшифрованный текст >> ";
        }
        
        // вывод в байтах
        for (uint8_t &byte : output)
            std::cout << static_cast<char>(byte);
        std::cout << std::endl;

        return;
    }
}

void ProcessFileEncryption()
{
    if (loadedCiphers.empty()) {
        std::cout << "Отсутствуют загруженные алгоритмы!" << std::endl;
        return;
    }

    while (true) {
        auto cipherChoice = CipherChoice();
        if (!cipherChoice)
            return;

        CipherPlugin &cipher = loadedCiphers[cipherChoice.value() - '0' - 1];

        auto operation = OperationChoice();
        if (!operation)
            continue;

        std::cout << "Ключ >> ";
        std::string key;
        std::getline(std::cin, key);

        if (!cipher.ValidateKey(key)) {
            std::cout << "Неверный ключ для выбранного алгоритма! Возврат в меню выбора алгоритма." << std::endl;
            continue;
        }

        // ввод путей к файлам
        std::string inputPath;
        bool validInputFile = false;
        while (!validInputFile) {
            std::cout << "Путь к исходному файлу >> ";
            std::getline(std::cin, inputPath);
            
            if (!FileExists(inputPath)) {
                std::cout << "Ошибка >> файл '" << inputPath << "' не существует!" << std::endl;
                std::cout << "1. Повторить ввод" << std::endl;
                std::cout << "2. Отменить операцию" << std::endl;
                std::cout << ">> ";
                
                char retryChoice;
                std::cin >> retryChoice;
                std::cout << std::endl;
                
                switch (retryChoice)
                {
                    case '1':
                        std::cin.ignore(32767, '\n');
                        break;
                    case '2':
                        std::cout << "Операция отменена." << std::endl;
                        std::cin.ignore(32767, '\n');
                        return;
                    default:
                        std::cout << "Неверный выбор! Попробуйте снова." << std::endl;
                        std::cin.ignore(32767, '\n');
                        break;
                }
            } else
                validInputFile = true;
        }

        // ввод и проверка выходного файла
        std::string outputPath;
        bool validOutputFile = false;
        while (!validOutputFile) {
            std::cout << "Путь к выходному файлу >> ";
            std::getline(std::cin, outputPath);
            
            if (FileExists(outputPath)) {
                std::cout << "\nВнимание >> файл '" << outputPath << "' уже существует!" << std::endl;
                std::cout << "1. Перезаписать файл" << std::endl;
                std::cout << "2. Ввести новый путь" << std::endl;
                std::cout << "3. Отменить операцию" << std::endl;
                std::cout << ">> ";
                
                char overwriteChoice;
                std::cin >> overwriteChoice;
                std::cout << std::endl;
                
                switch (overwriteChoice)
                {
                    case 1:
                        std::cout << "Файл будет перезаписан." << std::endl;
                        validOutputFile = true;
                        std::cin.ignore(32767, '\n');
                        break;
                    case 2:
                        std::cin.ignore(32767, '\n');
                        break;
                    case 3:
                        std::cout << "Операция отменена." << std::endl;
                        std::cin.ignore(32767, '\n');
                        return;
                    default:
                        std::cout << "Неверный выбор! Попробуйте снова." << std::endl;
                        std::cin.ignore(32767, '\n');
                        break;
                }
            } else
                validOutputFile = true;
        }

        try {
            // чтение
            std::vector<uint8_t> inputData = ReadFile(inputPath);
            std::vector<uint8_t> outputData(inputData.size() /*+ 9*/);
            
            if (operation.value() == '1') {
                cipher.EncryptData(inputData.data(), outputData.data(), inputData.size(),
                                    key);
                std::cout << "Файл зашифрован." << std::endl;
            } else {
                cipher.DecryptData(inputData.data(), outputData.data(), inputData.size(),
                                    key);
                std::cout << "Файл дешифрован." << std::endl;
            }
            
            // запись
            if (WriteFile(outputPath, outputData))
                std::cout << "Результат сохранен в >> " << outputPath << std::endl;
            else
                std::cout << "Ошибка сохранения файла!" << std::endl;
            
        } catch (const std::runtime_error &e) {
            std::cout << "Ошибка чтения файла! >> " << e.what() << std::endl;
        } catch (const std::exception &e) {
            std::cout << "Неожиданная ошибка! >> " << e.what() << std::endl;
        }
        return;
    }
}

void ShowKeyGenerator()
{
    if (loadedCiphers.empty()) {
        std::cout << "Отсутствуют загруженные алгоритмы!" << std::endl;
        return;
    }

    auto cipherChoice = CipherChoice();
    if (!cipherChoice)
        return;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist1(1, 255);
    std::uniform_int_distribution<int> dist0(0, 255);
    
    switch (cipherChoice.value())
    {
        case '1':
            std::cout << "Атбаш не использует ключ. Можно ввести любой текст, например >> привет, мир!" << std::endl;
            break;
        case '2': {
            // случайная матрица с неч det
            int a, b, c, d;
            int attempts = 0;
            do {
                a = dist1(gen); // от 1 до 255
                b = dist0(gen);
                c = dist0(gen);
                d = dist1(gen); // от 1 до 255
                attempts++;
                
                // упрощение при долгом переборе
                if (attempts > 100) {
                    a = 1; b = 0; c = 0; d = 1; // E
                    break;
                }
            } while ((a * d - b * c) % 2 == 0); // det % 2 == 0
            
            std::string key = std::to_string(a) + ", " + std::to_string(b) + ", " +
                            std::to_string(c) + ", " + std::to_string(d);
            
            std::cout << "Сгенерированный ключ для шифра Хилла >> " << key << std::endl;
            std::cout << "Матрица >> [[" << a << ", " << b << "], [" << c << ", " << d << "]]" << std::endl;
            std::cout << "Определитель >> " << (a * d - b * c) << " (нечетный)" << std::endl;
            break;
        }
        case '3': {
            // случайная перестановка
            std::vector<int> numbers {1, 2, 3, 4};
            
            for (int i = 3; i > 0; --i) {
                std::uniform_int_distribution<int> dist(0, i);
                int j = dist(gen);
                std::swap(numbers[i], numbers[j]);
            }
            
            std::string key = std::to_string(numbers[0]) + ", " + std::to_string(numbers[1]) + 
                            "," + std::to_string(numbers[2]) + ", " + std::to_string(numbers[3]);
            std::cout << "Сгенерированный ключ для перестановки >> " << key << std::endl;
            std::cout << "Перестановка >> [1, 2, 3, 4] -> [" << numbers[0] << ", " 
                    << numbers[1] << ", " << numbers[2] << ", " << numbers[3] << "]" << std::endl;
            break;
        }
    }

    std::cout << "\nНажмите Enter для продолжения >>> ";
    // std::cin.ignore(32767, '\n');
    std::cin.get();
}