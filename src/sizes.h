#ifndef SIZES_H
#define SIZES_H
#include "defines.h"
#include "structs.h"

/* functions for querying the size of structures
 * at runtime instead of compile-time */

FMP4_API
size_t
fmp4_mux_size(void);

FMP4_API
size_t
fmp4_track_size(void);

FMP4_API
size_t
fmp4_sample_info_size(void);

FMP4_API
size_t
fmp4_loudness_size(void);

FMP4_API
size_t
fmp4_measurement_size(void);

FMP4_API
size_t
fmp4_emsg_size(void);
#endif
