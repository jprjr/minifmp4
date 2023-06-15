#ifndef SETTERS_H
#define SETTERS_H
#include "defines.h"
#include "structs.h"


/* you probably just want to set various pieces of data directly but,
 * if you've embedded this in some kind of FFI environment where
 * using data structures is tricky, there's also set functions */

FMP4_API
void
fmp4_track_set_stream_type(fmp4_track* track, fmp4_stream_type stream_type);

FMP4_API
void
fmp4_track_set_codec(fmp4_track* track, fmp4_codec codec);

FMP4_API
void
fmp4_track_set_object_type(fmp4_track* track, fmp4_object_type object_type);

FMP4_API
void
fmp4_track_set_base_media_decode_time(fmp4_track* track, uint64_t base_media_decode_time);

FMP4_API
void
fmp4_track_set_time_scale(fmp4_track* track, uint32_t time_scale);

FMP4_API
void
fmp4_track_set_language(fmp4_track* track, const char* language);

#define fmp4_track_set_audio_sample_rate(t, r) fmp4_track_set_time_scale(t, r)

FMP4_API
void
fmp4_track_set_encoder_delay(fmp4_track* track, uint32_t delay);

FMP4_API
void
fmp4_track_set_roll_distance(fmp4_track* track, int16_t distance);

FMP4_API
void
fmp4_track_set_roll_type(fmp4_track* track, fmp4_roll_type roll_type);

FMP4_API
void
fmp4_track_set_audio_channels(fmp4_track* track, uint16_t channels);

FMP4_API
fmp4_result
fmp4_track_set_dsi(fmp4_track* track, const void* dsi, size_t len);

FMP4_API
void
fmp4_track_set_default_sample_info(fmp4_track *track, const fmp4_sample_info* info);

FMP4_API
void
fmp4_track_set_first_sample_flags(fmp4_track* track, const fmp4_sample_flags* flags);

FMP4_API
void
fmp4_track_set_trun_sample_flags(fmp4_track* track, const fmp4_sample_flags* flags);

FMP4_API
void
fmp4_track_set_trun_sample_duration(fmp4_track* track, uint32_t duration);

FMP4_API
void
fmp4_track_set_trun_sample_size(fmp4_track* track, uint32_t size);

FMP4_API
void
fmp4_track_set_trun_sample_count(fmp4_track* track, uint64_t count);

FMP4_API
void
fmp4_loudness_set_downmix_id(fmp4_loudness* loudness, uint8_t id);

FMP4_API
void
fmp4_loudness_set_drc_id(fmp4_loudness* loudness, uint8_t id);

FMP4_API
void
fmp4_loudness_set_type(fmp4_loudness* loudness, fmp4_loudness_type type);

FMP4_API
fmp4_result
fmp4_loudness_set_sample_peak(fmp4_loudness* loudness, double peak);

FMP4_API
fmp4_result
fmp4_loudness_set_true_peak(fmp4_loudness* loudness, double peak);

FMP4_API
fmp4_result
fmp4_loudness_set_system(fmp4_loudness* loudness, uint8_t system);

FMP4_API
fmp4_result
fmp4_loudness_set_reliability(fmp4_loudness* loudness, uint8_t reliability);

FMP4_API
fmp4_result
fmp4_measurement_set_method(fmp4_measurement* measurement, uint8_t method);

FMP4_API
fmp4_result
fmp4_measurement_set_value(fmp4_measurement* measurement, double value);

FMP4_API
fmp4_result
fmp4_measurement_set_system(fmp4_measurement* measurement, uint8_t system);

FMP4_API
fmp4_result
fmp4_measurement_set_reliability(fmp4_measurement* measurement, uint8_t reliability);

FMP4_API
void
fmp4_sample_flags_set_is_leading(fmp4_sample_flags* flags, uint8_t is_leading);

FMP4_API
void
fmp4_sample_flags_set_depends_on(fmp4_sample_flags* flags, uint8_t depends_on);

FMP4_API
void
fmp4_sample_flags_set_is_depended_on(fmp4_sample_flags* flags, uint8_t is_depended_on);

FMP4_API
void
fmp4_sample_flags_set_has_redundancy(fmp4_sample_flags* flags, uint8_t has_redundancy);

FMP4_API
void
fmp4_sample_flags_set_padding_value(fmp4_sample_flags* flags, uint8_t padding_value);

FMP4_API
void
fmp4_sample_flags_set_is_non_sync(fmp4_sample_flags* flags, uint8_t is_non_sync);

FMP4_API
void
fmp4_sample_flags_set_degradation_priority(fmp4_sample_flags* flags, uint16_t priority);

FMP4_API
void
fmp4_sample_info_set_duration(fmp4_sample_info* info, uint32_t duration);

FMP4_API
void
fmp4_sample_info_set_size(fmp4_sample_info* info, uint32_t size);

FMP4_API
void
fmp4_sample_info_set_flags(fmp4_sample_info* info, const fmp4_sample_flags* flags);

FMP4_API
fmp4_result
fmp4_emsg_set_version(fmp4_emsg* emsg, uint8_t version);

FMP4_API
void
fmp4_emsg_set_timescale(fmp4_emsg* emsg, uint32_t timescale);

FMP4_API
void
fmp4_emsg_set_presentation_time_delta(fmp4_emsg* emsg, uint32_t presentation_time_delta);

FMP4_API
void
fmp4_emsg_set_presentation_time(fmp4_emsg* emsg, uint64_t presentation_time);

FMP4_API
void
fmp4_emsg_set_event_duration(fmp4_emsg* emsg, uint32_t event_duration);

FMP4_API
void
fmp4_emsg_set_id(fmp4_emsg* emsg, uint32_t id);

FMP4_API
fmp4_result
fmp4_emsg_set_scheme_id_uri(fmp4_emsg* emsg, const char* scheme_id_uri);

FMP4_API
fmp4_result
fmp4_emsg_set_value(fmp4_emsg* emsg, const char* value);

FMP4_API
fmp4_result
fmp4_emsg_set_message(fmp4_emsg* emsg, const uint8_t* message_data, uint32_t message_size);

#endif
