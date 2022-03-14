#include "RoutingTable.h"
#include"ioc.h"

using namespace std;
using nlohmann::json;
RoutingTable::RoutingTable() {
    auto res = runCmd("route -n");
    if (res.first != 0) {
        LOGGER->ERRORF("failed to run route -n: return value %d", res.first);
        return;
    }
    auto lines = split(res.second, "\n");
    if (lines.size() < 2) {
        LOGGER->ERROR("got less than 2 lines output from command 'route -n'");
        return;
    }
    // remove the first line and second line
    for (int i = 2; i < lines.size(); i++) {
        if (lines[i] == "") {
            continue;
        }
        RoutingTableItem item(lines[i]);
        item.setFromRoutingProtocol(false);
        table.push_back(item);
    }
}

bool RoutingTable::addRoutingTableItem(RoutingTableItem item) {
    // route add -net 1.1.1.0/24 gw 192.168.50.1
    stringstream ss;
    ss << "route add -net " << item.getDestination().toString() << "/" << item.getMask().getPrefixLength() << " gw " << item.getNextHop().toString();

    // check whether there is identical routing item
    for (auto i : table) {
        if (i.getDestination() == item.getDestination() && i.getMask() == item.getMask()) {
            LOGGER->WARNING("route with identical destination and mask already exists");
            return false;
        }
    }

    auto res = runCmd(ss.str());
    if (res.first != 0) {
        LOGGER->ERRORF("failed to run %s, %d returned,output is %s", ss.str().c_str(), res.first, res.second.c_str());
        return false;
    }

    table.push_back(item);
    return true;
}

void RoutingTable::removeAllItem() {
    auto tableCopy=table;
    for (auto i : tableCopy) {
        if (!i.isFromRoutingProtocol()) {
            continue;
        }
        deleteRoutingTableItem(i);
    }
}

bool RoutingTable::deleteRoutingTableItem(RoutingTableItem item) {
    stringstream ss;
    ss << "route delete -net " << item.getDestination().toString() << "/" << item.getMask().getPrefixLength();
    LOGGER->VERBOSE(ss.str());

    vector<RoutingTableItem>::iterator itr;
    for (itr = table.begin(); itr != table.end(); itr++) {
        if (itr->getDestination() == item.getDestination() && itr->getMask() == item.getMask()) {
            break;
        }
    }
    if (itr == table.end()) {
        LOGGER->ERRORF("failed to find identical item %s", item.toString().c_str());
        return false;
    }

    auto res = runCmd(ss.str());
    if (res.first != 0) {
        LOGGER->ERRORF("failed to run %s, %d returned,output is %s", ss.str().c_str(), res.first, res.second.c_str());
        return false;
    }
    table.erase(itr);
    return true;
}

json RoutingTable::marshal() const {
    vector<json> res;
    for (auto i : table) {
        res.push_back(i.marshal());
    }
    json j(res);
    return j;
}
