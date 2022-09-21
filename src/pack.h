#ifndef PACK_H
#define PACK_H
#include "defines.h"
#include "int.h"

FMP4_PRIVATE
void
fmp4_pack_uint64be(uint8_t* dest, uint64_t n);

FMP4_PRIVATE
void
fmp4_pack_uint32be(uint8_t* dest, uint32_t n);

FMP4_PRIVATE
void
fmp4_pack_uint24be(uint8_t* dest, uint32_t n);

FMP4_PRIVATE
void
fmp4_pack_uint16be(uint8_t* dest, uint16_t n);

#endif
