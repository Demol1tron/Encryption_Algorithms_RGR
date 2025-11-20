#ifndef API_H
#define API_H

#include <cstddef>
#include <cstdint>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

const char* GetCipherName();
bool ValidateKey(const std::string &key);
void EncryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize, 
                const std::string &key);
void DecryptData(const uint8_t *inputData, uint8_t *outputData, size_t dataSize, 
                const std::string &key);

#ifdef __cplusplus
}
#endif

#endif