#include "NdnRoutingInterfaceState.h"
using namespace std;
string getNameForInterfaceStateType(NdnRoutingInterfaceStateType state) {
    switch (state) {
        case NdnRoutingInterfaceStateType::UP:
            return "UP";
        case NdnRoutingInterfaceStateType::DOWN:
            return "DOWN";
    }
    return "";
}
string getNameForInterfaceEventType(NdnRoutingInterfaceEventType event) {
    switch (event) {
        case NdnRoutingInterfaceEventType::INTERFACE_UP:
            return "INTERFACE_UP";
        // case HELLO_TIMER:
        //     return "HELLO_TIMER";
        // case INFO_TIMER:
        //     return "INFO_TIMER";
        case NdnRoutingInterfaceEventType::INTERFACE_DOWN:
            return "INTERFACE_DOWN";
    }
    return "";
}