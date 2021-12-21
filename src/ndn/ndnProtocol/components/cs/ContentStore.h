#ifndef __CONTENTSTORE_H_
#define __CONTENTSTORE_H_
#include <unordered_map>
#include <queue>
#include "ndn/ndnPacket/NdnData.h"
class ContentStore {
   public:
    std::shared_ptr<NdnData> contentHit(std::string name);
    void insertPacket(std::shared_ptr<NdnData>);

    private:
    std::unordered_map<std::string, std::shared_ptr<NdnData>>contentMap;
    std::queue<std::string>packetNames;

};

#endif