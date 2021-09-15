#include <thread>

#include "ndn/ndnPacket/NdnInterest.h"
#include "ndn/ndnProtocol/NdnProtocol.h"
#include "ndn/ndnProtocol/NdnTransmitter.h"
#include "util/log/FileLogger.h"
using namespace std;
int main(int argc, char* argv[]) {
    string name(argv[1]);  // s1 or s2
    auto logger = make_shared<FileLogger>(name + ".log");
    NIC::setPrefix(name + "-");

    logger->INFO(name + " ndn protocol start");
    auto trans = NdnTransmitter::getTransmitter(logger);
    auto protocol = new NdnProtocol(logger);
    trans->setOnReceivePacket([protocol](int interfaceIndex,
                                         MacAddress sourceMac,
                                         shared_ptr<NdnPacket> packet) -> void {
        auto interest = dynamic_pointer_cast<NdnInterest>(packet);
        protocol->onIncomingInterest(interfaceIndex, sourceMac, interest);
    });

    // thread 1  for recv
    thread recv([name, trans, logger]() -> void {
        logger->INFO(name + " recv thread start");
        trans->listen();
    });
    thread send([trans, logger, name]() -> void {   
        if(name!="s1"){
            return;
        }

        logger->INFO(name + " send thread start");
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            auto packet = make_shared<NdnInterest>();
            packet->setName("/test/"+name);
            packet->setApplicationParameters(11, "Helloworld");
            auto nics = NIC::getAllInterfaces(logger);
            logger-> INFO(packet->getName());
            for (auto nic : nics) {
                trans->send(nic.getInterfaceID(),
                            MacAddress("ff:ff:ff:ff:ff:ff"), packet);
            }
    });

    send.join();
    recv.join();

    return 0;
}