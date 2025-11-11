#include <iostream>
#include <random>
#include "cipher_plugin.h"
#include "../utils/file_utils.h"

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
    if (!cipher.ValidateKey(keyStr)) {
        std::cout << "Неверный ключ для выбранного шифра!" << std::endl;
        return;
    }

    std::cout << "Введите текст: ";
    std::string text;
    std::getline(std::cin, text);
    
    // результат (3 шифр может увеличить выход)
    std::vector<uint8_t> output(text.size() + 100);
    
    // напрямую с данными строки
    if (operation == 1) {
        cipher.EncryptData(reinterpret_cast<const uint8_t*>(text.data()), output.data(),
                            text.size(), keyStr);
        std::cout << "Зашифрованный текст: ";
    } else {
        cipher.DecryptData(reinterpret_cast<const uint8_t*>(text.data()), output.data(),
                            text.size(), keyStr);
        std::cout << "Расшифрованный текст: ";
    }
    
    // вывод в байтах
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
    
    if (!cipher.ValidateKey(keyStr)) {
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
                                keyStr);
            std::cout << "Файл зашифрован." << std::endl;
        } else {
            cipher.DecryptData(inputData.data(), outputData.data(), inputData.size(),
                                keyStr);
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
    if (loadedCiphers.empty()) {
        std::cout << "Нет загруженных шифров!" << std::endl;
        return;
    }

    std::cout << "\n***Генератор ключей***" << std::endl;
    for (size_t i = 0; i < loadedCiphers.size(); ++i)
        std::cout << i + 1 << ". " << loadedCiphers[i].GetCipherName() << std::endl;
    std::cout << "Выберите шифр для генерации ключа: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    // Инициализируем генератор случайных чисел
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist1(1, 255);
    std::uniform_int_distribution<int> dist0(0, 255);
    
    switch (choice)
    {
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
            
            std::cout << "Сгенерированный ключ для шифра Хилла: " << key << std::endl;
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
            std::cout << "Сгенерированный ключ для перестановки: " << key << std::endl;
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
