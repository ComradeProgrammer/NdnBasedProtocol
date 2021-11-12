#include "ethernet/RawSocket.h"
#include "ethernet/ethernetPacket/EthernetPacket.h"
#include "ethernet/interface/NIC.h"
using namespace std;
// this host should be h2
// nic name should be h2-eth0 ,index 2
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

    RawSocket sock;
    char data[] = "Helloworld!";
    shared_ptr<EthernetPacket> packet =
        make_shared<EthernetPacket>(MacAddress("ff:ff:ff:ff:ff:ff"), nics[i].getMacAddress(), 0xffff, data, 12);
    sock.sendPacket(nics[i].getInterfaceID(), packet);
}