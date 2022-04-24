#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "Md5RsaSignatureFactory.h"
using namespace std;
class Md5RsaSignatureFactoryTest : public ::testing::Test {
   protected:
};

TEST_F(Md5RsaSignatureFactoryTest, testMd5RsaSignature) {
    const char* tmp = "Glory be to Ukraine";

    auto keyPair = RsaCipher::generateRsaKeyPair(1024);

    auto cipherDecode = make_shared<Md5RsaSignatureFactory>();
    cipherDecode->loadPublicKey(keyPair.first);
    cipherDecode->input(tmp, 20);

    auto cipherEncode = make_shared<Md5RsaSignatureFactory>();
    cipherEncode->loadPrivateKey(keyPair.second);
    cipherEncode->input(tmp, 20);

    auto sig = cipherEncode->generateSignature();
    bool ok = cipherDecode->verifySignature(sig.first.get(), sig.second);
    ASSERT_TRUE(ok);
}

TEST_F(Md5RsaSignatureFactoryTest, testMd5RsaSignature2) {
    const char* tmp = "Glory be to Ukraine";
    const char* tmp2 = "Glory be to Ukraine!";
    auto keyPair = RsaCipher::generateRsaKeyPair(1024);

    auto cipherDecode = make_shared<Md5RsaSignatureFactory>();
    cipherDecode->loadPublicKey(keyPair.first);
    cipherDecode->input(tmp, 20);

    auto cipherEncode = make_shared<Md5RsaSignatureFactory>();
    cipherEncode->loadPrivateKey(keyPair.second);
    cipherEncode->input(tmp2, 20);

    auto sig = cipherEncode->generateSignature();
    bool ok = cipherDecode->verifySignature(sig.first.get(), sig.second);
    ASSERT_FALSE(ok);
}