#ifndef __INFOINTERESTPACK_H_
#define __INFOINTERESTPACK_H_
#include <cstdint>
#include <vector>

#include "LinkStateDigest.h"
enum  InfoType { INFO_DOWN = 0, INFO_UP = 1, INFO_REFRESH = 2 };
class InfoInterestPack:public Printable {
   public:
    InfoType infoType;
    uint32_t src;
    std::vector<LinkStateDigest> ls;
    std::vector<uint32_t> neighbors;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual std::string toString() override;

};

inline std::string getNameForInfoType(InfoType t){
    switch(t){
        case InfoType::INFO_DOWN:return "DOWN";
        case InfoType::INFO_UP:return "UP";
        case InfoType::INFO_REFRESH: return "REFRESH";
    }
    return "";
}

#endif