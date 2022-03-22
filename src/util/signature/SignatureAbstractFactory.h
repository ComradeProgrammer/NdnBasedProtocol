#ifndef __SIGNATURE_ABSTRACT_FACTORY_H_
#define __SIGNATURE_ABSTRACT_FACTORY_H_
#include <memory>

#include"util/hash/Hasher.h"
#include"util/cipher/AsymmetricCipher.h"
class SignatureAbstractFactory {
    public:

    virtual void input(const void* data, unsigned int length);
    virtual void loadPrivateKey(std::string privateKeyString);
    virtual void loadPublicKey(std::string publicKeyString);
    virtual std::pair<std::unique_ptr<unsigned char[]>, int> generateSignature();

    virtual bool verifySignature(std::unique_ptr<unsigned char[]>, int);

    protected:
    std::shared_ptr<Hasher>hasher=nullptr;
    std::shared_ptr<AsymmetricCipher>cipher=nullptr;
    SignatureAbstractFactory(){}
};

bool dataEqual(std::pair<std::unique_ptr<unsigned char[]>, int>  s1,std::pair<std::unique_ptr<unsigned char[]>, int> s2);

#endif