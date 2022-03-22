#ifndef __ASYMMETRIC_CIPHER_H_
#define __ASYMMETRIC_CIPHER_H_
#include<memory>
class AsymmetricCipher{
    public:
    virtual void loadPublicKey(std::string publicKeyString)=0;

    virtual void loadPrivateKey(std::string publicKeyString)=0;


    /**
     * @brief encrypt the data of specified length  with PRIVATE KEY
     * 
     * @param data data to be encrypted 
     * @param length lenth of data, Note: the length must be smaller than lenghth of key - 11
     */
    virtual std::pair<std::unique_ptr<unsigned char[]>,int>encrypt(const unsigned char* data, int length)=0;

    /**
     * @brief decrypt the data of specified length  with PUBLIC KEY
     * 
     * @param data data to be encrypted 
     * @param length lenth of data, Note: the length must be smaller than lenghth of key - 11
     */
    virtual std::pair<std::unique_ptr<unsigned char[]>,int>decrypte(const unsigned char* data, int length)=0; 

};
#endif