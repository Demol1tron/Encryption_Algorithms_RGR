#include "api.h"
#include <vector>
#include <string>
#include <sstream>

const char* GetCipherName()
{
    return "Хилл"; // 2*2
}

bool ValidateKey(const uint8_t *key)
{
    // ключ = 4 числа через запятую "a,b,c,d" для матрицы [[a,b],[c,d]]
    // "1,2,3,4" -> матрица [[1,2],[3,4]]
    
    std::string keyStr(reinterpret_cast<const char*>(key));
    std::stringstream ss(keyStr);
    std::vector<int> numbers;
    std::string token;
    
    // парсим 4 числа из строки
    while (std::getline(ss, token, ',')) {
        try {
            numbers.push_back(std::stoi(token));
        } catch (...) {
            return false; // не число
        }
    }
    
    // должно быть ровно 4 числа
    if (numbers.size() != 4)
        return false;
    
    // проверяем что определитель нечетный (условие обратимости по mod 256)
    // det = a * d - b * c
    int det = (numbers[0] * numbers[3] - numbers[1] * numbers[2]) % 256;
    
    // должен быть нечетным
    if (det % 2 == 0)
        return false;
    
    return true;
}

void EncryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize, 
                const uint8_t *key)
{
    // парсим ключ-матрицу
    std::string keyStr(reinterpret_cast<const char*>(key));
    std::stringstream ss(keyStr);
    int a, b, c, d;
    char comma;
    ss >> a >> comma >> b >> comma >> c >> comma >> d;
    
    // шифруем блоками по 2 байта
    for (size_t i = 0; i + 1 < dataSize; i += 2) {
        int x1 = inputData[i];
        int x2 = inputData[i + 1];
        
        // матричное умножение: [y1, y2] = [x1, x2] * [[a, b],[c, d]]
        int y1 = (a * x1 + c * x2) % 256;
        int y2 = (b * x1 + d * x2) % 256;
        
        outputData[i] = static_cast<uint8_t>(y1);
        outputData[i + 1] = static_cast<uint8_t>(y2);
    }
    
    // если нечетное количество байт - последний байт без изменений
    if (dataSize % 2 == 1)
        outputData[dataSize - 1] = inputData[dataSize - 1];
}

void DecryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize, 
                const uint8_t *key)
{
    // парсим ключ-матрицу
    std::string keyStr(reinterpret_cast<const char*>(key));
    std::stringstream ss(keyStr);
    int a, b, c, d;
    char comma;
    ss >> a >> comma >> b >> comma >> c >> comma >> d;
    
    // вычисляем обратную матрицу по mod 256
    int det = (a * d - b * c) % 256;
    if (det < 0)
        det += 256;
    
    // находим обратный определитель по простому модулю
    int detInv = -1;
    for (int i = 1; i < 256; ++i)
        if ((det * i) % 256 == 1) {
            detInv = i;
            break;
        }
    
    if (detInv == -1) {
        // не удалось найти обратный - оставляем данные как есть \_(*_*)_/
        for (size_t i = 0; i < dataSize; ++i)
            outputData[i] = inputData[i];
        return;
    }
    
    // обратная матрица: [[d, -b], [-c, a]] * det_inv
    int aInv = (d * detInv) % 256;
    int bInv = (-b * detInv) % 256;
    int cInv = (-c * detInv) % 256;
    int dInv = (a * detInv) % 256;
    
    if (bInv < 0)
        bInv += 256;
    if (cInv < 0)
        cInv += 256;
    
    // дешифруем блоками по 2 байта
    for (size_t i = 0; i + 1 < dataSize; i += 2) {
        int y1 = inputData[i];
        int y2 = inputData[i + 1];
        
        // умножение на обратную матрицу
        int x1 = (aInv * y1 + cInv * y2) % 256;
        int x2 = (bInv * y1 + dInv * y2) % 256;
        
        outputData[i] = static_cast<uint8_t>(x1);
        outputData[i + 1] = static_cast<uint8_t>(x2);
    }
    
    // если нечетное количество байт - последний байт без изменений
    if (dataSize % 2 == 1)
        outputData[dataSize - 1] = inputData[dataSize - 1];
}