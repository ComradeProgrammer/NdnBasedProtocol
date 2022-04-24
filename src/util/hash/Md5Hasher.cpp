#include "Md5Hasher.h"

#include <iostream>
using namespace std;
Md5Hasher::Md5Hasher() { MD5_Init(&ctx); }

void Md5Hasher::input(const void* data, unsigned int length) { MD5_Update(&ctx, data, length); }

std::pair<std::unique_ptr<unsigned char[]>, int> Md5Hasher::getResult() {
    unsigned char* tmp = new unsigned char[16];
    MD5_Final(tmp, &ctx);
    return {unique_ptr<unsigned char[]>(tmp), 16};
}

std::pair<std::unique_ptr<unsigned char[]>, int> Md5Hasher::hash(const void* data, unsigned int length) {
    unsigned char* tmp = new unsigned char[16];
    MD5((const unsigned char*)data, length, tmp);
    return {unique_ptr<unsigned char[]>(tmp), 16};
}
