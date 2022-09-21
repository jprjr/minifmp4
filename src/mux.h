#ifndef MUX_H
#define MUX_H
#include "defines.h"
#include "structs.h"

/* allocates and adds a new track to the muxer, it will
 * automatically be freed whenever you close/free the muxer */
FMP4_API
fmp4_track*
fmp4_mux_new_track(fmp4_mux* mux);

/* allocates an emsg, does NOT add it to the next media
 * segment write. It will automatically be freed whenever you
 * close/free the muxer */
FMP4_API
fmp4_emsg*
fmp4_mux_new_emsg(fmp4_mux* mux);

/* adds an already-allocated track to the muxer, stores a reference to the track pointer,
 * you'll have to deallocate/free the track on your own */
FMP4_API
fmp4_result
fmp4_mux_add_track(fmp4_mux* mux, const fmp4_track* track);

/* adds an emsg to the muxer for the next segment write - the
 * emsg isn't serialized until you write the segment, so you can add the emsg
 * and update fields, add data, etc up until you call fmp4_mux_write_segment */
FMP4_API
fmp4_result
fmp4_mux_add_emsg(fmp4_mux* mux, const fmp4_emsg* emsg);

/* validates we have everything needed to write out an initialization segment */
FMP4_API
fmp4_result
fmp4_mux_validate_init(const fmp4_mux* mux);

/* write out an initialization segment */
FMP4_API
fmp4_result
fmp4_mux_write_init(fmp4_mux* mux, fmp4_write_cb write, void* userdata);

/* validates we have everything needed to write out a media segment */
FMP4_API
fmp4_result
fmp4_mux_validate_segment(const fmp4_mux* mux);

/* write out a media segment - this will clear all buffered samples from
 * tracks, and remove emsg references */
FMP4_API
fmp4_result
fmp4_mux_write_segment(fmp4_mux* mux, fmp4_write_cb write, void* userdata);

/* sets the major brand */
FMP4_API
void
fmp4_mux_set_brand_major(fmp4_mux* mux, const char brand[4], uint32_t ver);

/* add a brand to the compatible brand list - the major brand
 * is automatically included */
FMP4_API
fmp4_result
fmp4_mux_add_brand(fmp4_mux* mux, const char brand[4]);

#endif
