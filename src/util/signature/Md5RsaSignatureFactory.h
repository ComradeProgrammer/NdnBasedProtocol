#ifndef __MD5_RSA_SIGNATURE_FACTORY_H_
#define __MD5_RSA_SIGNATURE_FACTORY_H_

#include"SignatureAbstractFactory.h"
#include"util/hash/Md5Hasher.h"
#include"util/cipher/rsa/RsaCipher.h"
//the signature will be 128 bytes. 
class Md5RsaSignatureFactory : public SignatureAbstractFactory{
    public:
    Md5RsaSignatureFactory(){
        hasher=std::make_shared<Md5Hasher>();
        cipher=std::make_shared<RsaCipher>();
    }
};
#endif