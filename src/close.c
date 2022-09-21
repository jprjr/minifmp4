#include "close.h"
#include "init.h"
#include "membuf.h"
#include "allocator.h"

FMP4_API
void
fmp4_mux_close(fmp4_mux* mux) {
    size_t i;
    size_t len;

    fmp4_track** tracks;
    fmp4_emsg** emsgs;

    fmp4_membuf_free(&mux->tracks);
    fmp4_membuf_free(&mux->buffer);
    fmp4_membuf_free(&mux->stack);
    fmp4_membuf_free(&mux->brands);
    fmp4_membuf_free(&mux->emsgs);

    tracks = (fmp4_track**)mux->alloc_track.x;
    len = mux->alloc_track.len / sizeof(fmp4_track*);
    for(i=0;i<len;i++) {
        fmp4_track_free(tracks[i]);
    }

    emsgs = (fmp4_emsg**)mux->alloc_emsg.x;
    len = mux->alloc_emsg.len / sizeof(fmp4_emsg*);
    for(i=0;i<len;i++) {
        fmp4_emsg_free(emsgs[i]);
    }

    fmp4_membuf_free(&mux->alloc_track);
    fmp4_membuf_free(&mux->alloc_emsg);

    return;
}

FMP4_API
void
fmp4_track_close(fmp4_track *track) {
    size_t i;
    size_t len;

    fmp4_loudness** loudnesses;

    fmp4_membuf_free(&track->sample_info);
    fmp4_membuf_free(&track->mdat);
    fmp4_membuf_free(&track->loudness);
    fmp4_membuf_free(&track->dsi);

    loudnesses = (fmp4_loudness**)track->alloc_loudness.x;
    len = track->alloc_loudness.len / sizeof(fmp4_loudness*);
    for(i=0;i<len;i++) {
        fmp4_loudness_free(loudnesses[i]);
    }
    fmp4_membuf_free(&track->alloc_loudness);

    return;
}

FMP4_API
void
fmp4_loudness_close(fmp4_loudness* loudness) {
    size_t i;
    size_t len;

    fmp4_measurement** measurements;

    fmp4_membuf_free(&loudness->measurements);

    measurements = (fmp4_measurement**)loudness->alloc_measurement.x;
    len = loudness->alloc_measurement.len / sizeof(fmp4_measurement*);
    for(i=0;i<len;i++) {
        fmp4_measurement_free(measurements[i]);
    }
    fmp4_membuf_free(&loudness->alloc_measurement);
    return;
}

FMP4_API
void
fmp4_emsg_close(fmp4_emsg* emsg) {
    fmp4_membuf_free(&emsg->scheme_id_uri);
    fmp4_membuf_free(&emsg->value);
    fmp4_membuf_free(&emsg->message);
    return;
}

FMP4_API
void
fmp4_mux_free(fmp4_mux* mux) {
    const fmp4_allocator *allocator = mux->allocator;
    if(allocator == NULL) allocator = &fmp4_default_allocator;

    fmp4_mux_close(mux);
    allocator->free(mux,allocator->userdata);
    return;
}

FMP4_API
void
fmp4_track_free(fmp4_track *track) {
    const fmp4_allocator *allocator = track->allocator;
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    fmp4_track_close(track);
    allocator->free(track,allocator->userdata);
    return;
}

FMP4_API
void
fmp4_sample_info_free(fmp4_sample_info* sample_info) {
    const fmp4_allocator *allocator = sample_info->allocator;
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    allocator->free(sample_info,allocator->userdata);
    return;
}

FMP4_API
void
fmp4_sample_flags_free(fmp4_sample_flags* sample_flags) {
    const fmp4_allocator *allocator = sample_flags->allocator;
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    allocator->free(sample_flags,allocator->userdata);
    return;
}

FMP4_API
void
fmp4_loudness_free(fmp4_loudness* loudness) {
    const fmp4_allocator *allocator = loudness->allocator;
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    fmp4_loudness_close(loudness);
    allocator->free(loudness,allocator->userdata);
    return;
}

FMP4_API
void
fmp4_measurement_free(fmp4_measurement* measurement) {
    const fmp4_allocator *allocator = measurement->allocator;
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    allocator->free(measurement,allocator->userdata);
    return;
}

FMP4_API
void
fmp4_emsg_free(fmp4_emsg* emsg) {
    const fmp4_allocator *allocator = emsg->allocator;
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    fmp4_emsg_close(emsg);
    allocator->free(emsg,allocator->userdata);
    return;
}
