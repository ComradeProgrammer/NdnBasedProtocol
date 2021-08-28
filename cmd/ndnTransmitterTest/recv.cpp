#include "ndn/ndnPacket/NdnInterest.h"
#include "ndn/ndnProtocol/NdnTransmitter.h"
using namespace std;
int main() {
    auto trans = make_shared<NdnTransmitter>();
    trans->setOnReceivePacket([](int interfaceIndex, MacAddress sourceMac,
                                 shared_ptr<NdnPacket> packet) -> void {
        shared_ptr<NdnInterest> tmp = dynamic_pointer_cast<NdnInterest>(packet);
        string res = (tmp->getApplicationParameters().second.get());
        if (res == "Helloworld") {
            exit(0);
        } else {
            exit(1);
        }
    });
    trans->listen();
}