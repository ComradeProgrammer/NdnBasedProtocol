#include "NicManagerUnix.h"
using namespace std;

void NicManagerUnix::flush() {
    unordered_map<int, shared_ptr<Nic>> oldNicMap = nicMapCache;
    nicMapCache.clear();
    int sock = socket(PF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        LOGGER->ERROR("NIC::getAllInterfaces: failed to open a raw socket");
    }
    // read all NICS through command /proc/net/dev

    fstream dev("/proc/net/dev", ios::in);

    // now read all configs
    int i = 0;
    while (1) {
        string line;
        if (!getline(dev, line)) {
            break;
        }
        i++;
        if (i <= 2) {
            continue;
        }

        auto splits = split(line, " ");

        string name;
        for (string s : splits) {
            if (s != "") {
                name = s;
                break;
            }
        }
        // remove the ':'
        name = name.substr(0, name.size() - 1);

        // use another new request to get the index
        ifreq newReq;
        strncpy(newReq.ifr_name, name.c_str(), name.size() + 1);
        int ret = ioctl(sock, SIOCGIFINDEX, &newReq);
        if (ret != 0) {
            LOGGER->WARNING(string("NIC::getAllInterfaces: failed to get SIOCGIFINDEX for ") + name);
            continue;
        }
        int interfaceID = newReq.ifr_ifindex;

        // use another new request to get the mac address
        ifreq newReq2;
        strncpy(newReq2.ifr_name, name.c_str(), name.size() + 1);
        ret = ioctl(sock, SIOCGIFHWADDR, &newReq2);
        if (ret != 0) {
            LOGGER->WARNING("NIC::getAllInterfaces: failed to get SIOCGIFHWADD for " + name);
            continue;
        }

        MacAddress macAddress;
        for (int j = 0; j < 6; j++) {
            macAddress.addr[j] = newReq2.ifr_hwaddr.sa_data[j];
        }

        // use another new request to get ip address
        ifreq newReq3;
        strncpy(newReq3.ifr_name, name.c_str(), name.size() + 1);
        ret = ioctl(sock, SIOCGIFADDR, &newReq3);
        if (ret != 0) {
            LOGGER->WARNING("NIC::getAllInterfaces: failed to get SIOCGIFADDR for " + name);
            continue;
        }
        struct sockaddr_in *sin = (struct sockaddr_in *)&(newReq3.ifr_addr);
        Ipv4Address ipAddr;
        ipAddr.addr = sin->sin_addr.s_addr;
        // and another request to get mask
        ifreq newReq4;
        strncpy(newReq4.ifr_name, name.c_str(), name.size() + 1);
        ret = ioctl(sock, SIOCGIFNETMASK, &newReq4);
        if (ret != 0) {
            LOGGER->WARNING("NIC::getAllInterfaces: failed to get SIOCGIFNETMASK for " + name);
            continue;
        }
        struct sockaddr_in *maskin = (struct sockaddr_in *)&(newReq4.ifr_addr);
        Ipv4Address ipMask;
        ipMask.addr = maskin->sin_addr.s_addr;

        bool isUp = checkLinkUpByName(name);
        if(name!="lo"){
        nicMapCache[interfaceID] = make_shared<Nic>(name, interfaceID, macAddress, ipAddr, ipMask, isUp);

        }
    }

    // currently we only consider that some NIC may go up or down. we don't considerate the possibility that a NIC get
    // deleted or created.
    for (auto pair : oldNicMap) {
        int index = pair.first;
        if (nicMapCache.find(index) == nicMapCache.end()) {
            continue;
        }
        if (nicMapCache[index]->getLinkUp() == true && oldNicMap[index]->getLinkUp() == false) {
            notifyObservers(index, NICEvent::NIC_UP);
        }
        if (nicMapCache[index]->getLinkUp() == false && oldNicMap[index]->getLinkUp() == true) {
            notifyObservers(index, NICEvent::NIC_DOWN);
        }
    }
}

bool NicManagerUnix::checkLinkUpByName(std::string s) {
    int sock;
    struct ifreq ifr;
    struct ethtool_value edata;
    const char *pIfName = s.c_str();
    edata.cmd = ETHTOOL_GLINK;
    edata.data = 0;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, pIfName, sizeof(ifr.ifr_name) - 1);
    ifr.ifr_data = (char *)&edata;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        LOGGER->ERROR("NIC::checkLinkUpByName failed to read link state of " + s);
        return false;
    }
    if (ioctl(sock, SIOCETHTOOL, &ifr) == -1) {
        close(sock);
        LOGGER->ERROR("NIC::checkLinkUpByName failed to read link state of " + s);
        return false;
    }
    close(sock);
    return edata.data == 1;
}
unordered_map<int, shared_ptr<Nic>> NicManagerUnix::getAllNicsInMap(bool shouldFlush) {
    lock_guard<mutex> lockFunction(lock);
    if (nicMapCache.size() == 0 || shouldFlush) {
        flush();
    }
    return nicMapCache;
}

void NicManagerUnix::registerObserver(NicObserverInterface *observer, int interfaceID) {
    lock_guard<mutex> lockFunction(lock);

    if (observers.find(interfaceID) == observers.end()) {
        observers[interfaceID] = vector<NicObserverInterface *>();
    }
    auto itr = find(observers[interfaceID].begin(), observers[interfaceID].end(), observer);
    if (itr == observers[interfaceID].end()) {
        observers[interfaceID].push_back(observer);
    }
}

void NicManagerUnix::deleteObserver(NicObserverInterface *observer) {
    lock_guard<mutex> lockFunction(lock);

    for (auto p : observers) {
        auto itr = find(observers[p.first].begin(), observers[p.first].end(), observer);
        if (itr != observers[p.first].end()) {
            observers[p.first].erase(itr);
        }
    }
}

void NicManagerUnix::notifyObservers(int interfaceIndex, NICEvent event) {
    for (auto observer : observers[interfaceIndex]) {
        thread tmp([interfaceIndex,event,observer]()->void{
            observer->onEventHappen(interfaceIndex, event);
        });
        tmp.detach();
        
    }
    for (auto observer : observers[-1]) {
        thread tmp([interfaceIndex,event,observer]()->void{
            observer->onEventHappen(interfaceIndex, event);
        });
        tmp.detach();
    }
}

void NicManagerUnix::startMonitor() {
    thread monitorDaemon([this]() -> void {
        while (1) {
            this_thread::sleep_for(std::chrono::milliseconds(NIC_STATUS_POLLING_INTERVAL));
            lock.lock();
            flush();
            lock.unlock();
        }
    });
    monitorDaemon.detach();
}
