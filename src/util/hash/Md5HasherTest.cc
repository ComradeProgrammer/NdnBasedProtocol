#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "Md5Hasher.h"
class Md5HasherTest : public ::testing::Test {
   protected:
};

TEST_F(Md5HasherTest, testMd5Hash) {
    Md5Hasher tmp;
    char str[] = "abdefg1234567890";
    tmp.input(str, 16);

    unsigned char res[16] = {0xe3, 0x80, 0xe8, 0x8e, 0x8d, 0x09, 0xeb, 0xf8, 0xd8, 0x65, 0x9a, 0x15, 0xb0, 0xea, 0x70, 0xb5};
    auto res2 = tmp.getResult();
    for (int i = 0; i < 16; i++) {
        ASSERT_EQ(res[i], res2[i]);
    }
}
TEST_F(Md5HasherTest, testMd5Hash2) {
    Md5Hasher tmp;
    char str1[] = "abdefg";
    char str2[] = "1234567890";

    tmp.input(str1, 6);
    tmp.input(str2, 10);

    unsigned char res[16] = {0xe3, 0x80, 0xe8, 0x8e, 0x8d, 0x09, 0xeb, 0xf8, 0xd8, 0x65, 0x9a, 0x15, 0xb0, 0xea, 0x70, 0xb5};
    auto res2 = tmp.getResult();
    for (int i = 0; i < 16; i++) {
        ASSERT_EQ(res[i], res2[i]);
    }
}
TEST_F(Md5HasherTest, testMd5Hash3) {
    char str[] = "abdefg1234567890";

    unsigned char res[16] = {0xe3, 0x80, 0xe8, 0x8e, 0x8d, 0x09, 0xeb, 0xf8, 0xd8, 0x65, 0x9a, 0x15, 0xb0, 0xea, 0x70, 0xb5};
    auto res2 = Md5Hasher::hash(str, 16);
    for (int i = 0; i < 16; i++) {
        ASSERT_EQ(res[i], res2[i]);
    }
}