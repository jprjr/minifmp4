#ifndef CLOSE_H
#define CLOSE_H

#include "defines.h"
#include "structs.h"

/* struct closure functions */

/* frees all allocated data used by the muxer - like buffer/scratch space,
 * all tracks allocated via fmp4_mux_new_track,
 * all emsgs allocated via fmp4_mux_new_emsg */

FMP4_API
void
fmp4_mux_close(fmp4_mux* mux);

/* frees all allocated data used by the track - dsi and samples,
 * and any loudness allocated via fmp4_track_new_loudness */
FMP4_API
void
fmp4_track_close(fmp4_track* track);

/* frees allocated data used by the loudness, and any
 * measurements allocated via fmp4_loudness_new_measurement */
FMP4_API
void
fmp4_loudness_close(fmp4_loudness* loudness);

/* frees allocated data used by the emsg */
FMP4_API
void
fmp4_emsg_close(fmp4_emsg* emsg);

/* closes and frees */
FMP4_API
void
fmp4_mux_free(fmp4_mux* mux);

FMP4_API
void
fmp4_track_free(fmp4_track *track);

FMP4_API
void
fmp4_sample_info_free(fmp4_sample_info* sample_info);

FMP4_API
void
fmp4_sample_flags_free(fmp4_sample_flags* sample_flags);

FMP4_API
void
fmp4_loudness_free(fmp4_loudness* loudness);

FMP4_API
void
fmp4_measurement_free(fmp4_measurement* measurement);

FMP4_API
void
fmp4_emsg_free(fmp4_emsg* emsg);

#endif
