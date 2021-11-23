#include"RoutingTableItem.h"
using namespace std;
RoutingTableItem::RoutingTableItem(string info,std::shared_ptr<Logger>_logger){
    logger=Logger::getDefaultLoggerIfNull(_logger);
    regex pattern("\\s*([0-9.]+)\\s+([0-9.]+)\\s+([0-9.]+)\\s+\\w+\\s+\\w+\\s+\\w+\\s+\\w+\\s+\\w+\\s*");
    smatch result;
    if(!regex_match(info,result,pattern)){
        logger->ERRORF("failed to parse info string %s",info.c_str());
        return ;
    }
    destination.setIp(result[1].str());
    nextHop.setIp(result[2].str());
    mask.setIp(result[3].str());
}