#include "NdnAddrInterfaceState.h"
using namespace std;
string getNameForAddrInterfaceStateType(NdnAddrInterfaceStateType state) {
    switch (state) {
        case NdnAddrInterfaceStateType::DOWN:
            return "DOWN";
        case NdnAddrInterfaceStateType::WAITING:
            return "WAITING";
        case NdnAddrInterfaceStateType::LEADER:
            return "LEADER";
        case NdnAddrInterfaceStateType::NORMAL:
            return "NORMAL";
    }
}

string getNameForAddrInterfaceEventType(NdnAddrInterfaceEventType event) {
    switch (event) {
        case NdnAddrInterfaceEventType::INTERFACE_UP:
            return "INTERFACE_UP";
        case NdnAddrInterfaceEventType::WAIT_TIMER_UP:
            return "WAIT_TIMER_UP";
        case NdnAddrInterfaceEventType::NEIGHBOR_CHANGE:
            return "NEIGHBOR_CHANGE";
        case NdnAddrInterfaceEventType::INTERFACE_DOWN:
            return "INTERFACE_DOWN";
    }
}