#include <iostream>

#include "ioc.h"
#include "physicalLayer/nic/NicManager.h"
using namespace std;

int main(int argc, char* argv[]) {
    auto tmp = IOC->getNicManager()->getAllNicsInMap(false);
    for (auto i : tmp) {
        cout << i.second->toString() << endl;
    }
}