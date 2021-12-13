#include "NdnAddressAssignment.h"
using namespace std;
mutex NdnAddressAssignment::staticLock;
shared_ptr<NdnAddressAssignment> NdnAddressAssignment::staticPtr = nullptr;
shared_ptr<NdnAddressAssignment> NdnAddressAssignment::getNdnAddressAssignment(std::shared_ptr<Logger> _logger) {
    lock_guard<mutex> lockFunction(staticLock);
    if (staticPtr == nullptr) {
        NdnAddressAssignment* tmp = new NdnAddressAssignment(_logger);
        staticPtr = shared_ptr<NdnAddressAssignment>(tmp);
    }
    return staticPtr;
}

NdnAddressAssignment::NdnAddressAssignment(shared_ptr<Logger> _logger) {
    logger = Logger::getDefaultLoggerIfNull(_logger);
}

void NdnAddressAssignment::lock() { objectLock.lock(); }

void NdnAddressAssignment::unlock() { objectLock.unlock(); }

void NdnAddressAssignment::initialize(){
    
}
