#include <iostream>

#include "ioc.h"
#include "networkLayer/ndn/NdnProtocol.h"
#include "networkLayerPlus/NdnProtocolPlus.h"
#include "networkLayerPlus/ndnAddr/NdnAddrAssignment.h"
#include "util/flag/Flag.h"
#include "util/signature/Md5RsaSignatureFactory.h"
#include "util/traceback/traceback.h"

using namespace std;
int main(int argc, char* argv[]) {
    try {
        Flag flag;
        flag.setFlagForValue("--name", "name of router");
        flag.setFlagForExistence("--root","is root");
        flag.setFlagForValue("--address","address");

        string error = flag.parseFlag(argc, argv, false);
        if (error != "") {
            return -1;
        }
        bool root=flag.flagExists("--root");
        string name = flag.getStringFlag("--name");
        string address = flag.getStringFlag("--address");

        Ioc::IOCInit({{LOGGER_TYPE, LOGGER_FILE}, {LOGGER_FILENAME, name + ".log"}, {PLATFORM, PLATFORM_UNIX}, {DISPLAY_NAME, name}});
        initSignalTraceback([](string traceback) { LOGGER->ERROR(traceback); });
        /*
            0: universal
            1: ndn
            2: ndnrouting
        */
        RouterID routerID = atoi(name.substr(1, name.size() - 1).c_str());
        LOGGER->setLevels({0,3});
        struct timeval tm;
        gettimeofday(&tm, NULL);
        int seed = tm.tv_sec * 1000 + tm.tv_usec / 1000;
        srand(seed+routerID);
        auto tmp = IOC->getNicManager()->getAllNicsInMap(false);
        for (auto i : tmp) {
            LOGGER->VERBOSE(i.second->toString());
        }
        auto ndnProtocol = make_shared<NdnProtocol>();
        IOC->getTransmitter()->registerNetworkLayerProtocol(NDN_PROTOCOL, ndnProtocol);

        // generate key pair
        // auto keyPair = RsaCipher::generateRsaKeyPair();
        // hash the publicKey to be routerID
        // RouterID routerID = CityHash64(keyPair.first.c_str(), keyPair.first.size() + 1);
        LOGGER->VERBOSEF("%s routerID %d, is root %d", name.c_str(), routerID,root);

        auto ndnAddrProtocol = make_shared<NdnAddrAssignmentProtocol>(routerID, ndnProtocol);
        ndnAddrProtocol->setIsRoot(root);
        if(root){
            ndnAddrProtocol->setAddressPool( make_shared<DumbAddressPool>(Ipv4Address(address)));
        }
        ndnProtocol->registerUpperLayerProtocol(NDN_ADDRASSIGNMENT, ndnAddrProtocol.get());
        ndnAddrProtocol->start();

        IOC->getTransmitter()->listen();
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
