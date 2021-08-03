#ifndef __TLVOBJECT_H_
#define __TLVOBJECT_H_
#include<cstring>
#include<string>
#include<arpa/inet.h>
#include<unordered_map>
#include<sstream>
#include"util/declaration.h"
#include"util/log/Logger.h"
#include"util/util.h"
class TlvObject{
    public:
    TlvObject(uint64_t type,uint64_t length,const char* _buffer);
    //TlvObject(char* rawData, int length);
    TlvObject(uint64_t _type,uint32_t data);
    TlvObject(uint64_t _type,uint16_t data);
    TlvObject(uint64_t _type,std::string data);

    ~TlvObject();   
    TlvObject(const TlvObject& obj);
    TlvObject& operator=(const TlvObject& obj);

    std::pair<int,const char*> encode()const{return {currentPos-buffer,buffer};}

    static const TlvObject decode(char* rawData);
    int getRawDataLength()const{return currentPos-buffer;}
    uint64_t getType()const{return type;}
    uint64_t getLength()const{return length;}

    uint32_t parseUInt32()const;
    uint16_t parseUInt16()const;
    std::string parseString()const;
    const char* getData()const;



    private:
    uint64_t type;
    uint64_t length;
    char* buffer=nullptr;
    int bufferSize;
    char* currentPos;

    void writeTypeOrLength(uint64_t data);
    void write(const char* data, int length);
    void write(unsigned char c);

    static int getTLLength(uint64_t tl);
};

#endif