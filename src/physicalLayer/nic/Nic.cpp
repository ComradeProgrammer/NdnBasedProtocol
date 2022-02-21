#include "Nic.h"
using namespace std;
using nlohmann::json;
Nic::Nic(std::string _name, int _interfaceID, MacAddress address, Ipv4Address _ipAddr, Ipv4Address _ipMask, bool _linkUp)
    : name(_name), interfaceID(_interfaceID), macAddress(address), linkUp(_linkUp), ipAddr(_ipAddr), ipMask(_ipMask) {}
nlohmann::json Nic::marshal() const {
    json j;
    j["name"] = name;
    j["interfaceID"] = interfaceID;
    j["macAddress"] = macAddress.toString();
    j["ip"] = ipAddr.toString();
    j["mask"] = ipAddr.toString();
    j["linkUp"] = linkUp;
    return j;
}