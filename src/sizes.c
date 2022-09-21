#include "sizes.h"

FMP4_API
size_t
fmp4_mux_size(void) {
    return sizeof(fmp4_mux);
}

FMP4_API
size_t
fmp4_track_size(void) {
    return sizeof(fmp4_track);
}

FMP4_API
size_t
fmp4_sample_info_size(void) {
    return sizeof(fmp4_sample_info);
}

FMP4_API
size_t
fmp4_loudness_size(void) {
    return sizeof(fmp4_loudness);
}

FMP4_API
size_t
fmp4_measurement_size(void) {
    return sizeof(fmp4_measurement);
}

FMP4_API
size_t
fmp4_emsg_size(void) {
    return sizeof(fmp4_emsg);
}
