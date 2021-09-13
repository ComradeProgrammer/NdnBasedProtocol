#include "util.h"
#include"util/json/json.hpp"
#include "ethernet/ethernetPacket/MacAddress.h"
using namespace std;
using nlohmann::json;

std::vector<std::string> split(std::string s, string sep) {
    vector<string> res;
    if (s == "" || sep == "") {
        return res;
    }
    size_t pos = 0;
    while (pos < s.size()) {
        size_t nextPos = s.find_first_of(sep, pos);
        if (nextPos == string::npos) {
            nextPos = s.size();
        }
        string tmp = s.substr(pos, nextPos - pos);
        res.push_back(tmp);
        pos = nextPos + sep.size();
    }
    return res;
}
string intVectorToString(vector<int>input){
    json j(input);
    return j.dump();
}
string intMacAddressVectorToString(std::vector<std::pair<int,MacAddress>>input){
    vector<string>tmp;
    for(int i=0;i<input.size();i++){
        tmp.push_back(string("interface ")+to_string(input[i].first)+"("+input[i].second.toString()+")");
    }
    json j(tmp);
    return j.dump();
}

