#include "NIC.h"
using namespace std;
#define BUF_SIZE 512
mutex NIC::lock;
vector<NIC> NIC::nicVectorCache;
unordered_map<int, NIC> NIC::nicMapCache;
string NIC::prefix="";
NIC::NIC(std::string _name, int _interfaceID, MacAddress address)
    : name(_name), interfaceID(_interfaceID), macAddress(address) {}

vector<NIC> NIC::getAllInterfaces(std::shared_ptr<Logger> _logger) {
    lock_guard<mutex> lockFunction(lock);
    if (nicVectorCache.size() == 0) {
        flush(_logger);
    }
    if(prefix==""){
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
unordered_map<int, NIC> NIC::getNICMap(shared_ptr<Logger> _logger) {
    lock_guard<mutex> lockFunction(lock);
    if (nicVectorCache.size() == 0) {
        flush(_logger);
    }

    if(prefix==""){
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

void NIC::reloadCache(shared_ptr<Logger> _logger) {
    lock_guard<mutex> lockFunction(lock);
    flush(_logger);
}

void NIC::setPrefix(string _prefix) {
    lock_guard<mutex> lockFunction(lock);
    prefix = _prefix;
}

void NIC::flush(std::shared_ptr<Logger> _logger) {
    std::shared_ptr<Logger> logger = Logger::getDefaultLoggerIfNull(_logger);
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
        nicVectorCache.push_back(NIC(name, interfaceID, macAddress));
    }
    for (int i = 0; i < nicVectorCache.size(); i++) {
        nicMapCache[nicVectorCache[i].getInterfaceID()] = nicVectorCache[i];
        logger->INFO(
            "NdnTransmitter: detected " + nicVectorCache[i].getName() +
            " at interface " + to_string(nicVectorCache[i].getInterfaceID()) +
            ", mac address " + nicVectorCache[i].getMacAddress().toString());
    }
}
