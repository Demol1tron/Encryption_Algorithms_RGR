#include "api.h"

const char* GetCipherName()
{
    return "Атбаш";
}

bool ValidateKey(const uint8_t *key)
{
    return true; // любой (Атбаш не использует ключ)
}

void EncryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize,
                const uint8_t *key)
{
    for (size_t i = 0; i < dataSize; ++i)
        outputData[i] = 255 - inputData[i];
}

void DecryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize,
                const uint8_t *key)
{
    EncryptData(inputData, outputData, dataSize, key); // шифрование и дешифрование одинаковы
}