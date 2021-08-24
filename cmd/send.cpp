#include"ethernet/RawSocket.h"
#include"ethernet/ethernetPacket/EthernetPacket.h"
using namespace std;
int main(){
     RawSocket sock;
    char data[] = "Helloworld!";
    shared_ptr<EthernetPacket> packet = make_shared<EthernetPacket>(
        MacAddress("ff:ff:ff:ff:ff:ff"),MacAddress("fa:33:42:5c:9e:23"),
        0xffff, data, 12);
    sock.sendPacket(1, packet);  // maybe this index should be modified depend
                                 // on where this code is running
}