#include "TlvObjectList.h"

using namespace std;
TlvObjectList::TlvObjectList(int _bufferSize, std::shared_ptr<Logger> log)
    : bufferSize(_bufferSize) {
    logger = Logger::getDefaultLoggerIfNull(log);
    buffer = new char[_bufferSize];
    currentPos = buffer;
}

TlvObjectList::TlvObjectList(char* rawData, int length,std::shared_ptr<Logger> log){
    logger = Logger::getDefaultLoggerIfNull(log);
    buffer=new char[length];
    currentPos=buffer+length;
    memcpy(buffer,rawData,length);
}


TlvObjectList::TlvObjectList(const TlvObjectList& obj) {
    logger = obj.logger;
    currentPos = obj.currentPos;
    bufferSize = obj.bufferSize;
    buffer = new char[obj.bufferSize];
    memcpy(buffer, obj.buffer, bufferSize);
}

TlvObjectList& TlvObjectList::operator=(const TlvObjectList& obj) {
    logger = obj.logger;
    currentPos = obj.currentPos;
    bufferSize = obj.bufferSize;
    delete buffer;
    buffer = new char[obj.bufferSize];
    memcpy(buffer, obj.buffer, bufferSize);
}

TlvObjectList::~TlvObjectList() { delete[] buffer; }

void TlvObjectList::write(const char* data, int length) {
    if (currentPos + length > buffer + bufferSize) {
        logger->ERROR("TlvObject::write: writing position exceeds.");
        return;
    }
    memcpy(currentPos, data, length);
    currentPos += length;
}

void TlvObjectList::write(unsigned char c) {
    if (currentPos >= buffer + bufferSize) {
        logger->ERROR("TlvObject::write: writing position exceeds.");
        return;
    }
    *(currentPos++) = c;
}

void TlvObjectList::writeTypeOrLength(uint64_t data) {
    if (data <= 0xfc) {
        write(static_cast<unsigned char>(data));
        return;
    } else if (data > 0xfc && data <= 0xffff) {
        write(0xfd);
        uint16_t tmp = htons(static_cast<uint16_t>(data));
        write(reinterpret_cast<char*>(&tmp), 2);
        return;
    } else if (data > 0xffff && data < 0xffffffff) {
        write(0xfe);
        uint32_t tmp = htonl(static_cast<uint32_t>(data));
        write(reinterpret_cast<char*>(&tmp), 4);
        return;
    } else {
        // if data is greater tha  0xffffffff
        write(0xff);
        uint64_t tmp = htonll(data);
        write(reinterpret_cast<char*>(&tmp), 8);
        return;
    }
}

void TlvObjectList::insertObject(uint64_t type) {
    writeTypeOrLength(type);
    writeTypeOrLength(0);
}

void TlvObjectList::insertObject(uint64_t type, std::string obj) {
    TlvObject tlvObj(type, obj.size(), obj.c_str());
    auto encodePair = tlvObj.encode();
    write(encodePair.second, encodePair.first);
}

void TlvObjectList::insertObject(const TlvObject& obj) {
    auto encodePair = obj.encode();
    write(encodePair.second, encodePair.first);
}


vector<shared_ptr<TlvObject>>TlvObjectList::decode(){
    vector<shared_ptr<TlvObject>>res;
    char* ptr=buffer;
    while(1){
        auto obj=TlvObject::decode(ptr);
        int rawDataLength=obj.getRawDataLength();
        ptr+=rawDataLength;
        res.push_back(make_shared<TlvObject>(obj));
        if(ptr==currentPos){
            return res;
        }else if(ptr>currentPos){
            logger->ERROR("TlvObjectList::decode: broken buffer");
            return res;
        }
    }

}