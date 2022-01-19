#include <memory>
#include <unistd.h> 
#include"util/log/Logger.h"
#include"util/timer/Timer.h"
#include"ioc.h"
#include "util/log/TerminalLogger.h"
#include "util/log/FileLogger.h"
#include"physicalLayer/nic/NicManager.h"
#include"networkLayerPlus/NdnProtocolPlus.h"
#include"networkLayer/ndn/NdnProtocol.h"
#include"networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
using namespace std;
int main(int argc, char* argv[]){
    struct timeval tm;
    gettimeofday(&tm, NULL);
    int seed = tm.tv_sec * 1000 + tm.tv_usec / 1000;
    srand(seed);

    string name(argv[1]);
    Ioc::IOCInit({
        {LOGGER_TYPE,LOGGER_FILE},
        {LOGGER_FILENAME,name+".log"},
        {PLATFORM,PLATFORM_UNIX},
        {DISPLAY_NAME,name},

    });

    auto tmp = IOC->getNicManager()->getAllNicsInMap(false);
    for (auto i: tmp) {
        LOGGER->VERBOSE(i.second->toString());
    }
    auto ndnProtocol=make_shared<NdnProtocol>();
    IOC->getTransmitter()->registerNetworkLayerProtocol(NDN_PROTOCOL,ndnProtocol);
    auto ndnRoutingProtocol=make_shared<NdnRoutingProtocol>(atoi(name.substr(1, 1).c_str()),ndnProtocol);
    ndnProtocol->registerUpperLayerProtocol(NDN_ROUTING,ndnRoutingProtocol.get());
    ndnRoutingProtocol->start();
    IOC->getTransmitter()->listen();

}