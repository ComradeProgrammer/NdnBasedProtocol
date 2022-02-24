#ifndef __MD5_HASHER_H_
#define __MD5_HASHER_H_
#include <openssl/md5.h>

#include <memory>

#include "string.h"
class Md5Hasher {
   public:
    Md5Hasher();
    Md5Hasher(const Md5Hasher& h) = delete;

    void input(const void* data, unsigned int length);
    std::unique_ptr<unsigned char[]> getResult();

    static std::unique_ptr<unsigned char[]> hash(const void* data, unsigned int length);

   private:
    MD5_CTX ctx;
};

#endif