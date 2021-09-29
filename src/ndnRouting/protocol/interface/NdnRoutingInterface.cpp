#include "NdnRoutingInterface.h"
NdnRoutingInterface::NdnRoutingInterface(NIC nic,
                                         std::shared_ptr<Logger> _logger) {
    name = nic.getName();
    interfaceID = nic.getInterfaceID();
    macAddress = nic.getMacAddress();
    logger = Logger::getDefaultLoggerIfNull(_logger);
}