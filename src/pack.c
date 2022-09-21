#include "pack.h"
FMP4_PRIVATE
void
fmp4_pack_uint64be(uint8_t* dest, uint64_t n) {
    dest[0] = ((n >> 56) & 0xFF );
    dest[1] = ((n >> 48) & 0xFF );
    dest[2] = ((n >> 40) & 0xFF );
    dest[3] = ((n >> 32) & 0xFF );
    dest[4] = ((n >> 24) & 0xFF );
    dest[5] = ((n >> 16) & 0xFF );
    dest[6] = ((n >>  8) & 0xFF );
    dest[7] = ((n >>  0) & 0xFF );
}

FMP4_PRIVATE
void
fmp4_pack_uint32be(uint8_t* dest, uint32_t n) {
    dest[0] = ((n >> 24) & 0xFF );
    dest[1] = ((n >> 16) & 0xFF );
    dest[2] = ((n >>  8) & 0xFF );
    dest[3] = ((n >>  0) & 0xFF );
}

FMP4_PRIVATE
void
fmp4_pack_uint24be(uint8_t* dest, uint32_t n) {
    dest[0] = ((n >> 16) & 0xFF );
    dest[1] = ((n >>  8) & 0xFF );
    dest[2] = ((n >>  0) & 0xFF );
}

FMP4_PRIVATE
void
fmp4_pack_uint16be(uint8_t* dest, uint16_t n) {
    dest[0] = ((n >>  8) & 0xFF );
    dest[1] = ((n >>  0) & 0xFF );
}


