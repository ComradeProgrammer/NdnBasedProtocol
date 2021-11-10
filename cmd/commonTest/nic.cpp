#include "ethernet/interface/NIC.h"
#include "ethernet/interface/NICManager.h"
#include <arpa/inet.h>
#include <linux/ethtool.h>
#include <linux/if_packet.h>
#include <linux/sockios.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <fstream>
#include <iostream>

#include "ethernet/RawSocket.h"
#include "util/log/FileLogger.h"
using namespace std;

int main(int argc, char* argv[]) {
    string prefix = "";
    if (argc > 1) {
        prefix = string(argv[1]);
    }
    NICManager::getNICManager()->setPrefix(prefix);
    auto tmp =  NICManager::getNICManager()->getAllInterfaces();
    for (auto i = 0; i < tmp.size(); i++) {
        cout << tmp[i].toString()
             << endl;
    }
}