#ifndef __MD5_HASHER_H_
#define __MD5_HASHER_H_
#include <openssl/md5.h>

#include <memory>

#include "Hasher.h"
#include "string.h"
class Md5Hasher : public Hasher {
   public:
    Md5Hasher();
    Md5Hasher(const Md5Hasher& h) = delete;

    virtual void input(const void* data, unsigned int length);
    virtual std::pair<std::unique_ptr<unsigned char[]>, int> getResult();

    virtual std::pair<std::unique_ptr<unsigned char[]>, int> hash(const void* data, unsigned int length);

   private:
    MD5_CTX ctx;
};

#endif