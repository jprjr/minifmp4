#include "setters.h"
#include "membuf.h"
#include "flags.h"
#include "fmp4_math.h"
#include "fmp4_string.h"

FMP4_API
void
fmp4_track_set_stream_type(fmp4_track* track, fmp4_stream_type stream_type) {
    track->stream_type = stream_type;
    return;
}

FMP4_API
void
fmp4_track_set_codec(fmp4_track* track, fmp4_codec codec) {
    track->codec = codec;
    return;
}

FMP4_API
void
fmp4_track_set_object_type(fmp4_track* track, fmp4_object_type object_type) {
    track->object_type = object_type;
    return;
}

FMP4_API
void
fmp4_track_set_base_media_decode_time(fmp4_track* track, uint64_t base_media_decode_time) {
    track->base_media_decode_time = base_media_decode_time;
    return;
}

FMP4_API
void
fmp4_track_set_time_scale(fmp4_track* track, uint32_t time_scale) {
    track->time_scale = time_scale;
    return;
}

FMP4_API
void
fmp4_track_set_language(fmp4_track* track, const char* language) {
    size_t l = strlen(language);
    if(l > 3) l = l;
    memcpy(track->language, language, 3);
    track->language[3] = '\0';
    return;
}

FMP4_API
void
fmp4_track_set_encoder_delay(fmp4_track* track, uint32_t delay) {
    track->encoder_delay = delay;
}

FMP4_API
void
fmp4_track_set_roll_distance(fmp4_track* track, int16_t distance) {
    track->roll_distance = distance;
}

FMP4_API
void
fmp4_track_set_roll_type(fmp4_track* track, fmp4_roll_type roll_type) {
    track->roll_type = roll_type;
}

FMP4_API
void
fmp4_track_set_audio_channels(fmp4_track* track, uint16_t channels) {
    track->info.audio.channels = channels;
    return;
}


FMP4_API
void
fmp4_track_set_default_sample_info(fmp4_track *track, const fmp4_sample_info* info) {
    memcpy(&track->default_sample_info,info,sizeof(fmp4_sample_info));
    return;
}

FMP4_API
fmp4_result
fmp4_track_set_dsi(fmp4_track* track, const void* dsi, size_t len) {
    track->dsi.len = 0;
    return fmp4_membuf_cat(&track->dsi,dsi,len);
}

FMP4_API
void
fmp4_track_set_first_sample_flags(fmp4_track* track, const fmp4_sample_flags* flags) {
    track->first_sample_flags = fmp4_encode_sample_flags(flags);
    return;
}

FMP4_API
void
fmp4_track_set_trun_sample_flags(fmp4_track* track, const fmp4_sample_flags* flags) {
    track->trun_sample_flags_set = 1;
    track->trun_sample_flags = fmp4_encode_sample_flags(flags);
    return;
}

FMP4_API
void
fmp4_track_set_trun_sample_duration(fmp4_track* track, uint32_t duration) {
    track->trun_sample_duration_set = 1;
    track->trun_sample_duration = duration;
    return;
}

FMP4_API
void
fmp4_track_set_trun_sample_size(fmp4_track* track, uint32_t size) {
    track->trun_sample_size_set = 1;
    track->trun_sample_size = size;
    return;
}

FMP4_API
void
fmp4_track_set_trun_sample_count(fmp4_track* track, uint64_t count) {
    track->trun_sample_count = count;
    return;
}

FMP4_API
void
fmp4_loudness_set_downmix_id(fmp4_loudness* loudness, uint8_t id) {
    loudness->downmix_id = id;
    return;
}

FMP4_API
void
fmp4_loudness_set_drc_id(fmp4_loudness* loudness, uint8_t id) {
    loudness->drc_id = id;
    return;
}

FMP4_API
void
fmp4_loudness_set_type(fmp4_loudness* loudness, fmp4_loudness_type type) {
    loudness->type = type;
    return;
}

FMP4_API
fmp4_result
fmp4_loudness_set_sample_peak(fmp4_loudness* loudness, double peak) {
    int64_t tmp;
    if(isinf(peak) || isnan(peak)) {
        loudness->sample_peak = 0;
    }
    else {
        tmp = (int64_t)((peak - 20.0f) * -32.0f);
        if(tmp < 0 || tmp > 0xFFFF) return FMP4_PEAKINVALID;
        loudness->sample_peak = (uint16_t)tmp;
    }
    return FMP4_OK;
}

FMP4_API
fmp4_result
fmp4_loudness_set_true_peak(fmp4_loudness* loudness, double peak) {
    int64_t tmp;
    if(isinf(peak) || isnan(peak)) {
        loudness->true_peak = 0;
    }
    else {
        tmp = (int64_t)((peak - 20.0f) * -32.0f);
        if(tmp < 0 || tmp > 0xFFFF) return FMP4_PEAKINVALID;
        loudness->true_peak = (uint16_t)tmp;
    }
    return FMP4_OK;
}

FMP4_API
fmp4_result
fmp4_loudness_set_system(fmp4_loudness* loudness, uint8_t system) {
    if(system > 5) return FMP4_SYSTEMINVALID;
    loudness->system = system;
    return FMP4_OK;
}

FMP4_API
fmp4_result
fmp4_loudness_set_reliability(fmp4_loudness* loudness, uint8_t reliability) {
    if(reliability > 3) return FMP4_RELIABILITYINVALID;
    loudness->reliability = reliability;
    return FMP4_OK;
}

FMP4_API
fmp4_result
fmp4_measurement_set_method(fmp4_measurement* measurement, uint8_t method) {
    if(method > 8) return FMP4_METHODINVALID;
    measurement->method = method;
    return FMP4_OK;
}

FMP4_PRIVATE
uint8_t
fmp4_encode_loudness_range(double val) {
    if(val < 0.0f) return 0;
    if(val <= 32.0f) return (uint8_t)(4.0f * val + 0.5f);
    if(val <= 70.0f) return ((uint8_t)(2.0f * (val - 32) + 0.5f)) + 128;
    if(val <= 121.0f) return (uint8_t)((val - 70.0f) + 0.5f) + 204;
    return 255;
}

FMP4_API
fmp4_result
fmp4_measurement_set_value(fmp4_measurement* measurement, double value) {
    switch(measurement->method) {
        case 1: /* fall-through */
        case 2: /* fall-through */
        case 3: /* fall-through */
        case 4: /* fall-through */
        case 5: measurement->value = (uint8_t)((value + 57.75) * 4.0f); break;
        case 6: measurement->value = fmp4_encode_loudness_range(value); break;
        case 7: measurement->value = (uint8_t)(value - 80.0f); break;
        case 8: measurement->value = (uint8_t)value; break;
        default: return FMP4_METHODINVALID;
    }
    return FMP4_OK;
}

FMP4_API
fmp4_result
fmp4_measurement_set_system(fmp4_measurement* measurement, uint8_t system) {
    if(system > 5) return FMP4_SYSTEMINVALID;
    measurement->system = system;
    return FMP4_OK;
}

FMP4_API
fmp4_result
fmp4_measurement_set_reliability(fmp4_measurement* measurement, uint8_t reliability) {
    if(reliability > 3) return FMP4_RELIABILITYINVALID;
    measurement->reliability = reliability;
    return FMP4_OK;
}

FMP4_API
void
fmp4_sample_flags_set_is_leading(fmp4_sample_flags* flags, uint8_t is_leading) {
    flags->is_leading = is_leading;
    return;
}

FMP4_API
void
fmp4_sample_flags_set_depends_on(fmp4_sample_flags* flags, uint8_t depends_on) {
    flags->depends_on = depends_on;
    return;
}

FMP4_API
void
fmp4_sample_flags_set_is_depended_on(fmp4_sample_flags* flags, uint8_t is_depended_on) {
    flags->is_depended_on = is_depended_on;
    return;
}

FMP4_API
void
fmp4_sample_flags_set_has_redundancy(fmp4_sample_flags* flags, uint8_t has_redundancy) {
    flags->has_redundancy = has_redundancy;
    return;
}

FMP4_API
void
fmp4_sample_flags_set_padding_value(fmp4_sample_flags* flags, uint8_t padding_value) {
    flags->padding_value = padding_value;
    return;
}

FMP4_API
void
fmp4_sample_flags_set_is_non_sync(fmp4_sample_flags* flags, uint8_t is_non_sync) {
    flags->is_non_sync = is_non_sync;
    return;
}

FMP4_API
void
fmp4_sample_flags_set_degradation_priority(fmp4_sample_flags* flags, uint16_t priority) {
    flags->degradation_priority = priority;
    return;
}

FMP4_API
void
fmp4_sample_info_set_duration(fmp4_sample_info* info, uint32_t duration) {
    info->duration = duration;
    return;
}

FMP4_API
void
fmp4_sample_info_set_size(fmp4_sample_info* info, uint32_t size) {
    info->size = size;
    return;
}

FMP4_API
void
fmp4_sample_info_set_sample_group(fmp4_sample_info* info, uint32_t sample_group) {
    info->sample_group = sample_group;
    return;
}

FMP4_API
void
fmp4_sample_info_set_flags(fmp4_sample_info* info, const fmp4_sample_flags* flags) {
    memcpy(&info->flags,flags,sizeof(fmp4_sample_flags));
    return;
}

FMP4_API
fmp4_result
fmp4_emsg_set_version(fmp4_emsg* emsg, uint8_t version) {
    if(version > 1) return FMP4_INVALIDEMSGVER;
    emsg->version = version;
    return FMP4_OK;
}

FMP4_API
void
fmp4_emsg_set_timescale(fmp4_emsg* emsg, uint32_t timescale) {
    emsg->timescale = timescale;
    return;
}

FMP4_API
void
fmp4_emsg_set_presentation_time_delta(fmp4_emsg* emsg, uint32_t presentation_time_delta) {
    emsg->presentation_time_delta = presentation_time_delta;
    return;
}

FMP4_API
void
fmp4_emsg_set_presentation_time(fmp4_emsg* emsg, uint64_t presentation_time) {
    emsg->presentation_time = presentation_time;
    return;
}

FMP4_API
void
fmp4_emsg_set_event_duration(fmp4_emsg* emsg, uint32_t event_duration) {
    emsg->event_duration = event_duration;
    return;
}

FMP4_API
void
fmp4_emsg_set_id(fmp4_emsg* emsg, uint32_t id) {
    emsg->id = id;
    return;
}

FMP4_API
fmp4_result
fmp4_emsg_set_scheme_id_uri(fmp4_emsg* emsg, const char* scheme_id_uri) {
    emsg->scheme_id_uri.len = 0;
    return fmp4_membuf_cat(&emsg->scheme_id_uri, scheme_id_uri, strlen(scheme_id_uri) + 1);
}

FMP4_API
fmp4_result
fmp4_emsg_set_value(fmp4_emsg* emsg, const char* value) {
    emsg->value.len = 0;
    return fmp4_membuf_cat(&emsg->value, value, strlen(value) + 1);
}

FMP4_API
fmp4_result
fmp4_emsg_set_message(fmp4_emsg* emsg, const uint8_t* message_data, uint32_t message_size) {
    emsg->message.len = 0;
    return fmp4_membuf_cat(&emsg->message, message_data, message_size);
}
