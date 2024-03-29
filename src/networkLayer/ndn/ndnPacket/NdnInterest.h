#ifndef __NDNINTEREST_H_
#define __NDNINTEREST_H_
#include <memory>

#include "NdnPacket.h"
#include "ioc.h"
#include "tlv/TlvObject.h"
#include "tlv/TlvObjectList.h"
#include "util/json/json.hpp"
class NdnInterest : public NdnPacket {
   public:
    NdnInterest() { setPacketType(TLV_INTEREST); };
    // deconstructor
    virtual ~NdnInterest();
    // copy constructor
    NdnInterest(const NdnInterest& old);
    // copy assignment operator
    NdnInterest& operator=(const NdnInterest& old);

    // getter for attribute mustBeFresh
    bool getMustBeFresh() { return mustBeFresh; }
    // setter for attribute mustBeFresh
    void setMustBeFresh(bool v) { mustBeFresh = v; }
    // getter for attribute canBePrefix
    bool getCanBePrefix() { return canBePrefix; }
    // setter for attribute canBePrefix
    void setCanBePrefix(bool v) { canBePrefix = v; }
    // getter for attribute nonce
    uint32_t getNonce() { return nonce; }
    // setter for attribute nonce
    void setNonce(uint32_t v) { nonce = v; }

    /**
     * @brief getter for attribute hopLimit
     * @return pair<bool,uint8_t>, 1st value marks whether this packet omitts
     * hopLimit. 2nd is the value of hopLimit. if 1st value is set as true, 2nd
     * value should be ignored
     * */
    std::pair<bool, uint8_t> getHopLimit() { return std::pair<bool, uint8_t>(hoplimitOmitted, hopLimit); }
    /**
     * @brief setter for attribute hopLimit
     * @param omitted mark for whether this packet should contain hopLimit.
     * @param value hopLimit value, omitted if parameter omitted is set true
     */
    void setHopLimit(bool omitted, uint8_t value) {
        hoplimitOmitted = omitted;
        hopLimit = value;
    }

    std::pair<int, std::unique_ptr<char[]>> getApplicationParameters();
    void setApplicationParameters(int length, const char* data);

    // convert this packet into binary data. 1st value of pair is actual length
    // of data;
    virtual std::pair<int, std::unique_ptr<char[]>> encode() override;
    // convert binary data to NdnInterest
    static std::shared_ptr<NdnInterest> decode(const char* data);
    // convert this Ndndata object into readable string
    std::string toString() override;

   protected:
    bool canBePrefix = false;
    bool mustBeFresh = false;
    uint32_t nonce = false;
    uint8_t hopLimit;
    bool hoplimitOmitted = true;

    char* applicationParameters = nullptr;
    int applicationParametersSize = 0;

    // encode attributes of this object into a vector of TlvObject
    virtual std::vector<TlvObject> encodeIntoTlvObjectArray() override;
};

#endif