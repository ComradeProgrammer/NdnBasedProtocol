#include "AddressValidator.h"
using namespace std;

void AddressValidator::establishFromChainBlock(string blockData) {
    vector<string> arr = split(blockData, ";");
    for (string line : arr) {
        vector<string> tuple = split(line, " ");
        if (tuple.size() != 2) {
            LOGGER->WARNING("AddressValidator::establishFromChainBlock invalid line " + line);
            continue;
        }
        Ipv4Address addr(tuple[0]);
        Ipv4Address mask(tuple[1]);
        if (!checkAddress(addr, mask)) {
            LOGGER->WARNINGF("AddressValidator::establishFromChainBlock address overlapped %s %s", addr.toString().c_str(), mask.toString().c_str());
            continue;
        }
        addresses.push_back(addr);
        masks.push_back(mask);
    }
}

void AddressValidator::clear() {
    addresses.clear();
    masks.clear();
}

bool AddressValidator::checkAddress(Ipv4Address net, Ipv4Address mask) {
    // currently we only handle network with mask /30
    for (int i = 0; i < addresses.size(); i++) {
        if (net.andMask(mask) == addresses[i]) {
            return false;
        }
    }
    return true;
}
