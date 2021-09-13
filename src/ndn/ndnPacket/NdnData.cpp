#include "NdnData.h"
using namespace std;
using json = nlohmann::json;
NdnData::~NdnData() { delete[] content; }

NdnData::NdnData(const NdnData& data) :NdnPacket(data){
    logger = data.logger;
    contentSize = data.contentSize;
    content = new char[data.contentSize];
    if (data.contentSize != 0) {
        memcpy(content, data.content, data.contentSize);
    }
}

NdnData& NdnData::operator=(const NdnData& data) {
    NdnPacket::operator=(data);
    logger = data.logger;
    contentSize = data.contentSize;
    delete[] content;
    content = new char[data.contentSize];
    if (data.contentSize != 0) {
        memcpy(content, data.content, data.contentSize);
    }
    return *this;
}
pair<int, unique_ptr<char[]>> NdnData::getContent() {
    char* tmp = new char[contentSize];
    if (contentSize != 0) {
        memcpy(tmp, content, contentSize);
    }
    return {contentSize, unique_ptr<char[]>(tmp)};
}

void NdnData::setContent(int length, const char* _content) {
    contentSize = length;
    delete[] content;
    content = new char[contentSize];
    if (contentSize != 0) {
        memcpy(content, _content, contentSize);
    }
}

pair<int, unique_ptr<char[]>> NdnData::encode() {
    int bufferLength = 0;
    // here we didn't use the list object directly to avoid allocating a buffer
    // of MTU size
    vector<TlvObject> tlvObjects = encodeIntoTlvObjectArray();
    // calculate the size of these datas;
    for (int i = 0; i < tlvObjects.size(); i++) {
        bufferLength += tlvObjects[i].getRawDataLength();
    }
    TlvObjectList objectList(bufferLength, logger);
    for (int i = 0; i < tlvObjects.size(); i++) {
        objectList.insertObject(tlvObjects[i]);
    }
    auto objectListEncodePair = objectList.encode();

    TlvObject packetObject(TLV_DATA, objectListEncodePair.first,
                           objectListEncodePair.second);
    auto packetObjectEncodePair = packetObject.encode();

    // make a copy of the data and return
    char* data = new char[packetObjectEncodePair.first];
    memcpy(data, packetObjectEncodePair.second, packetObjectEncodePair.first);

    return {packetObjectEncodePair.first, unique_ptr<char[]>(data)};
}

vector<TlvObject> NdnData::encodeIntoTlvObjectArray() {
    vector<TlvObject> res;
    // encode name
    res.push_back(TlvObject(TLV_NAME, name));
    // encode content
    if (contentSize != 0) {
        res.push_back(TlvObject(TLV_CONTENT, contentSize, content));
    }
    return res;
}

shared_ptr<NdnData> NdnData::decode(const char* data,
                                    std::shared_ptr<Logger> _logger) {
    auto logger = Logger::getDefaultLoggerIfNull(_logger);
    const TlvObject packetObject = TlvObject::decode(data);
    if (packetObject.getType() != TLV_DATA) {
        logger->ERROR("NdnData::decode: not interest packet");
        return nullptr;
    }
    auto innerData = packetObject.parseData();
    auto innerDataLength = packetObject.getLength();
    TlvObjectList listObject(innerData, innerDataLength, logger);
    auto list = listObject.decode();
    auto datapacket = make_shared<NdnData>(logger);
    for (auto ptr : list) {
        switch (ptr->getType()) {
            case TLV_NAME:
                datapacket->setName(ptr->parseString());
                break;
            case TLV_CONTENT:
                datapacket->setContent(ptr->getLength(), ptr->parseData());
                break;
            default:
                logger->WARNING("NdnData::decode: unrecognized type " +
                                to_string(ptr->getType()));
        }
    }
    return datapacket;
}

string NdnData::toString(){
    json j;
    j["name"]=name;
    j["packetType"]=packetType;
    j["content"]="data of "+to_string(contentSize)+" bytes";
    return j.dump();
}
