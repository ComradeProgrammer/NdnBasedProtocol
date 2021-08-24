#include <fstream>
#include <string>

#include "ethernet/RawSocket.h"
#include "ethernet/ethernetPacket/EthernetPacket.h"
using namespace std;

// this host should be h1
// nic name should be h1-eth0
int main() {
    RawSocket sock;
    auto p = sock.receivePacket(NDN_PROTOCOL);
    if (string(p.second->getData()) == "Helloworld!") {
        cout << "PASS";
        return 0;
    }
    cout << "FAIL";
    return 1;
}