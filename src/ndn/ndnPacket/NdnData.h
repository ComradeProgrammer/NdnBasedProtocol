#ifndef __NDNDATA_H_
#define __NDNDATA_H_
#include <memory>
#include <string>

#include "NdnPacket.h"
class NdnData : public NdnPacket {
   public:
    // default constructor
    NdnData(std::shared_ptr<Logger> log = nullptr) {
        logger = Logger::getDefaultLoggerIfNull(log);
    };
    ~NdnData();
    // copy constructor of NdnData
    NdnData(const NdnData& data);

    NdnData& operator=(const NdnData& data);

    std::pair<int, std::unique_ptr<char[]>> getContent();
    void setContent(int length, const char* content);
    virtual std::pair<int, std::unique_ptr<char[]>> encode();
    static std::shared_ptr<NdnData> decode(
        const char* data, std::shared_ptr<Logger> _logger = nullptr);

   protected:
    std::shared_ptr<Logger> logger = nullptr;
    char* content = nullptr;
    // in this object buffer size is always the content size
    int contentSize = 0;
    virtual std::vector<TlvObject> encodeIntoTlvObjectArray();
};
#endif