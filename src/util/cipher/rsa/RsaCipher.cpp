#include "RsaCipher.h"
using namespace std;

void RsaCipher::loadPublicKey(string publicKeyString) {
    BIO* bio = BIO_new_mem_buf((void*)publicKeyString.c_str(), -1);
    publicKey = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);

    if (publicKey == nullptr) {
        LOGGER->ERROR("failed to parse publicKeyString");
    }
    BIO_free_all(bio);
}

void RsaCipher::loadPrivateKey(string privateKeyString) {
    BIO* bio = BIO_new_mem_buf((void*)privateKeyString.c_str(), -1);
    privateKey = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);

    if (privateKey == nullptr) {
        LOGGER->ERROR("failed to parse publicKeyString");
    }
    BIO_free_all(bio);
}

pair<std::unique_ptr<unsigned char[]>, int> RsaCipher::encrypt(const unsigned char* data, int length) {
    //加密的明文长度不能超过RSA密钥的长度-11，比如1024位的，明文长度不能超过117。
    unsigned char* buffer = new unsigned char[128];
    int ret = RSA_private_encrypt(length, data, buffer, privateKey, RSA_PKCS1_PADDING);
    if (ret < 0) {
        LOGGER->ERROR("RsaCipher::encrypt failed to encrypt");
    }
    return {unique_ptr<unsigned char[]>(buffer), ret};
}

pair<std::unique_ptr<unsigned char[]>, int> RsaCipher::decrypte(const unsigned char* data, int length) {
    unsigned char* buffer = new unsigned char[128];
    int ret = RSA_public_decrypt(length, data, buffer, publicKey, RSA_PKCS1_PADDING);
    if (ret < 0) {
        LOGGER->ERROR("RsaCipher::decrypt failed to decrypt");
    }
    return {unique_ptr<unsigned char[]>(buffer), ret};
}

RsaCipher::~RsaCipher() {
    if (privateKey != nullptr) {
        RSA_free(privateKey);
    }
    if (publicKey != nullptr) {
        RSA_free(publicKey);
    }
}

pair<string, string> RsaCipher::generateRsaKeyPair(int keyLength) {
    BIGNUM* bne = BN_new();
    BN_set_word(bne, RSA_F4);

    RSA* keypair = RSA_new();
    RSA_generate_key_ex(keypair, keyLength, bne, NULL);

    BIO* pri = BIO_new(BIO_s_mem());
    BIO* pub = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
    PEM_write_bio_RSAPublicKey(pub, keypair);

    int privateKeyLen = BIO_pending(pri);
    int publicKeyLen = BIO_pending(pub);

    auto privateKeyBuffer = new char[privateKeyLen + 1];
    auto publicKeyBuffer = new char[publicKeyLen + 1];

    BIO_read(pri, privateKeyBuffer, privateKeyLen);
    BIO_read(pub, publicKeyBuffer, publicKeyLen);

    privateKeyBuffer[privateKeyLen] = '\0';
    publicKeyBuffer[publicKeyLen] = '\0';

    string priKeyString = string(privateKeyBuffer);
    string pubKeyString = string(publicKeyBuffer);
    delete[] privateKeyBuffer;
    delete[] publicKeyBuffer;
    return {pubKeyString, priKeyString};
}
