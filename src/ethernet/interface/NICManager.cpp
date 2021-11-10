#include"NICManager.h"
using namespace std;
shared_ptr<NICManager>NICManager::nicManager=nullptr;
mutex NICManager::staticLock;
shared_ptr<NICManager>NICManager::getNICManager(std::shared_ptr<Logger>logger){
    lock_guard<mutex>lockFunction(staticLock);
    if(nicManager==nullptr){
        NICManager* tmp=new NICManager(logger);
        nicManager=shared_ptr<NICManager>(tmp);
    }
    return nicManager;
}

NICManager::NICManager(std::shared_ptr<Logger> _logger){
    logger=Logger::getDefaultLoggerIfNull(_logger);
}

void NICManager::flush() {
    int sock = socket(PF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        logger->ERROR("NIC::getAllInterfaces: failed to open a raw socket");
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
            logger->WARNING(
                string(
                    "NIC::getAllInterfaces: failed to get SIOCGIFINDEX for ") +
                name);
            continue;
        }
        int interfaceID = newReq.ifr_ifindex;

        // use another new request to get the mac address
        ifreq newReq2;
        strncpy(newReq2.ifr_name, name.c_str(), name.size() + 1);
        ret = ioctl(sock, SIOCGIFHWADDR, &newReq2);
        if (ret != 0) {
            logger->WARNING(
                "NIC::getAllInterfaces: failed to get SIOCGIFHWADD for " +
                name);
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
            logger->WARNING(
                "NIC::getAllInterfaces: failed to get SIOCGIFADDR for " + name);
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
            logger->WARNING(
                "NIC::getAllInterfaces: failed to get SIOCGIFNETMASK for " +
                name);
            continue;
        }
        struct sockaddr_in *maskin = (struct sockaddr_in *)&(newReq4.ifr_addr);
        Ipv4Address ipMask;
        ipMask.addr = maskin->sin_addr.s_addr;

        bool isUp = checkLinkUpByName(name);
        nicVectorCache.push_back(
            NIC(name, interfaceID, macAddress, ipAddr, ipMask, isUp));
    }
    for (int i = 0; i < nicVectorCache.size(); i++) {
        nicMapCache[nicVectorCache[i].getInterfaceID()] = nicVectorCache[i];
    }
}

bool NICManager::checkLinkUpByName(std::string s) {
    int skfd;
    struct ifreq ifr;
    struct ethtool_value edata;
    const char *pIfName = s.c_str();
    edata.cmd = ETHTOOL_GLINK;
    edata.data = 0;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, pIfName, sizeof(ifr.ifr_name) - 1);
    ifr.ifr_data = (char *)&edata;
    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        logger->ERROR("NIC::checkLinkUpByName failed to read link state of " +
                      s);
        return false;
    }
    if (ioctl(skfd, SIOCETHTOOL, &ifr) == -1) {
        close(skfd);
        logger->ERROR("NIC::checkLinkUpByName failed to read link state of " +
                      s);
        return false;
    }
    close(skfd);
    return edata.data == 1;
}
void NICManager::setPrefix(string _prefix) {
    lock_guard<mutex> lockFunction(lock);
    prefix = _prefix;
}

unordered_map<int, NIC> NICManager::getNICMap() {
    lock_guard<mutex> lockFunction(lock);
    if (nicVectorCache.size() == 0) {
        flush();
    }

    if (prefix == "") {
        return nicMapCache;
    }
    unordered_map<int, NIC> res;
    for (auto pair : nicMapCache) {
        if (pair.second.getName().find(prefix) != string::npos) {
            res[pair.first] = pair.second;
        }
    }
    return res;
}

vector<NIC> NICManager::getAllInterfaces() {
    lock_guard<mutex> lockFunction(lock);
    if (nicVectorCache.size() == 0) {
        flush();
    }
    if (prefix == "") {
        return nicVectorCache;
    }
    vector<NIC> res;
    for (auto nic : nicVectorCache) {
        if (nic.getName().find(prefix) != string::npos) {
            res.push_back(nic);
        }
    }
    return res;
}


void  NICManager::startMonitor(){
    thread monitorDaemon([this]()->void{
        this_thread::sleep_for(std::chrono::milliseconds(POLLING_INTERVAL));
        lock.lock();
        unordered_map<int, NIC> oldNicMap=nicMapCache;
        flush();
        //currently we only consider that some NIC may go down. we don't considerate the possibility that a NIC get deleted or created.
        //todo: CONTINUE HERE
        // for(auto pair: oldNicMap){
        //     if(nicMapCache.find(pair.first))
        // }
        lock.unlock();
    });
}
