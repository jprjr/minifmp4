#ifndef FLAGS_H
#define FLAGS_H
#include "defines.h"
#include "structs.h"

FMP4_PRIVATE
uint32_t
fmp4_encode_sample_flags(const fmp4_sample_flags* flags);

FMP4_PRIVATE
void
fmp4_decode_sample_flags(uint32_t val, fmp4_sample_flags* flags);

#endif
