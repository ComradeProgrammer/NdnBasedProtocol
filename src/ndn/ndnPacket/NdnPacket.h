#ifndef __NDNPACKET_H_
#define __NDNPACKET_H_
#include <string>
#include <unordered_map>
//Abstract class, representing a ndn packet
class NdnPacket{
    public:
    //getter of name attribute
    std::string getName(){return name;}
    //setter of name attribute
    void setName(std::string _name){name=_name;}

    //getter of attribute packetType
    uint32_t getPacketType(){return packetType;}
    //setter of attribute packetType
    void setPacketType(uint32_t _packetType){packetType=_packetType;}
    //PURE VITURAL FUNTION encode this packet into binary data in tlv format for fufilling ethernet packet
    virtual std::pair<int,const char*>encode()=0;

    protected:
    // when decoding name in tlv, we chose to simplify it into a 1-level string instead of nested component in offical ndn
    std::string name;
    uint32_t packetType;

};

#endif