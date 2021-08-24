#include"ethernet/RawSocket.h"
#include"ethernet/ethernetPacket/EthernetPacket.h"
using namespace std;
int main(){
     while(1){
          RawSocket sock;

        auto p = sock.receivePacket(0xffff);
        cout<<p.second->getData()<<endl;
     }
}