#ifndef __CONTENTSTORE_H_
#define __CONTENTSTORE_H_
#include"ndn/ndnPacket/NdnData.h"
class ContentStore{
    public:
    std::shared_ptr<NdnData>contentHit(std::string name){
        //TODO: implement CS
        return nullptr;
    }
};

#endif