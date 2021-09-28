#ifndef __NDNROUTINGPROTOCOL_H_
#define __NDNROUTINGPROTOCOL_H_
#include "util/log/Logger.h"
//singleton design pattern
class NdnRoutingProtocol{
    public:
    ~NdnRoutingProtocol()=default;
    void initialize();

    private:
    std::shared_ptr<Logger> logger;

    public:
    static std::shared_ptr<NdnRoutingProtocol>getNdnRoutingProtocol(std::shared_ptr<Logger> _logger = nullptr);

    private:
    NdnRoutingProtocol(std::shared_ptr<Logger> _logger = nullptr);
    static std::shared_ptr<NdnRoutingProtocol>protocol;
    static std::mutex staticLock;
};
#endif