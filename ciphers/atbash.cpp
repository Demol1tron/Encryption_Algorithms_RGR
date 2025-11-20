#include "api.h"

const char* GetCipherName()
{
    return "Атбаш";
}

bool ValidateKey(const std::string &key)
{
    return true;
}

void EncryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize,
                const std::string &key)
{
    for (size_t i = 0; i < dataSize; ++i)
        outputData[i] = 255 - inputData[i];
}

void DecryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize,
                const std::string &key)
{
    EncryptData(inputData, outputData, dataSize, key);
}