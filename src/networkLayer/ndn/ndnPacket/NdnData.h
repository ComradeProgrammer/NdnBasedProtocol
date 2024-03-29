#ifndef __NDNDATA_H_
#define __NDNDATA_H_
#include <memory>
#include <string>

#include "NdnPacket.h"
#include "ioc.h"
#include "util/json/json.hpp"
class NdnData : public NdnPacket {
   public:
    // default constructor
    NdnData() { setPacketType(TLV_DATA); };
    // default constructor
    virtual ~NdnData();
    // copy constructor of NdnData
    NdnData(const NdnData& data);
    // copy assignment operator
    NdnData& operator=(const NdnData& data);

    // getter of the attribute 'content'. return a copy of the content data. 1st
    // value of pair is actual length of data;
    std::pair<int, std::unique_ptr<char[]>> getContent();
    // setter of the attribute 'content' . the data will not be modified but
    // copied.
    void setContent(int length, const char* content);

    // convert this packet into binary data. 1st value of pair is actual length
    // of data;
    virtual std::pair<int, std::unique_ptr<char[]>> encode();
    // convert binary data to NdnData
    static std::shared_ptr<NdnData> decode(const char* data);
    // convert this Ndndata object into readable string
    std::string toString() override;

   protected:
    char* content = nullptr;
    // in this object buffer size is always the content size
    int contentSize = 0;
    // encode attributes of this object into a vector of TlvObject
    virtual std::vector<TlvObject> encodeIntoTlvObjectArray() override;
};
#endif