#include "NdnInterest.h"
using namespace std;
using json = nlohmann::json;
NdnInterest::~NdnInterest() { delete[] applicationParameters; }

NdnInterest::NdnInterest(const NdnInterest& old) : NdnPacket(old) {
    canBePrefix = old.canBePrefix;
    mustBeFresh = old.mustBeFresh;
    nonce = old.nonce;
    hopLimit = old.hopLimit;
    hoplimitOmitted = old.hoplimitOmitted;
    applicationParametersSize = old.applicationParametersSize;
    applicationParameters = new char[applicationParametersSize];
    memcpy(applicationParameters, old.applicationParameters,
           applicationParametersSize);
    logger = old.logger;
}

NdnInterest& NdnInterest::operator=(const NdnInterest& old) {
    NdnPacket::operator=(old);
    delete[] applicationParameters;
    canBePrefix = old.canBePrefix;
    mustBeFresh = old.mustBeFresh;
    nonce = old.nonce;
    hopLimit = old.hopLimit;
    hoplimitOmitted = old.hoplimitOmitted;
    applicationParametersSize = old.applicationParametersSize;
    applicationParameters = new char[applicationParametersSize];
    memcpy(applicationParameters, old.applicationParameters,
           applicationParametersSize);
    logger = old.logger;
    return *this;
}

vector<TlvObject> NdnInterest::encodeIntoTlvObjectArray() {
    vector<TlvObject> res;
    // encode name
    res.push_back(TlvObject(TLV_NAME, name));

    // encode mustBeFresh
    if (canBePrefix) {
        res.push_back(TlvObject(TLV_CANBEPREFIX));
    }
    // encode mustBeFresh
    if (mustBeFresh) {
        res.push_back(TlvObject(TLV_MUSTBEFRESHED));
    }
    // encode nonce
    res.push_back(TlvObject(TLV_NONCE, nonce));
    // encode hopLimit
    if (!hoplimitOmitted) {
        res.push_back(TlvObject(TLV_HOPLIMIT, uint8_t(hopLimit)));
    }
    // encode applicationParameters
    if (applicationParametersSize != 0) {
        res.push_back(TlvObject(TLV_APLICATIONPARAMETERS,
                                applicationParametersSize,
                                applicationParameters));
    }
    return res;
}

pair<int, std::unique_ptr<char[]>> NdnInterest::encode() {
    int bufferLength = 0;
    // here we didn't use the list object directly to avoid allocating a buffer
    // of MTU
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
    TlvObject packetObject(TLV_INTEREST, objectListEncodePair.first,
                           objectListEncodePair.second);

    auto packetObjectEncodePair = packetObject.encode();

    // make a copy of the data and return
    char* data = new char[packetObjectEncodePair.first];
    memcpy(data, packetObjectEncodePair.second, packetObjectEncodePair.first);

    return {packetObjectEncodePair.first, unique_ptr<char[]>(data)};
}

shared_ptr<NdnInterest> NdnInterest::decode(const char* data,
                                            std::shared_ptr<Logger> _logger) {
    auto logger = Logger::getDefaultLoggerIfNull(_logger);
    const TlvObject packetObject = TlvObject::decode(data);
    if (packetObject.getType() != TLV_INTEREST) {
        logger->ERROR("NdnInterest::decode: not interest packet");
        return nullptr;
    }
    auto innerData = packetObject.parseData();
    auto innerDataLength = packetObject.getLength();

    TlvObjectList listObject(innerData, innerDataLength, logger);
    auto list = listObject.decode();
    auto interest = make_shared<NdnInterest>(logger);
    for (auto ptr : list) {
        switch (ptr->getType()) {
            case TLV_NAME:
                interest->setName(ptr->parseString());
                break;
            case TLV_CANBEPREFIX:
                interest->setCanBePrefix(true);
                break;
            case TLV_MUSTBEFRESHED:
                interest->setMustBeFresh(true);
                break;
            case TLV_NONCE:
                interest->setNonce(ptr->parseUInt32());
                break;
            case TLV_HOPLIMIT:
                interest->setHopLimit(false, ptr->parseUInt8());
                break;
            case TLV_APLICATIONPARAMETERS:
                interest->setApplicationParameters(ptr->getLength(),
                                                   ptr->parseData());
                break;
            default:
                logger->WARNING("NdnInterest::decode: unrecognized type " +
                                to_string(ptr->getType()));
        }
    }
    return interest;
}

pair<int, std::unique_ptr<char[]>> NdnInterest::getApplicationParameters() {
    char* tmp = new char[applicationParametersSize];
    if (applicationParametersSize != 0) {
        memcpy(tmp, applicationParameters, applicationParametersSize);
    }
    return {applicationParametersSize, unique_ptr<char[]>(tmp)};
}

void NdnInterest::setApplicationParameters(int length, const char* data) {
    delete applicationParameters;
    applicationParametersSize = length;
    applicationParameters = new char[length];
    memcpy(applicationParameters, data, length);
}

string NdnInterest::toString() {
    json j;
    j["name"] = name;
    j["packetType"] = packetType;
    j["canBePrefix"] = canBePrefix;
    j["mustBeFresh"] = mustBeFresh;
    j["nonce"] = nonce;
    if (!hoplimitOmitted) {
        j["hopLimit"] = hopLimit;
    }
    j["applicationParameters"] =
        "data of " + to_string(applicationParametersSize) + " bytes";
    return j.dump();
}