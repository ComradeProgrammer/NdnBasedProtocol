#ifndef __AES_H_
#define __AES_H_
#include <openssl/aes.h>
#include <string.h>

#include "util/cipher/SymmetricCipher.h"
#include "util/hash/Md5Hasher.h"
class Aes : public SymmetricCipher {
   public:
    /**
     * @brief Set the Key for encryption and decryption. Because the key given may not be 128bit 196bit or 256bit, the key will be hashed into 16bytes (128
     * bit).
     * @param _key pointer to the key
     * @param _length length of key in bytes.
     */
    virtual void setKey(const char* _key, int _length);
    /**
     * @brief encrypt the given data using the key set before
     *
     * @param data  data which is going to be encrypted
     * @param length length of data
     * @return std::pair<std::unique_ptr<unsigned char[]>, int> (encrypted data, length)
     */
    virtual std::pair<std::unique_ptr<unsigned char[]>, int> encrypt(const char* data, int length);

    /**
     * @brief decrypt the given data using the key set before
     *
     * @param data data which is going to be decrypted
     * @param length length of data
     * @return std::pair<std::unique_ptr<unsigned char[]>, int> (decrypted data, length)
     */
    virtual std::pair<std::unique_ptr<unsigned char[]>, int> decrypt(const char* data, int length);

    // default constructor
    Aes() = default;
    // copy constructor
    Aes(const Aes& a);
    // deconstructor
    ~Aes();

   private:
    // key
    char* key = nullptr;
    // length of key in bytes
    int keyLength;
    /**
     * @brief padding function for data to make them align for 16bytes
     *
     * @param in input data
     * @param out output buffer
     * @param inlen int data length
     * @param enc operation type. AES_ENCRYPT for padding, AES_DECRYPT for removing padding.
     * @return int output data length
     */
    int pkcs7_encode(const uint8_t* in, uint8_t* out, int inlen, const int enc);
};
#endif