#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "RsaCipher.h"
using namespace std;
class RsaCipherTest : public ::testing::Test {
   protected:
};

TEST_F(RsaCipherTest, testEncryptAndDecrypt){

   auto keyPair=RsaCipher::generateRsaKeyPair(1024);

   auto cipherDecode=make_shared<RsaCipher>();
   cipherDecode->loadPublicKey(keyPair.first);

   auto cipherEncode=make_shared<RsaCipher>();
   cipherEncode->loadPrivateKey(keyPair.second);

   string testContent="hello world!";
   auto tmp=cipherEncode->encrypt((const unsigned char *)testContent.c_str(),13);


   auto res=cipherDecode->decrypte(tmp.first.get(),tmp.second);
   ASSERT_EQ(testContent,string((const char*)res.first.get()));

} 