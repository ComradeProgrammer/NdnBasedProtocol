#ifndef __INFOINTERESTPACK_H_
#define __INFOINTERESTPACK_H_
#include <cstdint>
#include <vector>

#include "LinkStateDigest.h"
#include "util/printable/Jsonifiable.h"
enum InfoType { INFO_DOWN = 0, INFO_UP = 1, INFO_REFRESH = 2 };
class InfoInterestPack : public Jsonfiable {
   public:
    InfoType infoType;
    RouterID src;
    std::vector<LinkStateDigest> ls;
    std::vector<RouterID> neighbors;

   public:
    void decode(const char* data, int dataLength);
    std::pair<int, std::unique_ptr<char[]>> encode();
    virtual nlohmann::json marshal() const override;
};

inline std::string getNameForInfoType(InfoType t) {
    switch (t) {
        case InfoType::INFO_DOWN:
            return "DOWN";
        case InfoType::INFO_UP:
            return "UP";
        case InfoType::INFO_REFRESH:
            return "REFRESH";
    }
    return "";
}

#endif