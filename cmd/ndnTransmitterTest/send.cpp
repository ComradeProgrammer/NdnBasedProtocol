#include "ndn/ndnPacket/NdnInterest.h"
#include "ndn/ndnProtocol/NdnTransmitter.h"
using namespace std;
int main() {
    auto nics = NIC::getAllInterfaces();
    unsigned int i = 0;
    for (; i < nics.size(); i++) {
        if (nics[i].getName() == "h2-eth0") {
            cout << "h2-eth0 index " << nics[i].getInterfaceID() << endl;
            break;
        }
    }
    if (i == nics.size()) {
        cout << "fail to get nic" << endl;
        return -1;
    }

    auto trans = make_shared<NdnTransmitter>();
    auto packet = make_shared<NdnInterest>();
    packet->setApplicationParameters(11, "Helloworld");
    trans->send(nics[i].getInterfaceID(), MacAddress("ff:ff:ff:ff:ff:ff"),
                packet);
}