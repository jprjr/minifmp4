#ifndef TRACK_H
#define TRACK_H
#include "defines.h"
#include "structs.h"

/* buffer a sample - the date and sample_info are buffered, it's safe to free or re-use
 * those pointers after calling this */
FMP4_API
fmp4_result
fmp4_track_add_sample(fmp4_track* track, const void* data, const fmp4_sample_info* sample);

/* allocate a new fmp4_loudness pointer and add it to the track, you'll need
 * to free the pointer whenever you're done with it. It's safe to free it after
 * calling fmp4_mux_write_init - it won't be used again */
FMP4_API
fmp4_loudness*
fmp4_track_new_loudness(fmp4_track* track);

/* add an already-allocated loudness pointer to the track */
FMP4_API
fmp4_result
fmp4_track_add_loudness(fmp4_track* track, const fmp4_loudness* loudness);

/* validates the track has valid init-related data */
FMP4_API
fmp4_result
fmp4_track_validate_init(const fmp4_track* track);

/* validates the track has valid segment-related data */
FMP4_API
fmp4_result
fmp4_track_validate_segment(const fmp4_track* track);

#endif
