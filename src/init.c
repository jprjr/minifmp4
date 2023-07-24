#include "init.h"
#include "mux.h"
#include "membuf.h"
#include "allocator.h"
#include "fmp4_string.h"

FMP4_API
void
fmp4_mux_init(fmp4_mux* mux, const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    mux->allocator = allocator;

    fmp4_membuf_init(&mux->buffer, mux->allocator);
    fmp4_membuf_init(&mux->stack,  mux->allocator);
    fmp4_membuf_init(&mux->brands, mux->allocator);
    fmp4_membuf_init(&mux->tracks, mux->allocator);
    fmp4_membuf_init(&mux->emsgs,  mux->allocator);
    fmp4_membuf_init(&mux->alloc_track, mux->allocator);
    fmp4_membuf_init(&mux->alloc_emsg,  mux->allocator);
    mux->brand_minor_version = 0;
    mux->fragments = 0;
    mux->moof_offset = 0;

    /* set a default major brand */
    fmp4_mux_set_brand_major(mux, "iso6", 0);
    return;
}

FMP4_API
void
fmp4_sample_flags_init(fmp4_sample_flags* flags) {
    flags->is_leading = 0;
    flags->depends_on = 0;
    flags->is_depended_on = 0;
    flags->has_redundancy = 0;
    flags->padding_value = 0;
    flags->is_non_sync = 0;
    flags->degradation_priority = 0;
    return;
}


FMP4_API
void
fmp4_sample_info_init(fmp4_sample_info* sample_info) {
    sample_info->duration = 0;
    sample_info->size = 0;
    sample_info->sample_group = 0;
    fmp4_sample_flags_init(&sample_info->flags);
    return;
}


FMP4_API
void
fmp4_track_init(fmp4_track *track, const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    track->allocator = allocator;

    track->stream_type = FMP4_STREAM_TYPE_FORBIDDEN;
    track->base_media_decode_time = 0;
    track->time_scale = 0;
    track->encoder_delay = 0;
    track->roll_distance = 0;
    track->roll_type = FMP4_ROLL_TYPE_ROLL;

    fmp4_sample_info_init(&track->default_sample_info);

    track->trun_sample_flags_set     = 0;
    track->trun_sample_flags         = 0;
    track->trun_sample_duration_set  = 0;
    track->trun_sample_duration      = 0;
    track->trun_sample_size_set      = 0;
    track->trun_sample_size          = 0;

    track->trun_sample_count       = 0;
    track->data_offset_pos         = 0;

    memset(track->language,0,sizeof(track->language));

    track->codec = FMP4_CODEC_UNDEFINED;
    track->object_type = FMP4_OBJECT_TYPE_FORBIDDEN;
    track->info.audio.channels = 0;

    fmp4_membuf_init(&track->sample_info, track->allocator);
    fmp4_membuf_init(&track->mdat,        track->allocator);
    fmp4_membuf_init(&track->dsi,         track->allocator);
    fmp4_membuf_init(&track->loudness,    track->allocator);
    fmp4_membuf_init(&track->alloc_loudness, track->allocator);
    return;
}

FMP4_API
void
fmp4_loudness_init(fmp4_loudness* loudness, const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    loudness->allocator = allocator;

    loudness->type = FMP4_LOUDNESS_UNDEF;
    loudness->downmix_id  = 0;
    loudness->drc_id      = 0;
    loudness->sample_peak = 0;
    loudness->true_peak   = 0;
    loudness->system      = 0;
    loudness->reliability = 0;

    fmp4_membuf_init(&loudness->measurements, loudness->allocator);
    fmp4_membuf_init(&loudness->alloc_measurement, loudness->allocator);
    return;
}

FMP4_API
void
fmp4_measurement_init(fmp4_measurement* measurement) {
    measurement->method = 0;
    measurement->value = 0;
    measurement->system = 0;
    measurement->reliability = 0;
    return;
}

FMP4_API
void
fmp4_emsg_init(fmp4_emsg* emsg, const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    emsg->allocator = allocator;

    emsg->version = 0;
    emsg->timescale = 0;
    emsg->presentation_time_delta = 0;
    emsg->presentation_time = 0;
    emsg->event_duration = 0;
    emsg->id = 0;
    fmp4_membuf_init(&emsg->scheme_id_uri, emsg->allocator);
    fmp4_membuf_init(&emsg->value,         emsg->allocator);
    fmp4_membuf_init(&emsg->message,       emsg->allocator);
    return;
}

FMP4_API
fmp4_mux*
fmp4_mux_new(const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    fmp4_mux* mux = (fmp4_mux*)allocator->malloc(sizeof(fmp4_mux),allocator->userdata);
    if(mux != NULL) fmp4_mux_init(mux, allocator);
    return mux;
}

FMP4_API
fmp4_track*
fmp4_track_new(const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    fmp4_track* track = (fmp4_track*)allocator->malloc(sizeof(fmp4_track),allocator->userdata);
    if(track != NULL) fmp4_track_init(track,allocator);
    return track;
}

FMP4_API
fmp4_loudness*
fmp4_loudness_new(const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    fmp4_loudness* loudness = (fmp4_loudness*)allocator->malloc(sizeof(fmp4_loudness),allocator->userdata);
    if(loudness != NULL) fmp4_loudness_init(loudness,allocator);
    return loudness;
}

FMP4_API
fmp4_sample_info*
fmp4_sample_info_new(const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    fmp4_sample_info* sample_info = (fmp4_sample_info*)allocator->malloc(sizeof(fmp4_sample_info),allocator->userdata);
    if(sample_info != NULL) {
        fmp4_sample_info_init(sample_info);
        sample_info->allocator = allocator;
    }
    return sample_info;
}

FMP4_API
fmp4_sample_flags*
fmp4_sample_flags_new(const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    fmp4_sample_flags* sample_flags = (fmp4_sample_flags*)allocator->malloc(sizeof(fmp4_sample_flags),allocator->userdata);
    if(sample_flags != NULL) {
        fmp4_sample_flags_init(sample_flags);
        sample_flags->allocator = allocator;
    }
    return sample_flags;
}

FMP4_API
fmp4_measurement*
fmp4_measurement_new(const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    fmp4_measurement* measurement = (fmp4_measurement*)allocator->malloc(sizeof(fmp4_measurement),allocator->userdata);
    if(measurement != NULL) {
        fmp4_measurement_init(measurement);
        measurement->allocator = allocator;
    }
    return measurement;
}

FMP4_API
fmp4_emsg*
fmp4_emsg_new(const fmp4_allocator* allocator) {
    if(allocator == NULL) allocator = &fmp4_default_allocator;
    fmp4_emsg* emsg = (fmp4_emsg*)allocator->malloc(sizeof(fmp4_emsg),allocator->userdata);
    if(emsg != NULL) fmp4_emsg_init(emsg,allocator);
    return emsg;
}
