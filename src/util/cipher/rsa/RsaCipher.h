#ifndef __RSA_CIPHER_H_
#define __RSA_CIPHER_H_

#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#include "ioc.h"
#include "util/cipher/AsymmetricCipher.h"

class RsaCipher : public AsymmetricCipher {
   public:
    RsaCipher() = default;
    RsaCipher(const RsaCipher& r) = delete;
    virtual ~RsaCipher();

    /**
     * @brief Generate rsa public & private key pair string in (PKCS#1) format. public key starts with "-----BEGIN RSA PUBLIC KEY-----" and private key starts
     * with "-----BEGIN RSA PRIVATE KEY-----"
     *
     * @param keyLength unit is bit not byte
     *
     * @return std::pair<std::string, std::string> <publickey.privatkey>
     *
     */
    static std::pair<std::string, std::string> generateRsaKeyPair(int keyLength = 1024);

    /**
     * @brief load public key
     *
     * @param publicKeyString  string of public key in RSA Public Key file (PKCS#1) format. It should begin with "-----BEGIN RSA PUBLIC KEY-----"
     */
    void loadPublicKey(std::string publicKeyString);

    void loadPrivateKey(std::string publicKeyString);

    virtual std::pair<std::unique_ptr<unsigned char[]>, int> encrypt(const unsigned char* data, int length);

    virtual std::pair<std::unique_ptr<unsigned char[]>, int> decrypte(const unsigned char* data, int length);

   private:
    RSA* privateKey = nullptr;
    RSA* publicKey = nullptr;
};
#endif