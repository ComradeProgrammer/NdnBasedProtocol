#ifndef __TLVOBJECT_H_
#define __TLVOBJECT_H_
#include <arpa/inet.h>

#include <cstring>
#include <sstream>
#include <string>
#include <unordered_map>

#include "ioc.h"
#include "util/declaration.h"
#include "util/util.h"
class TlvObject {
   public:
    /**
     * @brief create a tlv object with specified type and binary data
     * @param _type type Number of tlv object, usually declared in
     * util/declaration.h
     * @param length length of binary data
     * @param _buffer buffer containing data. This buffer will not be
     * modified,saved or deleted. Instead, the constructor will alloc a new
     * memory persist a copy of this data.
     */
    TlvObject(uint64_t _type, uint64_t length, const char* _buffer);

    //@brief create a tlv object with a 32-bit unsigned int data
    TlvObject(uint64_t _type, uint32_t data);

    //@brief create a tlv object with a 16-bit unsigned int data
    TlvObject(uint64_t _type, uint16_t data);
    //@brief create a tlv object with a 8-bit unsigned int data
    TlvObject(uint64_t _type, uint8_t data);

    //@brief create a tlv object with a string
    TlvObject(uint64_t _type, std::string data);

    //@brief create a tlv object with no data
    TlvObject(uint64_t _type);

    // deconstructor
    virtual ~TlvObject();

    // copy constructor
    TlvObject(const TlvObject& obj);

    // copy assignment operator
    TlvObject& operator=(const TlvObject& obj);

    /**
     * @brief encode the current object into tlv-format binary data
     * @return pair<int, const char*>, 1st value: lenght of data, 2nd value:
     * binary data. The data returned is actually stored in this object, thus it
     * will be deleted if this object is deconstructed. If you want to persist
     * this data, make sure to make a copy!
     */
    std::pair<int, const char*> encode() const { return {currentPos - buffer, buffer}; }
    /**
     * @brief convert tlv-format binary data into a tlv data and return it.
     * @param rawData buffer containing data. This buffer will not be
     * modified,saved or deleted. Instead, the constructor will alloc a new
     * memory persist a copy of this data.
     */
    static const TlvObject decode(const char* rawData);
    // return the actual lenth of the WHOLE BINARY data.
    int getRawDataLength() const { return currentPos - buffer; }

    static uint64_t peekType(const char* rawData);
    // return the type of tlv object
    uint64_t getType() const { return type; }
    // return the length of tlv object, NOT THE LENGTH OF BINARY DATA, but the
    // 'L' in "TLV"
    uint64_t getLength() const { return length; }

    uint32_t parseUInt32() const;
    uint16_t parseUInt16() const;
    uint8_t parseUInt8() const;
    std::string parseString() const;
    const char* parseData() const;

   private:
    uint64_t type;
    uint64_t length;
    char* buffer = nullptr;
    int bufferSize;
    char* currentPos;

    void writeTypeOrLength(uint64_t data);
    void write(const char* data, int length);
    void write(unsigned char c);

    static int getTLLength(uint64_t tl);
};

#endif