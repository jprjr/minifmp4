#include "flags.h"
FMP4_PRIVATE
uint32_t
fmp4_encode_sample_flags(const fmp4_sample_flags* flags) {
    return 0
           | ( (flags->is_leading & 0x03) << 26)
           | ( (flags->depends_on & 0x03) << 24)
           | ( (flags->is_depended_on & 0x03) << 22)
           | ( (flags->has_redundancy & 0x03) << 20)
           | ( (flags->padding_value & 0x07) << 17)
           | ( (flags->is_non_sync & 0x01) << 16)
           |    flags->degradation_priority;
}

FMP4_PRIVATE
void
fmp4_decode_sample_flags(uint32_t val, fmp4_sample_flags* flags) {
    flags->degradation_priority = (val >>  0) & 0xFFFF;
    flags->is_non_sync          = (val >> 16) & 0x01;
    flags->padding_value        = (val >> 17) & 0x07;
    flags->has_redundancy       = (val >> 20) & 0x03;
    flags->is_depended_on       = (val >> 22) & 0x03;
    flags->depends_on           = (val >> 24) & 0x03;
    flags->is_leading           = (val >> 26) & 0x03;
}
