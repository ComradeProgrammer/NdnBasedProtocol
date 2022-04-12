
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#include <iostream>
#include <string>
#define KEY_LENGTH 2048
using namespace std;

int pkcs7_encode(const uint8_t *in, uint8_t *out, int inlen, const int enc) {
    // uint8_t padchr[16] = {0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
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

int main() {
    unsigned char userkey[16];
    string str = "Hello Worldddd!";
    for (int i = 0; i < 16; i++) {
        userkey[i] = rand() % 0xff;
    }
    AES_KEY key;
    AES_set_encrypt_key(userkey, 128, &key);
    unsigned char tmp[32];
    auto t = pkcs7_encode((const uint8_t *)(str.c_str()), tmp, 16, AES_ENCRYPT);
    cout << t << endl;
    unsigned char encoded[32];
    AES_ecb_encrypt(tmp, encoded, &key, AES_ENCRYPT);

    AES_KEY key2;
    AES_set_decrypt_key(userkey, 128, &key2);

    unsigned char tmp2[32];
    AES_ecb_encrypt(encoded, tmp2, &key2, AES_DECRYPT);

    char res[32];
    int len = pkcs7_encode((const uint8_t *)(tmp2), (uint8_t *)res, 16, AES_DECRYPT);
    cout << res << endl;
    cout << len << endl;
    return 0;
}