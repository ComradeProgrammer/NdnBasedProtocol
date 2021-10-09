#include <thread>

#include "ndn/ndnPacket/NdnInterest.h"
#include "ndn/ndnProtocol/NdnProtocol.h"
#include "ndn/ndnProtocol/NdnTransmitter.h"
#include "util/log/FileLogger.h"
using namespace std;
int main(int argc, char* argv[]) {
    string name(argv[1]);
    NIC::setPrefix(name + "-");

    auto logger = make_shared<FileLogger>(name + ".log");
    logger->INFO(name + " ndn protocol start");
    auto trans = NdnTransmitter::getTransmitter(logger);
    auto protocol =NdnProtocol::getNdnProtocol(logger);
    trans->setOnReceivePacket([protocol](int interfaceIndex,
                                         MacAddress sourceMac,
                                         shared_ptr<NdnPacket> packet) -> void {
        protocol->onIncomingPacket(interfaceIndex, sourceMac, packet);
    });

    // set a fake protocol:

    NdnProtocol::registerUpperLayerProtocol(
        -1,
        [logger, name, protocol](int interfaceIndex, MacAddress sourceMac,
                                 std::shared_ptr<NdnPacket> packet) -> void {
            logger->INFOF("fake protocol received packet %s",
                          packet->toString().c_str());

            if (name == "s3") {
                auto data = make_shared<NdnData>();
                data->setName(packet->getName());
                protocol->onIncomingPacket(
                    interfaceIndex, MacAddress("00:00:00:00:00:00"), data);
            }
        });

    // thread 1  for recv
    thread recv([name, trans, logger]() -> void {
        logger->INFO(name + " recv thread start");
        trans->listen();
    });
    thread send([protocol, logger, name]() -> void {
        if (name != "s1") {
            return;
        }

        logger->INFO(name + " send thread start");
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        auto packet = make_shared<NdnInterest>();
        packet->setName("/test/" + name);
        packet->setApplicationParameters(11, "Helloworld");
        auto nics = NIC::getAllInterfaces(logger);
        logger->INFO(packet->getName());
        protocol->onIncomingPacket(-1, MacAddress("00:00:00:00:00:00"), packet);
    });

    send.join();
    recv.join();

    return 0;
}