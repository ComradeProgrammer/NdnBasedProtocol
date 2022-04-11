#include "SignatureAbstractFactory.h"
#include"ioc.h"
using namespace std;

void SignatureAbstractFactory::input(const void* data, unsigned int length) { hasher->input(data, length); }

void SignatureAbstractFactory::loadPrivateKey(string privateKeyString) { cipher->loadPrivateKey(privateKeyString); }

void SignatureAbstractFactory::loadPublicKey(string publicKeyString) { cipher->loadPublicKey(publicKeyString); }

pair<unique_ptr<unsigned char[]>, int> SignatureAbstractFactory::generateSignature() {
    auto hash = hasher->getResult();

    auto signature = cipher->encrypt(hash.first.get(), hash.second);
    return signature;
}
bool SignatureAbstractFactory::verifySignature(const unsigned char* signature, int len){
    //LOGGER->VERBOSEF("%x %d",signature,len);
    auto hash = hasher->getResult();
    auto decryptedHashPair=cipher->decrypte(signature,len);
    return dataEqual(std::move(hash),std::move(decryptedHashPair));
}
bool dataEqual(pair<unique_ptr<unsigned char[]>, int> s1, pair<unique_ptr<unsigned char[]>, int> s2) {
    if (s1.second != s2.second) {
        return false;
    }
    for (int i = 0; i < s1.second; i++) {
        if (s1.first[i] != s2.first[i]) {
            return false;
        }
    }
    return true;
}