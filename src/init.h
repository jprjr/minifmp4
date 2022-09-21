#ifndef INIT_H
#define INIT_H
#include "defines.h"
#include "structs.h"

/* struct initialization functions */
FMP4_API
void
fmp4_mux_init(fmp4_mux* mux, const fmp4_allocator* allocator);

FMP4_API
void
fmp4_track_init(fmp4_track* track, const fmp4_allocator* allocator);

FMP4_API
void
fmp4_sample_info_init(fmp4_sample_info* sample_info);

FMP4_API
void
fmp4_sample_flags_init(fmp4_sample_flags* sample_flags);

FMP4_API
void
fmp4_loudness_init(fmp4_loudness* loudness, const fmp4_allocator* allocator);

FMP4_API
void
fmp4_measurement_init(fmp4_measurement* measurement);

FMP4_API
void
fmp4_emsg_init(fmp4_emsg* emsg, const fmp4_allocator* allocator);

/* struct allocation functions (pre-initialized) */
FMP4_API
fmp4_mux*
fmp4_mux_new(const fmp4_allocator* allocator);

FMP4_API
fmp4_track*
fmp4_track_new(const fmp4_allocator* allocator);

FMP4_API
fmp4_sample_info*
fmp4_sample_info_new(const fmp4_allocator* allocator);

FMP4_API
fmp4_sample_flags*
fmp4_sample_flags_new(const fmp4_allocator* allocator);

FMP4_API
fmp4_loudness*
fmp4_loudness_new(const fmp4_allocator* allocator);

FMP4_API
fmp4_measurement*
fmp4_measurement_new(const fmp4_allocator* allocator);

FMP4_API
fmp4_emsg*
fmp4_emsg_new(const fmp4_allocator* allocator);
#endif
