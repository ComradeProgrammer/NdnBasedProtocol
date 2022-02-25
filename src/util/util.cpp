#include "util.h"

#include "linkLayer/MacAddress.h"
#include "util/json/json.hpp"
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

string intVectorToString(vector<int> input) {
    json j(input);
    return j.dump();
}

string intMacAddressVectorToString(std::vector<std::pair<int, MacAddress>> input) {
    vector<string> tmp;
    for (int i = 0; i < input.size(); i++) {
        tmp.push_back(string("interface ") + to_string(input[i].first) + "(" + input[i].second.toString() + ")");
    }
    json j(tmp);
    return j.dump();
}

string getCurrentTime() {
    struct timeval tpend;
    time_t timep;
    time(&timep);
    char dateBuf[20];
    strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", localtime(&timep));

    gettimeofday(&tpend, NULL);
    int secofday = (tpend.tv_sec + 3600 * 8) % 86400;
    int hours = secofday / 3600;
    int minutes = (secofday - hours * 3600) / 60;
    int seconds = secofday % 60;
    int milliseconds = tpend.tv_usec / 1000;
    char buf[40];
    sprintf(buf, "%02d:%02d:%02d.%03d", hours, minutes, seconds, milliseconds);
    return string(dateBuf) + " " + string(buf);
}

string hexString(const void* data, unsigned int length) {
    stringstream ss;
    const unsigned char* binary = (const unsigned char*)(data);
    for (int i = 0; i < length; i++) {
        ss << setfill('0') << setw(2) << hex << (unsigned int)(binary[i]);
    }
    return ss.str();
}