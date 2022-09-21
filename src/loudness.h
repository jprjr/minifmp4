#ifndef LOUDNESS_H
#define LOUDNESS_H
#include "defines.h"
#include "structs.h"

/* allocates a new loudness measurement, and adds it to the loudness struct */
FMP4_API
fmp4_measurement*
fmp4_loudness_new_measurement(fmp4_loudness* loudness);

/* add a pre-allocated measurement to a loudness */
FMP4_API
fmp4_result
fmp4_loudness_add_measurement(fmp4_loudness* loudness, const fmp4_measurement* measurement);

/* validates a loudness */
FMP4_API
fmp4_result
fmp4_loudness_validate(const fmp4_loudness* loudness);
#endif
