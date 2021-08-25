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
    // @brief create a list object containing an empty buffer in pre-assigned size
    TlvObjectList(int _bufferSize = MTU, std::shared_ptr<Logger> log = nullptr);
    //@brief convert a binary data of tlv-format object list into  TlvObjectList
    //This rawData will not be modified,saved or deleted. Instead, the constructor will alloc a new memory persist a copy of this data.
    TlvObjectList(const char* rawData, int length,
                  std::shared_ptr<Logger> log = nullptr);
    //copy constructor
    TlvObjectList(const TlvObjectList& obj);

    //copy assignment operator
    TlvObjectList& operator=(const TlvObjectList& obj);

    //deconstructor
    ~TlvObjectList();

    void insertObject(uint64_t type);
    void insertObject(uint64_t type, std::string obj);
    void insertObject(const TlvObject& obj);

    /**
     * @brief encode the current object into tlv-format binary data list
     * @return pair<int, const char*>, 1st value: lenght of data, 2nd value: binary data. 
     * The data returned is actually stored in this object, thus it will be deleted if this object is deconstructed. If you want to persist this data, make sure to make a copy! 
    */
    std::pair<int, const char*> encode() const {
        return {currentPos - buffer, buffer};
    }

    //convert the binary data stored in this object to as vector containing lots of TlvObject
    std::vector<std::shared_ptr<TlvObject>> decode();

   private:
    int bufferSize;
    char* buffer = nullptr;
    char* currentPos = nullptr;
    std::shared_ptr<Logger> logger;

    //write data in to buffer and increase the currentPos
    void writeTypeOrLength(uint64_t data);
    //write data in to buffer and increase the currentPos. data will be copied but not modified or shallow saved
    void write(const char* data, int length);
    //write byte in to buffer and increase the currentPos
    void write(unsigned char c);
};
#endif