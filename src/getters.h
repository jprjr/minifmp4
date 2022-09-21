#ifndef GETTERS_H
#define GETTERS_H
#include "defines.h"
#include "structs.h"

/* you probably just want to get various pieces of data directly but,
 * if you've embedded this in some kind of FFI environment where
 * using data structures is tricky, there's also get functions */

FMP4_API
fmp4_result
fmp4_mux_get_brand_major(const fmp4_mux* mux, char brand[4], uint32_t* ver);

FMP4_API
uint32_t
fmp4_mux_get_brand_minor_version(const fmp4_mux* mux);

FMP4_API
const char*
fmp4_mux_get_brands(const fmp4_mux* mux, size_t* len);

FMP4_API
fmp4_stream_type
fmp4_track_get_stream_type(const fmp4_track* track);

FMP4_API
fmp4_codec
fmp4_track_get_codec(const fmp4_track* track);

FMP4_API
fmp4_object_type
fmp4_track_get_object_type(const fmp4_track* track);

FMP4_API
uint64_t
fmp4_track_get_base_media_decode_time(const fmp4_track* track);

FMP4_API
uint32_t
fmp4_track_get_time_scale(const fmp4_track* track);
#define fmp4_track_get_audio_sample_rate(track) fmp4_track_get_time_scale(track)

FMP4_API
const char*
fmp4_track_get_language(const fmp4_track* track);

FMP4_API
uint16_t
fmp4_track_get_audio_channels(const fmp4_track* track);

FMP4_API
uint32_t
fmp4_track_get_encoder_delay(const fmp4_track* track);

FMP4_API
int16_t
fmp4_track_get_roll_distance(const fmp4_track* track);

FMP4_API
fmp4_result
fmp4_track_get_default_sample_info(const fmp4_track* track, fmp4_sample_info* info);

FMP4_API
const void*
fmp4_track_get_dsi(const fmp4_track* track, size_t* len);

FMP4_API
fmp4_result
fmp4_track_get_first_sample_flags(const fmp4_track* track, fmp4_sample_flags* flags);

FMP4_API
uint8_t
fmp4_track_get_trun_sample_flags_set(const fmp4_track* track);

FMP4_API
fmp4_result
fmp4_track_get_trun_sample_flags(const fmp4_track* track, fmp4_sample_flags* flags);

FMP4_API
uint8_t
fmp4_track_get_trun_sample_duration_set(const fmp4_track* track);

FMP4_API
uint32_t
fmp4_track_get_trun_sample_duration(const fmp4_track* track);

FMP4_API
uint8_t
fmp4_track_get_trun_sample_size_set(const fmp4_track* track);

FMP4_API
uint32_t
fmp4_track_get_trun_sample_size(const fmp4_track* track);

FMP4_API
uint64_t
fmp4_track_get_trun_sample_count(const fmp4_track* track);

FMP4_API
fmp4_loudness_type
fmp4_loudness_get_type(const fmp4_loudness* loudness);

FMP4_API
uint8_t
fmp4_loudness_get_downmix_id(const fmp4_loudness* loudness);

FMP4_API
uint8_t
fmp4_loudness_get_drc_id(const fmp4_loudness* loudness);

FMP4_API
double
fmp4_loudness_get_sample_peak(const fmp4_loudness* loudness);

FMP4_API
double
fmp4_loudness_get_rue_peak(const fmp4_loudness* loudness);

FMP4_API
uint8_t
fmp4_loudness_get_system(const fmp4_loudness* loudness);

FMP4_API
uint8_t
fmp4_loudness_get_reliability(const fmp4_loudness* loudness);

FMP4_API
uint8_t
fmp4_measurement_get_method(const fmp4_measurement* measurement);

FMP4_API
double
fmp4_measurement_get_value(const fmp4_measurement* measurement);

FMP4_API
uint8_t
fmp4_measurement_get_system(const fmp4_measurement* measurement);

FMP4_API
uint8_t
fmp4_measurement_get_reliability(const fmp4_measurement* measurement);

FMP4_API
uint8_t
fmp4_sample_flags_get_is_leading(const fmp4_sample_flags* flags);

FMP4_API
uint8_t
fmp4_sample_flags_get_depends_on(const fmp4_sample_flags* flags);

FMP4_API
uint8_t
fmp4_sample_flags_get_is_depended_on(const fmp4_sample_flags* flags);

FMP4_API
uint8_t
fmp4_sample_flags_get_has_redundancy(const fmp4_sample_flags* flags);

FMP4_API
uint8_t
fmp4_sample_flags_get_padding_value(const fmp4_sample_flags* flags);

FMP4_API
uint8_t
fmp4_sample_flags_get_is_non_sync(const fmp4_sample_flags* flags);

FMP4_API
uint16_t
fmp4_sample_flags_get_degradation_priority(const fmp4_sample_flags* flags);

FMP4_API
uint32_t
fmp4_sample_info_get_duration(const fmp4_sample_info* info);

FMP4_API
uint32_t
fmp4_sample_info_get_size(const fmp4_sample_info* info);

FMP4_API
fmp4_result
fmp4_sample_info_get_flags(const fmp4_sample_info* info, fmp4_sample_flags* flags);

FMP4_API
uint8_t
fmp4_emsg_get_version(const fmp4_emsg* emsg);

FMP4_API
uint32_t
fmp4_emsg_get_timescale(const fmp4_emsg* emsg);

FMP4_API
uint32_t
fmp4_emsg_get_presentation_time_delta(const fmp4_emsg* emsg);

FMP4_API
uint64_t
fmp4_emsg_get_presentation_time(const fmp4_emsg* emsg);

FMP4_API
uint32_t
fmp4_emsg_get_event_duration(const fmp4_emsg* emsg);

FMP4_API
uint32_t
fmp4_emsg_get_id(const fmp4_emsg* emsg);

FMP4_API
const char*
fmp4_emsg_get_scheme_id_uri(const fmp4_emsg* emsg);

FMP4_API
const char*
fmp4_emsg_get_value(const fmp4_emsg* emsg);

FMP4_API
const uint8_t*
fmp4_emsg_get_message(const fmp4_emsg* emsg, uint32_t* message_size);

FMP4_API
uint32_t
fmp4_emsg_get_message_size(const fmp4_emsg* emsg);

#endif
