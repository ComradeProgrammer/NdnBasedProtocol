#include "TlvObject.h"
using namespace std;
TlvObject::TlvObject(uint64_t _type, uint64_t _length, const char* _buffer) {
    type = _type;
    length = _length;
    bufferSize = length + 9 + 9;
    buffer = new char[bufferSize];
    currentPos = buffer;
    writeTypeOrLength(type);
    writeTypeOrLength(length);
    write(_buffer, int(length));
}

TlvObject::TlvObject(uint64_t _type, uint32_t data) {
    uint32_t tmp = htonl(data);
    new (this) TlvObject(_type, 4, reinterpret_cast<char*>(&tmp));
}

TlvObject::TlvObject(uint64_t _type, uint16_t data) {
    uint16_t tmp = htons(data);
    new (this) TlvObject(_type, 2, reinterpret_cast<char*>(&tmp));
}

TlvObject::TlvObject(uint64_t _type, string data) {
    new (this) TlvObject(_type, data.size(), data.c_str());
}

TlvObject::TlvObject(uint64_t _type, uint8_t data) {
    new (this) TlvObject(_type, 1, reinterpret_cast<char*>(&data));
}

TlvObject::TlvObject(uint64_t _type) {
    type = _type;
    length = 0;
    bufferSize = 18;
    buffer = new char[bufferSize];
    currentPos = buffer;
    writeTypeOrLength(type);
    writeTypeOrLength(length);
}

TlvObject::~TlvObject() { delete[] buffer; }

TlvObject::TlvObject(const TlvObject& obj) {
    type = obj.type;
    length = obj.length;
    bufferSize = obj.bufferSize;
    buffer = new char[bufferSize];
    int len = obj.currentPos - obj.buffer;
    memcpy(buffer, obj.buffer, len);
    currentPos = buffer + len;
}

TlvObject& TlvObject::operator=(const TlvObject& obj) {
    delete[] buffer;
    type = obj.type;
    length = obj.length;
    bufferSize = obj.bufferSize;
    buffer = new char[bufferSize];
    int len = obj.currentPos - obj.buffer;
    memcpy(buffer, obj.buffer, len);
    currentPos = buffer + len;
    return *this;
}

void TlvObject::write(const char* data, int length) {
    memcpy(currentPos, data, length);
    currentPos += length;
}

void TlvObject::write(unsigned char c) { *(currentPos++) = c; }

void TlvObject::writeTypeOrLength(uint64_t data) {
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
const TlvObject TlvObject::decode(const char* rawData) {
    int ptr = 0;
    // read type
    unsigned char typeMark = rawData[0];
    uint64_t type;
    ptr++;
    if (typeMark == 0xff) {
        uint64_t* typePtr = (uint64_t*)(rawData + ptr);
        type = ntohll(*typePtr);
        ptr += 8;
    } else if (typeMark == 0xfe) {
        uint32_t* typePtr = (uint32_t*)(rawData + ptr);
        type = ntohl(*typePtr);
        ptr += 4;
    } else if (typeMark == 0xfd) {
        uint16_t* typePtr = (uint16_t*)(rawData + ptr);
        type = ntohs(*typePtr);
        ptr += 2;
    } else {
        type = typeMark;
    }

    // read length
    unsigned char lengthMark = rawData[ptr];
    uint64_t length;
    ptr++;
    if (lengthMark == 0xff) {
        uint64_t* lengthPtr = (uint64_t*)(rawData + ptr);
        length = ntohll(*lengthPtr);
        ptr += 8;
    } else if (lengthMark == 0xfe) {
        uint32_t* lengthPtr = (uint32_t*)(rawData + ptr);
        length = ntohl(*lengthPtr);
        ptr += 4;
    } else if (lengthMark == 0xfd) {
        uint16_t* lengthPtr = (uint16_t*)(rawData + ptr);
        length = ntohs(*lengthPtr);
        ptr += 2;
    } else {
        length = lengthMark;
    }
    return TlvObject(type, length, rawData + ptr);
}

int TlvObject::getTLLength(uint64_t tl) {
    if (tl > 0xffffffff) {
        return 9;
    } else if (tl > 0xffff) {
        return 5;
    } else if (tl > 0xfc) {
        return 3;
    }
    return 1;
}

uint32_t TlvObject::parseUInt32() const {
    int tlLength = getTLLength(type) + getTLLength(length);
    uint32_t* ptr = (uint32_t*)(buffer + tlLength);
    return ntohl(*ptr);
}

uint16_t TlvObject::parseUInt16() const {
    int tlLength = getTLLength(type) + getTLLength(length);
    uint16_t* ptr = (uint16_t*)(buffer + tlLength);
    return ntohs(*ptr);
}

uint8_t TlvObject::parseUInt8() const {
    int tlLength = getTLLength(type) + getTLLength(length);
    uint8_t* ptr = (uint8_t*)(buffer + tlLength);
    return *ptr;
}
std::string TlvObject::parseString() const {
    stringstream ss;
    int tlLength = getTLLength(type) + getTLLength(length);
    for (int i = 0; i < length; i++) {
        ss << buffer[tlLength + i];
    }
    return ss.str();
}

const char* TlvObject::parseData() const {
    int tlLength = getTLLength(type) + getTLLength(length);
    return buffer + tlLength;
}
uint64_t TlvObject::peekType(const char* rawData) {
    int ptr = 0;
    // read type
    unsigned char typeMark = rawData[0];
    uint64_t type;
    ptr++;
    if (typeMark == 0xff) {
        uint64_t* typePtr = (uint64_t*)(rawData + ptr);
        type = ntohll(*typePtr);
        ptr += 8;
    } else if (typeMark == 0xfe) {
        uint32_t* typePtr = (uint32_t*)(rawData + ptr);
        type = ntohl(*typePtr);
        ptr += 4;
    } else if (typeMark == 0xfd) {
        uint16_t* typePtr = (uint16_t*)(rawData + ptr);
        type = ntohs(*typePtr);
        ptr += 2;
    } else {
        type = typeMark;
    }
    return typeMark;
}