#include "util.h"
using namespace std;

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