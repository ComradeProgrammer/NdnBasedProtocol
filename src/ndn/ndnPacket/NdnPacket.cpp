#include "NdnPacket.h"

#include "NdnData.h"
#include "NdnInterest.h"
using namespace std;
std::shared_ptr<NdnPacket> NdnPacket::decode(const char* data, std::shared_ptr<Logger> _logger) {
    auto logger = Logger::getDefaultLoggerIfNull(_logger);
    uint64_t packetType = TlvObject::peekType(data);
    switch (packetType) {
        case TLV_INTEREST:
            return NdnInterest::decode(data, logger);
            break;
        case TLV_DATA:
            return NdnData::decode(data, logger);
            break;
        default:
            logger->ERROR("NdnPacket::decode unrecongnized packet type " + to_string(packetType));
    }
    return nullptr;
}