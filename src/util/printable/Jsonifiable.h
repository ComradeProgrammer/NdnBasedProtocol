#ifndef __JSONIFIABLE_H_
#define __JSONIFIABLE_H_
#include "util/json/json.hpp"
#include "util/printable/Printable.h"
class Jsonfiable: public Printable{
    public:
    virtual nlohmann::json marshal()=0;
    virtual std::string toString()override{
        return marshal().dump();
    }
};

#endif
