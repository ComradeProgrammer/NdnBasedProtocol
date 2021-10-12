#include "NdnRoutingInterfaceState.h"
using namespace std;
string getNameForInterfaceStateType(NdnRoutingInterfaceStateType state) {
    switch (state) {
        case UP:
            return "UP";
        case DOWN:
            return "DOWN";
    }
    return "";
}
string getNameForInterfaceEventType(NdnRoutingInterfaceEventType event) {
    switch (event) {
        case INTERFACE_UP:
            return "INTERFACE_UP";
        // case HELLO_TIMER:
        //     return "HELLO_TIMER";
        // case INFO_TIMER:
        //     return "INFO_TIMER";
        case INTERFACE_DOWN:
            return "INTERFACE_DOWN";
    }
    return "";
}