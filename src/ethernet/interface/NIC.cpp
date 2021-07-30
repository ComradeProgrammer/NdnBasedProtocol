#include "NIC.h"
using namespace std;
#define BUF_SIZE 512
NIC::NIC(std::string _name, int _interfaceID, MacAddress address)
    : name(_name), interfaceID(_interfaceID), macAddress(address) {}

vector<NIC> NIC::getAllInterfaces(std::shared_ptr<Logger> _logger) {
    vector<NIC> res;
    std::shared_ptr<Logger> logger = Logger::getDefaultLoggerIfNull(_logger);
    int sock = socket(PF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        logger->ERROR("NIC::getAllInterfaces: failed to open a raw socket");
        return res;
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
        strncpy(newReq2.ifr_name, name.c_str(), strlen(name.c_str()));
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
        res.push_back(NIC(name, interfaceID, macAddress));
    }
    return res;
}