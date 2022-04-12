#ifndef __STMMETRIC_CIPHER_H_
#define __STMMETRIC_CIPHER_H_
#include <memory>
#include <unordered_map>
class SymmetricCipher {
   public:
    virtual void setKey(const char* key, int length) = 0;

    virtual std::pair<std::unique_ptr<unsigned char[]>, int> encrypt(const char* data, int length) = 0;
    virtual std::pair<std::unique_ptr<unsigned char[]>, int> decrypt(const char* data, int length) = 0;
};
#endif