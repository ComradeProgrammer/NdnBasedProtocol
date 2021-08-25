#include"NdnInterest.h"
using namespace std;

vector<TlvObject>NdnInterest::encodeIntoTlvObjectArray(){
    vector<TlvObject>res;
    //encode name
    res.push_back(TlvObject(TLV_NAME,name));
   
    //encode mustBeFresh
    if(canBePrefix){
        res.push_back(TlvObject(TLV_CANBEPREFIX));
    }
    //encode mustBeFresh
    if(mustBeFresh){
        res.push_back(TlvObject(TLV_MUSTBEFRESHED));
    }
    //encode nonce
    res.push_back(TlvObject(TLV_NONCE,nonce));
    //encode hopLimit
    if(!hoplimitOmitted){
        res.push_back(TlvObject(TLV_HOPLIMIT,uint8_t(hopLimit)));
    }
    return res;
}

pair<int,std::unique_ptr<char[]>> NdnInterest::encode(){
    int bufferLength=0;
    //here we didn't use the list object directly to avoid allocating a buffer of MTU
    vector<TlvObject>tlvObjects=encodeIntoTlvObjectArray();
    //calculate the size of these datas;
    for(int i=0;i<tlvObjects.size();i++){
        bufferLength+=tlvObjects[i].getRawDataLength();
    }
    TlvObjectList objectList(bufferLength,logger);
    for(int i=0;i<tlvObjects.size();i++){
        objectList.insertObject(tlvObjects[i]);
    }

    auto objectListEncodePair=objectList.encode();
    TlvObject packetObject(
        TLV_INTEREST,
        objectListEncodePair.first,
        objectListEncodePair.second
    );

    auto packetObjectEncodePair=packetObject.encode();

    char* data=new char[packetObjectEncodePair.first];
    memcpy(data,packetObjectEncodePair.second,packetObjectEncodePair.first);

    return {
        packetObjectEncodePair.first,
        unique_ptr<char[]>(data)
    };
}

shared_ptr<NdnInterest>NdnInterest::decode(const char* data,std::shared_ptr<Logger> _logger){
    auto logger = Logger::getDefaultLoggerIfNull(_logger);
    const TlvObject packetObject=TlvObject::decode(data);
    if(packetObject.getType()!=TLV_INTEREST){
        logger->ERROR("NdnInterest::decode: not interest packet");
        return nullptr;
    }
    auto innerData=packetObject.parseData();
    auto innerDataLength=packetObject.getLength();

    TlvObjectList listObject(innerData,innerDataLength,logger);
    auto list=listObject.decode();
    auto interest=make_shared<NdnInterest>(logger);
    for(auto ptr:list){
        switch(ptr->getType()){
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
                interest->setHopLimit(true,ptr->parseUInt8());
                break;
            default:
                logger->WARNING("NdnInterest::decode: unrecognized type "+to_string(ptr->getType()));
        }
    }
    return interest;

}