#include "mux.h"
#include "membuf.h"
#include "boxes.h"
#include "pack.h"
#include "fmp4_string.h"
#include "init.h"
#include "close.h"
#include "track.h"
#include "emsg.h"

FMP4_API
fmp4_track*
fmp4_mux_new_track(fmp4_mux* mux) {
    fmp4_track* track = fmp4_track_new(mux->allocator);
    if(track == NULL) return track;

    if(fmp4_mux_add_track(mux,track) != FMP4_OK) {
        fmp4_track_free(track);
        track = NULL;
    }
    return track;
}

FMP4_API
fmp4_emsg*
fmp4_mux_new_emsg(fmp4_mux* mux) {
    fmp4_emsg* emsg = fmp4_emsg_new(mux->allocator);
    if(emsg == NULL) return emsg;

    if(fmp4_mux_add_emsg(mux,emsg) != FMP4_OK) {
        fmp4_emsg_free(emsg);
        emsg = NULL;
    }
    return emsg;
}

FMP4_API
fmp4_result
fmp4_mux_add_track(fmp4_mux* mux, const fmp4_track* track) {
    return fmp4_membuf_cat(&mux->tracks,&track,sizeof(fmp4_track*));
}

FMP4_API
fmp4_result
fmp4_mux_add_brand(fmp4_mux* mux, const char brand[4]) {
    return fmp4_membuf_cat(&mux->brands, brand, 4);
}

FMP4_API
void
fmp4_mux_set_brand_major(fmp4_mux* mux, const char brand[4], uint32_t ver) {
    memcpy(mux->brand_major, brand, 4);
    mux->brand_minor_version = ver;
    return;
}

FMP4_API
fmp4_result
fmp4_mux_write_init(fmp4_mux* mux, fmp4_write_cb write, void* userdata) {
    fmp4_result res;

    if( (res = fmp4_mux_validate_init(mux)) != FMP4_OK) goto cleanup;

    if( (res = fmp4_box_ftyp(mux)) != FMP4_OK) goto cleanup;
    if( (res = fmp4_box_moov(mux)) != FMP4_OK) goto cleanup;

    if(write(mux->buffer.x,mux->buffer.len,userdata) != mux->buffer.len) {
        res = FMP4_WRITEERR;
        goto cleanup;
    }

    res = FMP4_OK;

    cleanup:

    fmp4_membuf_reset(&mux->buffer);
    fmp4_membuf_reset(&mux->stack);

    return FMP4_OK;
}

FMP4_API
fmp4_result
fmp4_mux_write_segment(fmp4_mux* mux, fmp4_write_cb write, void* userdata) {
    fmp4_result res;
    size_t i;
    size_t len;
    fmp4_emsg** emsgs;
    fmp4_track** tracks;

    if( (res = fmp4_mux_validate_segment(mux)) != FMP4_OK) goto cleanup;

    tracks = (fmp4_track**)mux->tracks.x;
    emsgs = (fmp4_emsg**)mux->emsgs.x;

    if( (res = fmp4_box_styp(mux)) != FMP4_OK) goto cleanup;

    len = mux->emsgs.len / sizeof(fmp4_emsg*);
    for(i=0;i<len;i++) {
        if( (res = fmp4_box_emsg(mux, emsgs[i])) != FMP4_OK) goto cleanup;
    }

    if( (res = fmp4_box_moof(mux)) != FMP4_OK) goto cleanup;
    if( (res = fmp4_box_mdat(mux)) != FMP4_OK) goto cleanup;

    if(write(mux->buffer.x,mux->buffer.len,userdata) != mux->buffer.len) {
        res = FMP4_WRITEERR;
        goto cleanup;
    }

    res = FMP4_OK;

    cleanup:

    len = mux->tracks.len / sizeof(fmp4_track*);
    for(i=0;i<len;i++) {
        fmp4_membuf_reset(&tracks[i]->mdat);
        fmp4_membuf_reset(&tracks[i]->sample_info);
    }

    fmp4_membuf_reset(&mux->buffer);
    fmp4_membuf_reset(&mux->stack);
    fmp4_membuf_reset(&mux->emsgs);

    return res;
}

FMP4_API
fmp4_result
fmp4_mux_add_emsg(fmp4_mux* mux, const fmp4_emsg* emsg) {
    return fmp4_membuf_cat(&mux->emsgs,&emsg,sizeof(fmp4_emsg*));
}

FMP4_API
fmp4_result
fmp4_mux_validate_init(const fmp4_mux* mux) {
    fmp4_result res;
    fmp4_track** tracks;
    size_t i;
    size_t len;

    if(mux->tracks.len == 0) return FMP4_NOTRACKS;

    tracks = (fmp4_track**)mux->tracks.x;
    len = mux->tracks.len / sizeof(fmp4_track*);

    for(i=0;i<len;i++) {
        if( (res = fmp4_track_validate_init(tracks[i])) != FMP4_OK) return res;
    }

    return FMP4_OK;
}

FMP4_API
fmp4_result
fmp4_mux_validate_segment(const fmp4_mux* mux) {
    fmp4_result res;
    fmp4_track** tracks;
    fmp4_emsg** emsgs;
    size_t i;
    size_t len;
    size_t totalsamples;

    tracks = (fmp4_track**)mux->tracks.x;
    len = mux->tracks.len / sizeof(fmp4_track*);
    totalsamples = 0;

    for(i=0;i<len;i++) {
        totalsamples += tracks[i]->sample_info.len / sizeof(fmp4_sample_info);
        res = fmp4_track_validate_segment(tracks[i]);
        switch(res) {
            case FMP4_OK: /* fall-through */
            case FMP4_NOSAMPLES: break;
            default: return res;
        }
    }
    if(totalsamples == 0) return FMP4_NOSAMPLES;

    emsgs = (fmp4_emsg**)mux->emsgs.x;
    len = mux->emsgs.len / sizeof(fmp4_emsg*);
    for(i=0;i<len;i++) {
        if( (res = fmp4_emsg_validate(emsgs[i])) != FMP4_OK) return res;
    }

    return FMP4_OK;
}

