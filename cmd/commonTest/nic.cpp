#include "ethernet/interface/NIC.h"

#include <pthread.h>
#include <semaphore.h>

#include <fstream>
#include <iostream>

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include "ethernet/RawSocket.h"
#include "util/log/FileLogger.h"
using namespace std;


int main(int argc, char* argv[]) {
    string prefix = "";
    if (argc > 1) {
        prefix = string(argv[1]);
    }
    NIC::setPrefix(prefix);
    auto tmp = NIC::getAllInterfaces();
    for (auto i = 0; i < tmp.size(); i++) {
        cout << tmp[i].getName() << " " << tmp[i].getInterfaceID() << " "
             << tmp[i].getMacAddress().toString()<<" "<<tmp[i].getIpv4Address().toString()<<" "<<tmp[i].getIpv4Mask().toString()<<" " <<tmp[i].getLinkUp() <<endl;
    }
  
}