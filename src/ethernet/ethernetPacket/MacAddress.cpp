#include "MacAddress.h"
using namespace std;
MacAddress::MacAddress() {
    for (int i = 0; i < 6; i++) {
        addr[i] = 0xff;
    }
}

MacAddress::MacAddress(string address, std::shared_ptr<Logger> logger) {
    std::shared_ptr<Logger> log = Logger::getDefaultLoggerIfNull(logger);

    vector<string> splits = split(address, ":");
    if (splits.size() != 6) {
        log->ERROR("MacAddress: Invalid format " + address);
        return;
    }

    for (int i = 0; i < 6; i++) {
        string hexString = string("0x") + splits[i];
        uint8_t segment =
            static_cast<uint8_t>(strtol(hexString.c_str(), nullptr, 16));
        addr[i] = segment;
    }
}

string MacAddress::toString() {
    char buffer[18];
    sprintf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2],
            addr[3], addr[4], addr[5]);
    return string(buffer);
}
