#include "EthernetHeader.h"

EthernetHeader::EthernetHeader(const char* rawData) { memcpy(this, rawData, 14); }