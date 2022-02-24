#include "Md5Hasher.h"

#include <iostream>
using namespace std;
Md5Hasher::Md5Hasher() { MD5_Init(&ctx); }

void Md5Hasher::input(const void* data, unsigned int length) { MD5_Update(&ctx, data, length); }
unique_ptr<unsigned char[]> Md5Hasher::getResult() {
    unsigned char* tmp = new unsigned char[16];
    MD5_Final(tmp, &ctx);
    return unique_ptr<unsigned char[]>(tmp);
}

std::unique_ptr<unsigned char[]> Md5Hasher::hash(const void* data, unsigned int length) {
    unsigned char* tmp = new unsigned char[16];
    MD5((const unsigned char*)data, length, tmp);
    return unique_ptr<unsigned char[]>(tmp);
}
