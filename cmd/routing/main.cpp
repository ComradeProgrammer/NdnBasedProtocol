#include <unistd.h>
#include <fstream>
#include <memory>

#include "ioc.h"
#include "networkLayer/ndn/NdnProtocol.h"
#include "networkLayerPlus/NdnProtocolPlus.h"
#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
#include "physicalLayer/nic/NicManager.h"
#include "util/log/FileLogger.h"
#include "util/log/Logger.h"
#include "util/log/TerminalLogger.h"
#include "util/timer/Timer.h"
#include "util/traceback/traceback.h"
using namespace std;
int main(int argc, char* argv[]) {
    try {
        string name(argv[1]);
        Ioc::IOCInit({
            {LOGGER_TYPE, LOGGER_FILE},
            {LOGGER_FILENAME, name + ".log"},
            {PLATFORM, PLATFORM_UNIX},
            {DISPLAY_NAME, name},

        });

        initSignalTraceback([](string traceback) { LOGGER->ERROR(traceback); });

        /*
            0: universal
            1: ndn
            2: ndnrouting
        */
        LOGGER->setLevels({0,2});

        struct timeval tm;
        gettimeofday(&tm, NULL);
        int seed = tm.tv_sec * 1000 + tm.tv_usec / 1000;
        srand(seed);

        auto tmp = IOC->getNicManager()->getAllNicsInMap(false);
        for (auto i : tmp) {
            LOGGER->VERBOSE(i.second->toString());
        }

        auto ndnProtocol = make_shared<NdnProtocol>();
        IOC->getTransmitter()->registerNetworkLayerProtocol(NDN_PROTOCOL, ndnProtocol);
        auto ndnRoutingProtocol = make_shared<NdnRoutingProtocol>(atoi(name.substr(1, name.size()-1).c_str()), ndnProtocol);
        ndnProtocol->registerUpperLayerProtocol(NDN_ROUTING, ndnRoutingProtocol.get());
        ndnRoutingProtocol->start();

        //print database
        thread daemon([ndnRoutingProtocol,name]()->void{
            this_thread::sleep_for(std::chrono::milliseconds(115*1000));
            fstream out(name+"_database.json",ios::out);
            out<<ndnRoutingProtocol->databaseContent()<<endl;
            out.close();

        });
        daemon.detach();
        
        IOC->getTransmitter()->listen();

    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}