#include"util/log/Logger.h"
#include"util/timer/Timer.h"
#include "util/log/TerminalLogger.h"
#include "util/log/FileLogger.h"
#include <memory>
#include <unistd.h> 
using namespace std;
int main(){
    Logger::init(make_shared<FileLogger>("1.log"));
    auto timer=Timer::getTimer();
    timer->startTimer("test",500,[](string)->bool{
        LOGGER->INFO("here");
        return true;
    });
    while(1){
        sleep(1);
    }
}