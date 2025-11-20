#include "api.h"
#include <vector>
#include <sstream>

const char* GetCipherName()
{
    return "Шифр Хилла (2*2)";
}

bool ValidateKey(const std::string &key)
{
    std::stringstream ss(key);
    std::vector<int> numbers;
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        try {
            numbers.push_back(std::stoi(token));
        } catch (...) {
            return false; // не число
        }
    }
    
    if (numbers.size() != 4)
        return false;
    
    int det = (numbers[0] * numbers[3] - numbers[1] * numbers[2]) % 256;
    if (det % 2 == 0)
        return false;
    
    return true;
}

void EncryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize, 
                const std::string &key)
{
    std::stringstream ss(key);
    int a, b, c, d;
    char comma;
    ss >> a >> comma >> b >> comma >> c >> comma >> d;
    
    for (size_t i = 0; i + 1 < dataSize; i += 2) {
        int x1 = inputData[i];
        int x2 = inputData[i + 1];
        
        int y1 = (a * x1 + b * x2) % 256;
        int y2 = (c * x1 + d * x2) % 256;

        if (y1 < 0)
            y1 += 256;
        if (y2 < 0)
            y2 += 256;
        
        outputData[i] = static_cast<uint8_t>(y1);
        outputData[i + 1] = static_cast<uint8_t>(y2);
    }
    
    // если нечетное количество байт - последний байт без изменений
    if (dataSize % 2 == 1)
        outputData[dataSize - 1] = inputData[dataSize - 1];
}

void DecryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize, 
                const std::string &key)
{
    std::stringstream ss(key);
    int a, b, c, d;
    char comma;
    ss >> a >> comma >> b >> comma >> c >> comma >> d;
    
    int det = (a * d - b * c) % 256;
    if (det < 0)
        det += 256;
    
    int detInv = -1;
    for (int i = 1; i < 256; ++i)
        if ((det * i) % 256 == 1) {
            detInv = i;
            break;
        }
    
    if (detInv == -1) {
        // не удалось найти обратный det
        for (size_t i = 0; i < dataSize; ++i)
            outputData[i] = inputData[i];
        return;
    }
    
    // [[d, -b], [-c, a]] * detInv
    int aInv = (d * detInv) % 256;
    int bInv = (-b * detInv) % 256;
    int cInv = (-c * detInv) % 256;
    int dInv = (a * detInv) % 256;
    
    if (aInv <0)
        aInv += 256;
    if (bInv < 0)
        bInv += 256;
    if (cInv < 0)
        cInv += 256;
    if (dInv < 0)
        dInv += 256;
    
    // дешифруем блоками по 2 байта
    for (size_t i = 0; i + 1 < dataSize; i += 2) {
        int y1 = inputData[i];
        int y2 = inputData[i + 1];
        
        int x1 = (aInv * y1 + bInv * y2) % 256;
        int x2 = (cInv * y1 + dInv * y2) % 256;

        if (x1 < 0)
            x1 += 256;
        if (x2 < 0)
            x2 += 256;
        
        outputData[i] = static_cast<uint8_t>(x1);
        outputData[i + 1] = static_cast<uint8_t>(x2);
    }
    
    // если нечетное количество байт - последний байт без изменений
    if (dataSize % 2 == 1)
        outputData[dataSize - 1] = inputData[dataSize - 1];
}