#ifndef __DECLARATION_H_
#define __DECLARATION_H_
#define NDN_PROTOCOL 0xffff

#define MTU 1500

// TLV-TYPE number assignments
// Packet types
#define TLV_INTEREST 5
#define TLV_DATA 6
// Ndn Packet attributes
#define TLV_NAME 7
// Ndn interest attributes
#define TLV_CANBEPREFIX 33
#define TLV_MUSTBEFRESHED 18
#define TLV_NONCE 10
#define TLV_HOPLIMIT 34
#define TLV_APLICATIONPARAMETERS 36
// Ndn data attributes
#define TLV_CONTENT 21

#define PIT_EXPIRE_DURATION 6000

#define NDN_ROUTING -1
#define NDN_ADDRASSIGNMENT -2


#define NDN_ROUTING_MAX_AGE 3600
#define NDN_ROUTING_MAX_SEQ 0x7fffffff
#define NDN_ROUTING_LS_REFRESH 1800
#define NDN_ROUTING_HOP_LIMIT 32
#endif