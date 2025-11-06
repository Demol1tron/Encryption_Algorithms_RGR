#ifndef API_H
#define API_H

#include <cstddef>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

// функции каждой библиотеки шифра
const char* GetCipherName();
bool ValidateKey(const uint8_t *key);
void EncryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize, 
                const uint8_t *key);
void DecryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize, 
                const uint8_t *key);

#ifdef __cplusplus
}
#endif

#endif