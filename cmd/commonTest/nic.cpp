#include "ethernet/interface/NIC.h"

#include <pthread.h>
#include <semaphore.h>

#include <fstream>
#include <iostream>

#include "ethernet/RawSocket.h"
#include "util/log/FileLogger.h"
using namespace std;

int main() {
    auto tmp = NIC::getAllInterfaces();
    for (auto i = 0; i < tmp.size(); i++) {
        cout << tmp[i].getName() << " " << tmp[i].getInterfaceID() << " "
             << tmp[i].getMacAddress().toString() << endl;
    }
}