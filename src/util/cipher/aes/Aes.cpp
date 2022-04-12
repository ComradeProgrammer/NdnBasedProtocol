#include "Aes.h"
using namespace std;
Aes::Aes(const Aes& a) {
    if (a.key != nullptr) {
        key = new char[a.keyLength];
        memcpy(key, a.key, keyLength);
        keyLength = a.keyLength;
    }
}

Aes::~Aes() {
    if (key != nullptr) {
        delete[] key;
    }
}

void Aes::setKey(const char* _key, int _length) {
    Md5Hasher hasher;
    hasher.input(_key, _length);
    auto res = hasher.getResult();

    key = new char[16];
    memcpy(key, res.first.get(), 16);
    keyLength = 16;
}

pair<unique_ptr<unsigned char[]>, int> Aes::encrypt(const char* data, int length) {
    AES_KEY aeskey;
    AES_set_encrypt_key((unsigned char*)key, keyLength * 8, &aeskey);

    int encodedSize = length + 16 - length % 16;
    unsigned char* dataWithPadding = new unsigned char[encodedSize];
    pkcs7_encode((unsigned char*)data, dataWithPadding, length, AES_ENCRYPT);
    unsigned char* encoded = new unsigned char[encodedSize];
    for (int i = 0; i < encodedSize / 16; i++) {
        AES_ecb_encrypt(dataWithPadding + 16 * i, encoded + 16 * i, &aeskey, AES_ENCRYPT);
    }

    delete[] dataWithPadding;
    return {unique_ptr<unsigned char[]>(encoded), encodedSize};
}

pair<unique_ptr<unsigned char[]>, int> Aes::decrypt(const char* data, int length) {
    AES_KEY aeskey;
    AES_set_decrypt_key((unsigned char*)key, keyLength * 8, &aeskey);

    unsigned char* dataWithPadding = new unsigned char[length];
    for (int i = 0; i < length / 16; i++) {
        AES_ecb_encrypt((unsigned char*)(data + 16 * i), dataWithPadding + 16 * i, &aeskey, AES_DECRYPT);
    }

    unsigned char* res = new unsigned char[length];
    int len = pkcs7_encode(dataWithPadding, res, length, AES_DECRYPT);
    delete[] dataWithPadding;
    return {unique_ptr<unsigned char[]>(res), len};
}

int Aes::pkcs7_encode(const uint8_t* in, uint8_t* out, int inlen, const int enc) {
    int outlen;
    if (inlen <= 0) return 0;
    // 添加padding
    if (enc == AES_ENCRYPT) {
        outlen = inlen + 16 - inlen % 16;
        for (int i = 0; i < outlen; i++) {
            if (i < inlen)
                out[i] = in[i];
            else
                out[i] = 16 - inlen % 16;
        }
    }
    // 删除padding
    else if (enc == AES_DECRYPT) {
        outlen = inlen - (in[inlen - 1]);
        for (int i = 0; i < outlen; i++) {
            out[i] = in[i];
        }
    }
    return outlen;
}
