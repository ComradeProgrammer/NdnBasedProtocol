#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "Aes.h"
using namespace std;

class AesTest : public ::testing::Test {
   protected:
};

TEST_F(AesTest, testEncodeAndDecode) {
    char userkey[16];
    for (int i = 0; i < 16; i++) {
        userkey[i] = rand() % 0xff;
    }

    // 128 bytes data
    char* data = new char[128];
    for (int i = 0; i < 128; i++) {
        data[i] = rand() % 0xff;
    }

    Aes a;
    a.setKey(userkey, 16);

    auto encoded = a.encrypt(data, 128);
    auto decoded = a.decrypt((const char*)(encoded.first.get()), encoded.second);

    for (int i = 0; i < 128; i++) {
        ASSERT_EQ((unsigned char)(data[i]), decoded.first[i]);
    }

    auto encoded2 = a.encrypt(data, 127);
    auto decoded2 = a.decrypt((const char*)(encoded2.first.get()), encoded2.second);

    for (int i = 0; i < 127; i++) {
        ASSERT_EQ((unsigned char)(data[i]), decoded2.first[i]);
    }

    auto encoded3 = a.encrypt(data, 95);
    auto decoded3 = a.decrypt((const char*)(encoded3.first.get()), encoded3.second);

    for (int i = 0; i < 95; i++) {
        ASSERT_EQ((unsigned char)(data[i]), decoded3.first[i]);
    }

    delete[] data;
}


TEST_F(AesTest, testEncodeAndDecode2) {
    char userkey[16];
    for (int i = 0; i < 16; i++) {
        userkey[i] = rand() % 0xff;
    }

    // 128 bytes data
    char* data = new char[1284];
    for (int i = 0; i < 1284; i++) {
        data[i] = rand() % 0xff;
    }

    Aes a;
    a.setKey(userkey, 16);

    auto encoded = a.encrypt(data, 1284);
    auto decoded = a.decrypt((const char*)(encoded.first.get()), encoded.second);

    for (int i = 0; i < 1284; i++) {
        ASSERT_EQ((unsigned char)(data[i]), decoded.first[i]);
    }

   
    delete[] data;
}