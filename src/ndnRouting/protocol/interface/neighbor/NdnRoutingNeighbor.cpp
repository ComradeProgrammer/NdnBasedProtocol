#include"NdnRoutingNeighbor.h"
#include "ndnRouting/protocol/interface/NdnRoutingInterface.h"
NdnRoutingNeighbor::NdnRoutingNeighbor(NdnRoutingInterface* _interface,std::shared_ptr<Logger> _logger ):interface(_interface){
    logger=Logger::getDefaultLoggerIfNull(_logger);
}