#include"Pit.h"
using namespace std;
shared_ptr<PitEntry> Pit::getPitEntry(string name){
    std::lock_guard<std::mutex> lockFunction(lock);
    auto itr=entries.find(name);
    if(itr==entries.end()){
        shared_ptr<PitEntry> entry = make_shared<PitEntry>(name);
        entries[name] = entry;
        return entry;
    }
    return itr->second;
}

shared_ptr<PitEntry> Pit::findPitEntry(string name){
    std::lock_guard<std::mutex> lockFunction(lock);
    auto itr=entries.find(name);
    if(itr==entries.end()){
       return nullptr;
    }
    return itr->second;
}
void Pit::deletePitEntry(std::string name){
    std::lock_guard<std::mutex> lockFunction(lock);
    auto itr=entries.find(name);
    if(itr!=entries.end()){
        entries.erase(itr);
    }
}