#ifndef __PRINTABLE_H_
#define __PRINTABLE_H_
#include <string>
class Printable {
   public:
    virtual std::string toString() const = 0;
};

#endif