#ifndef __TLVOBJECTLIST_H_
#define __TLVOBJECTLIST_H_
#include <arpa/inet.h>

#include <cstring>
#include <string>
#include <vector>

#include "TlvObject.h"
#include "util/declaration.h"
#include "util/log/Logger.h"
#include "util/util.h"
class TlvObjectList {
   public:
    TlvObjectList(int _bufferSize = MTU, std::shared_ptr<Logger> log = nullptr);
    TlvObjectList(const char* rawData, int length,
                  std::shared_ptr<Logger> log = nullptr);
    TlvObjectList(const TlvObjectList& obj);
    TlvObjectList& operator=(const TlvObjectList& obj);
    ~TlvObjectList();

    void insertObject(uint64_t type);
    void insertObject(uint64_t type, std::string obj);
    void insertObject(const TlvObject& obj);

    std::pair<int, const char*> encode() const {
        return {currentPos - buffer, buffer};
    }

    std::vector<std::shared_ptr<TlvObject>> decode();

   private:
    int bufferSize;
    char* buffer = nullptr;
    char* currentPos = nullptr;
    std::shared_ptr<Logger> logger;

    void writeTypeOrLength(uint64_t data);
    void write(const char* data, int length);
    void write(unsigned char c);
};
#endif