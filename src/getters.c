#include "getters.h"
#include "flags.h"
#include "fmp4_string.h"
#include "fmp4_math.h"

FMP4_API
fmp4_result
fmp4_mux_get_brand_major(const fmp4_mux* mux, char brand[4], uint32_t* ver) {
    memcpy(brand,mux->brand_major,sizeof(char)*4);
    if(ver != NULL) *ver = mux->brand_minor_version;
    return FMP4_OK;
}

FMP4_API
uint32_t
fmp4_mux_get_brand_minor_version(const fmp4_mux* mux) {
    return mux->brand_minor_version;
}

FMP4_API
const char*
fmp4_mux_get_brands(const fmp4_mux* mux, size_t* len) {
    if(mux->brands.len == 0) return NULL;
    *len = mux->brands.len / sizeof(char);
    return (const char *)mux->brands.x;
}

FMP4_API
fmp4_stream_type
fmp4_track_get_stream_type(const fmp4_track* track) {
    return track->stream_type;
}

FMP4_API
fmp4_codec
fmp4_track_get_codec(const fmp4_track* track) {
    return track->codec;
}

FMP4_API
fmp4_object_type
fmp4_track_get_object_type(const fmp4_track* track) {
    return track->object_type;
}

FMP4_API
uint64_t
fmp4_track_get_base_media_decode_time(const fmp4_track* track) {
    return track->base_media_decode_time;
}

FMP4_API
uint32_t
fmp4_track_get_time_scale(const fmp4_track* track) {
    return track->time_scale;
}

FMP4_API
const char*
fmp4_track_get_language(const fmp4_track* track) {
    if(track->language[0] == '\0') return NULL;
    return (const char *)track->language;
}

FMP4_API
uint16_t
fmp4_track_get_audio_channels(const fmp4_track* track) {
    return track->info.audio.channels;
}

FMP4_API
uint32_t
fmp4_track_get_encoder_delay(const fmp4_track* track) {
    return track->encoder_delay;
}

FMP4_API
int16_t
fmp4_track_get_roll_distance(const fmp4_track* track) {
    return track->roll_distance;
}

FMP4_API
fmp4_result
fmp4_track_get_default_sample_info(const fmp4_track *track, fmp4_sample_info* info) {
    memcpy(info,&track->default_sample_info,sizeof(fmp4_sample_info));
    return FMP4_OK;
}


FMP4_API
const void*
fmp4_track_get_dsi(const fmp4_track* track, size_t* len) {
    if(len != NULL) *len = track->dsi.len;
    return track->dsi.x;
}

FMP4_API
fmp4_result
fmp4_track_get_first_sample_flags(const fmp4_track* track, fmp4_sample_flags* flags) {
    fmp4_decode_sample_flags(track->first_sample_flags, flags);
    return FMP4_OK;
}

FMP4_API
uint8_t
fmp4_track_get_trun_sample_flags_set(const fmp4_track* track) {
    return track->trun_sample_flags_set;
}

FMP4_API
fmp4_result
fmp4_track_get_trun_sample_flags(const fmp4_track* track, fmp4_sample_flags* flags) {
    fmp4_decode_sample_flags(track->trun_sample_flags, flags);
    return FMP4_OK;
}

FMP4_API
uint8_t
fmp4_track_get_trun_sample_duration_set(const fmp4_track* track) {
    return track->trun_sample_duration_set;
}

FMP4_API
uint32_t
fmp4_track_get_trun_sample_duration(const fmp4_track* track) {
    return track->trun_sample_duration;
}

FMP4_API
uint8_t
fmp4_track_get_trun_sample_size_set(const fmp4_track* track) {
    return track->trun_sample_size_set;
}

FMP4_API
uint32_t
fmp4_track_get_trun_sample_size(const fmp4_track* track) {
    return track->trun_sample_size;
}

FMP4_API
uint64_t
fmp4_track_get_trun_sample_count(const fmp4_track* track) {
    return track->trun_sample_count;
}

FMP4_API
fmp4_loudness_type
fmp4_loudness_get_type(const fmp4_loudness* loudness) {
    return loudness->type;
}

FMP4_API
uint8_t
fmp4_loudness_get_downmix_id(const fmp4_loudness* loudness) {
    return loudness->downmix_id;
}

FMP4_API
uint8_t
fmp4_loudness_get_drc_id(const fmp4_loudness* loudness) {
    return loudness->drc_id;
}

FMP4_API
double
fmp4_loudness_get_bs_sample_peak_level(const fmp4_loudness* loudness) {
    if(loudness->sample_peak == 0) return INFINITY;
    return (((double)loudness->sample_peak) / -32.0f) + 20.0f;
}

FMP4_API
double
fmp4_loudness_get_bs_true_peak_level(const fmp4_loudness* loudness) {
    if(loudness->true_peak == 0) return INFINITY;
    return (((double)loudness->true_peak) / -32.0f) + 20.0f;
}

FMP4_API
uint8_t
fmp4_loudness_get_system(const fmp4_loudness* loudness) {
    return loudness->system;
}

FMP4_API
uint8_t
fmp4_loudness_get_reliability(const fmp4_loudness* loudness) {
    return loudness->reliability;
}

FMP4_API
uint8_t
fmp4_measurement_get_method(const fmp4_measurement* measurement) {
    return measurement->method;
}

FMP4_PRIVATE
double
fmp4_decode_loudness_range(uint8_t val) {
    if(val <= 128) {
        return (((double)val) - 0.5f) / 4.0f;
    } else if(val <= 204) {
        return ((((double)val) - 128.5f) / 2.0f) + 32.0f;
    }
    return (((double)val) - 134.5f);
}

FMP4_API
double
fmp4_measurement_get_value(const fmp4_measurement* measurement) {
    switch(measurement->method) {
        case 1: /* fall-through */
        case 2: /* fall-through */
        case 3: /* fall-through */
        case 4: /* fall-through */
        case 5: return (((double)measurement->value) / 4.0f) - 57.75f;
        case 6: return fmp4_decode_loudness_range(measurement->value);
        case 7: return ((double)measurement->value) - 80.0f;
        case 8: return ((double)measurement->value);
        default: break;
    }
    return INFINITY;
}

FMP4_API
uint8_t
fmp4_measurement_get_system(const fmp4_measurement* measurement) {
    return measurement->system;
}

FMP4_API
uint8_t
fmp4_measurement_get_reliability(const fmp4_measurement* measurement) {
    return measurement->reliability;
}

FMP4_API
uint8_t
fmp4_sample_flags_get_is_leading(const fmp4_sample_flags* flags) {
    return flags->is_leading;
}

FMP4_API
uint8_t
fmp4_sample_flags_get_depends_on(const fmp4_sample_flags* flags) {
    return flags->depends_on;
}

FMP4_API
uint8_t
fmp4_sample_flags_get_is_depended_on(const fmp4_sample_flags* flags) {
    return flags->is_depended_on;
}

FMP4_API
uint8_t
fmp4_sample_flags_get_has_redundancy(const fmp4_sample_flags* flags) {
    return flags->has_redundancy;
}

FMP4_API
uint8_t
fmp4_sample_flags_get_padding_value(const fmp4_sample_flags* flags) {
    return flags->padding_value;
}

FMP4_API
uint8_t
fmp4_sample_flags_get_is_non_sync(const fmp4_sample_flags* flags) {
    return flags->is_non_sync;
}

FMP4_API
uint16_t
fmp4_sample_flags_get_degradation_priority(const fmp4_sample_flags* flags) {
    return flags->degradation_priority;
}

FMP4_API
uint32_t
fmp4_sample_info_get_duration(const fmp4_sample_info* info) {
    return info->duration;
}

FMP4_API
uint32_t
fmp4_sample_info_get_size(const fmp4_sample_info* info) {
    return info->size;
}

FMP4_API
fmp4_result
fmp4_sample_info_get_flags(const fmp4_sample_info* info, fmp4_sample_flags* flags) {
    memcpy(flags,&info->flags,sizeof(fmp4_sample_flags));
    return FMP4_OK;
}

FMP4_API
uint8_t
fmp4_emsg_get_version(const fmp4_emsg* emsg) {
    return emsg->version;
}

FMP4_API
uint32_t
fmp4_emsg_get_timescale(const fmp4_emsg* emsg) {
    return emsg->timescale;
}

FMP4_API
uint32_t
fmp4_emsg_get_presentation_time_delta(const fmp4_emsg* emsg) {
    return emsg->presentation_time_delta;
}

FMP4_API
uint64_t
fmp4_emsg_get_presentation_time(const fmp4_emsg* emsg) {
    return emsg->presentation_time;
}

FMP4_API
uint32_t
fmp4_emsg_get_event_duration(const fmp4_emsg* emsg) {
    return emsg->event_duration;
}

FMP4_API
uint32_t
fmp4_emsg_get_id(const fmp4_emsg* emsg) {
    return emsg->id;
}

FMP4_API
const char*
fmp4_emsg_get_scheme_id_uri(const fmp4_emsg* emsg) {
    return (const char *)emsg->scheme_id_uri.x;
}

FMP4_API
const char*
fmp4_emsg_get_value(const fmp4_emsg* emsg) {
    return (const char *)emsg->value.x;
}

FMP4_API
const uint8_t*
fmp4_emsg_get_message(const fmp4_emsg* emsg, uint32_t* message_size) {
    if(message_size != NULL) *message_size = emsg->message.len;
    return emsg->message.x;
}

