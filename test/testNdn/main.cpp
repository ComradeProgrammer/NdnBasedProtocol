#include <unistd.h>

#include <iostream>

#include "ioc.h"
#include "networkLayer/ndn/NdnProtocol.h"
#include "networkLayerPlus/NdnProtocolPlus.h"
#include "physicalLayer/nic/NicManager.h"
using namespace std;
class FakeProtocol : public NdnProtocolPlus {
   public:
    FakeProtocol(std::shared_ptr<NdnProtocol> _protocol) { protocol = _protocol; }
    virtual void onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet) override {
        LOGGER->INFOF(0,"FakeProtocol:packet received from interface %d,sourcemac %s, packet %s", interfaceIndex, sourceMac.toString().c_str(),
                      packet->toString().c_str());
        if (packet->getPacketType() == TLV_INTEREST) {
            auto data = make_shared<NdnData>();
            data->setName(packet->getName());
            data->setContent(6, "hello");
            protocol->onReceiveNdnPacket(NDN_ROUTING, MacAddress("ff:ff:ff:ff:ff:ff"), data);
        } else {
            auto data = dynamic_pointer_cast<NdnData>(packet);
            auto content = data->getContent();
            LOGGER->INFO(0,content.second.get());
        }
    }

    void start() {
        auto interest = make_shared<NdnInterest>();

        interest->setNonce(rand());
        IOC->getTimer()->startTimer("test_timer", 1000, [this, interest](string) -> bool {
            interest->setName("/routing/" + IOC->getDisplayName() + "/" + to_string(i));
            this->i++;
            protocol->onReceiveNdnPacket(NDN_ROUTING, MacAddress("ff:ff:ff:ff:ff:ff"), interest);
            return false;
        });
    }
    std::shared_ptr<NdnProtocol> protocol;
    int i;
};
int main(int argc, char* argv[]) {
    struct timeval tm;
    gettimeofday(&tm, NULL);
    int seed = tm.tv_sec * 1000 + tm.tv_usec / 1000;
    srand(seed);

    string name(argv[1]);
    Ioc::IOCInit({
        {LOGGER_TYPE, LOGGER_FILE},
        {LOGGER_FILENAME, name + ".log"},
        {PLATFORM, PLATFORM_UNIX},
        {DISPLAY_NAME, name},

    });

    auto tmp = IOC->getNicManager()->getAllNicsInMap(false);
    for (auto i : tmp) {
        LOGGER->VERBOSE(i.second->toString());
    }

    auto ndnProtocol = make_shared<NdnProtocol>();
    IOC->getTransmitter()->registerNetworkLayerProtocol(NDN_PROTOCOL, ndnProtocol);
    auto fakeProtocol = make_shared<FakeProtocol>(ndnProtocol);
    ndnProtocol->registerUpperLayerProtocol(NDN_ROUTING, fakeProtocol.get());
    fakeProtocol->start();
    IOC->getTransmitter()->listen();
}