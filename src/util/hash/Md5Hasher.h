#ifndef __MD5_HASHER_H_
#define __MD5_HASHER_H_
#include <openssl/md5.h>
#include <memory>
#include "string.h"

#include"Hasher.h"
class Md5Hasher :public Hasher {
   public:
    Md5Hasher();
    Md5Hasher(const Md5Hasher& h) = delete;

    virtual void input(const void* data, unsigned int length);
    virtual std::unique_ptr<unsigned char[]> getResult();

    virtual std::unique_ptr<unsigned char[]> hash(const void* data, unsigned int length);

   private:
    MD5_CTX ctx;
};

#endif