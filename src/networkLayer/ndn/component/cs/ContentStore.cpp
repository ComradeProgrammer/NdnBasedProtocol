#include "ContentStore.h"
using namespace std;
void ContentStore::insertPacket(shared_ptr<NdnData> data) {
    if (packetNames.size() > CSCAPACITY) {
        string deletedName = packetNames.front();
        packetNames.pop();
        auto itr = contentMap.find(deletedName);
        if (itr != contentMap.end()) {
            contentMap.erase(itr);
        }
    }
    contentMap[data->getName()] = data;
    packetNames.push(data->getName());
}

shared_ptr<NdnData> ContentStore::contentHit(string name) {
    auto itr = contentMap.find(name);
    if (itr != contentMap.end()) {
        return itr->second;
    }
    return nullptr;
}