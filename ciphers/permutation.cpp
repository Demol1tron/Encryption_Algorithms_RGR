#include "api.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

const char* GetCipherName()
{
    return "Фиксированная Перестановка"; // k = 4
}

bool ValidateKey(const uint8_t *key)
{
    // ключ = перестановка чисел 1-4 "3, 2, 4, 1"
    std::string keyStr(reinterpret_cast<const char*>(key));
    std::stringstream ss(keyStr);
    std::vector<int> permutation;
    std::string token;
    
    // парсим перестановку
    while (std::getline(ss, token, ',')) {
        try {
            int num = std::stoi(token);
            permutation.push_back(num);
        } catch (...) {
            return false; // Не число
        }
    }
    
    // должно быть ровно 4 числа
    if (permutation.size() != 4)
        return false;
    
    // проверяем что это перестановка чисел 1-4
    std::vector<int> sorted = permutation;
    std::sort(sorted.begin(), sorted.end());
    for (int i = 0; i < 4; i++)
        if (sorted[i] != i + 1)
            return false;
    
    return true;
}

void EncryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize,
                const uint8_t *key)
{
    // парсим перестановку
    std::string keyStr(reinterpret_cast<const char*>(key));
    std::stringstream ss(keyStr);
    int p1, p2, p3, p4;
    char comma;
    ss >> p1 >> comma >> p2 >> comma >> p3 >> comma >> p4;
    
    // применяем перестановку к блокам по 4 байта
    const int blockSize = 4;
    
    for (size_t blockStart = 0; blockStart < dataSize; blockStart += blockSize) {
        // определяем размер текущего блока (может быть меньше 4 в конце)
        int currentBlockSize = std::min(static_cast<int>(dataSize - blockStart), blockSize);
        
        // создаем временный буфер для блока
        std::vector<uint8_t> block(blockSize, 0); // заполняем нулями если блок неполный
        
        // копируем данные в блок
        for (int i = 0; i < currentBlockSize; i++)
            block[i] = inputData[blockStart + i];
        
        // Применяем перестановку [p1, p2, p3, p4]
        // p1 = 3, p2 = 2, p3 = 4, p4 = 1: [0, 1, 2, 3] -> [2, 1, 3, 0]
        int newPositions[4] = {p1-1, p2-1, p3-1, p4-1}; // переводим в 0-based индексы
        
        for (int i = 0; i < currentBlockSize; i++)
            outputData[blockStart + i] = block[newPositions[i]];
    }
}

void DecryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize, 
                const uint8_t *key)
{
    // парсим перестановку
    std::string keyStr(reinterpret_cast<const char*>(key));
    std::stringstream ss(keyStr);
    int p1, p2, p3, p4;
    char comma;
    ss >> p1 >> comma >> p2 >> comma >> p3 >> comma >> p4;
    
    // вычисляем обратную перестановку
    int originalPositions[4] = {p1 - 1, p2 - 1, p3 - 1, p4 - 1};
    int inversePositions[4];
    
    for (int i = 0; i < 4; ++i)
        inversePositions[originalPositions[i]] = i;
    
    // применяем обратную перестановку к блокам по 4 байта
    const int blockSize = 4;
    
    for (size_t blockStart = 0; blockStart < dataSize; blockStart += blockSize) {
        // определяем размер текущего блока
        int currentBlockSize = std::min(static_cast<int>(dataSize - blockStart), blockSize);
        
        // создаем временный буфер для блока
        std::vector<uint8_t> block(blockSize, 0);
        
        // копируем данные в блок
        for (int i = 0; i < currentBlockSize; ++i)
            block[i] = inputData[blockStart + i];
        
        // Применяем обратную перестановку
        for (int i = 0; i < currentBlockSize; ++i)
            outputData[blockStart + i] = block[inversePositions[i]];
    }
}