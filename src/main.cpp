#include <fstream>
#include <iostream>

#include "ethernet/ethernetPacket/MacAddress.h"
#include "util/log/FileLogger.h"
using namespace std;
int main() {
    // FileLogger l("test.txt");
    // l.error("test",74,"");
    cout << sizeof(MacAddress) << endl;
}