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

#endif