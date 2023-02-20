#ifndef STRUCTS_H
#define STRUCTS_H
#include "int.h"

/* callback used in the file-writing functions */
typedef size_t (*fmp4_write_cb)(const void* src, size_t len, void* userdata);

/* callbacks used for custom memory allocators */
typedef void* (*fmp4_malloc_cb)(size_t len, void* userdata);
typedef void* (*fmp4_realloc_cb)(void* ptr, size_t len, void* userdata);
typedef void (*fmp4_free_cb)(void* ptr, void* userdata);

struct fmp4_allocator {
    void* userdata;
    fmp4_malloc_cb malloc;
    fmp4_realloc_cb realloc;
    fmp4_free_cb free;
};

typedef struct fmp4_allocator fmp4_allocator;

/* stuff for managing memory buffers, kind of like a vector */
struct fmp4_membuf {
    uint8_t* x;
    size_t len;
    size_t a;
    const fmp4_allocator* allocator;
};

typedef struct fmp4_membuf fmp4_membuf;

enum fmp4_stream_type {
    FMP4_STREAM_TYPE_FORBIDDEN = 0x00,
    FMP4_STREAM_TYPE_AUDIO = 0x05,
};

typedef enum fmp4_stream_type fmp4_stream_type;

enum fmp4_object_type {
    FMP4_OBJECT_TYPE_FORBIDDEN = 0x00,
    FMP4_OBJECT_TYPE_AAC = 0x40, /* covers AAC, HE-AAC, xHE-AAC, etc */
    FMP4_OBJECT_TYPE_MP3 = 0x6B,
};

typedef enum fmp4_object_type fmp4_object_type;

enum fmp4_codec {
    FMP4_CODEC_UNDEFINED = 0x00000000,
    FMP4_CODEC_MP4A = 0x6d703461, /* covers AAC, HE-AAC, xHE-AAC, MP3-in-MP4, etc */
    FMP4_CODEC_ALAC = 0x616c6163,
    FMP4_CODEC_FLAC = 0x664c6143,
    FMP4_CODEC_OPUS = 0x4f707573,
    FMP4_CODEC_AC3  = 0x61632d33,
    FMP4_CODEC_EAC3 = 0x65632d33,
};

typedef enum fmp4_codec fmp4_codec;

struct fmp4_measurement {
    uint8_t method; /* methodDefinition */
    uint8_t value; /* methodValue */
    uint8_t system; /*measurementSystem */
    uint8_t reliability;
    const fmp4_allocator* allocator;
};

typedef struct fmp4_measurement fmp4_measurement;

enum fmp4_loudness_type {
    FMP4_LOUDNESS_UNDEF = 0,
    FMP4_LOUDNESS_TRACK = 1,
    FMP4_LOUDNESS_ALBUM = 2,
};

typedef enum fmp4_loudness_type fmp4_loudness_type;

struct fmp4_loudness {
    fmp4_loudness_type type;
    uint8_t downmix_id; /* downmix_ID */
    uint8_t drc_id; /* DRC_set_ID */
    int16_t sample_peak; /*bsSamplePeakLevel */
    int16_t true_peak; /* bsTruePeakLevel */
    uint8_t system; /*measurement_system_for_TP */
    uint8_t reliability; /* reliaibility_for_TP */
    fmp4_membuf measurements;
    fmp4_membuf alloc_measurement;
    const fmp4_allocator* allocator;
};

typedef struct fmp4_loudness fmp4_loudness;

struct fmp4_sample_flags {
    uint8_t is_leading;
    uint8_t depends_on;
    uint8_t is_depended_on;
    uint8_t has_redundancy;
    uint8_t padding_value;
    uint8_t is_non_sync;
    uint16_t degradation_priority;
    const fmp4_allocator* allocator;
};

typedef struct fmp4_sample_flags fmp4_sample_flags;

struct fmp4_sample_info {
    uint32_t duration;
    uint32_t size;
    fmp4_sample_flags flags;
    const fmp4_allocator* allocator;
};

typedef struct fmp4_sample_info fmp4_sample_info;

struct fmp4_emsg {
    uint8_t version;
    uint32_t timescale;
    uint32_t presentation_time_delta;
    uint64_t presentation_time;
    uint32_t event_duration;
    uint32_t id;
    fmp4_membuf scheme_id_uri;
    fmp4_membuf value;
    fmp4_membuf message;
    const fmp4_allocator* allocator;
};

typedef struct fmp4_emsg fmp4_emsg;

struct fmp4_track {
    fmp4_stream_type stream_type;
    fmp4_codec codec;
    fmp4_object_type object_type;
    uint64_t base_media_decode_time;
    uint32_t time_scale;
    uint8_t language[4];
    uint32_t encoder_delay;
    int16_t roll_distance;

    union {
        struct {
            uint16_t channels;
        } audio;
    } info;

    fmp4_sample_info default_sample_info;
    fmp4_membuf loudness;
    fmp4_membuf dsi;

    fmp4_membuf alloc_loudness;

    uint32_t first_sample_flags;

    uint8_t  trun_sample_flags_set;
    uint32_t trun_sample_flags;

    uint8_t  trun_sample_duration_set;
    uint32_t trun_sample_duration;

    uint8_t  trun_sample_size_set;
    uint32_t trun_sample_size;

    uint64_t trun_sample_count;
    size_t data_offset_pos;

    fmp4_membuf sample_info;
    fmp4_membuf mdat;
    const fmp4_allocator* allocator;
};

typedef struct fmp4_track fmp4_track;

struct fmp4_mux {
    char brand_major[4];
    uint32_t brand_minor_version;

    fmp4_membuf buffer;
    fmp4_membuf stack;
    fmp4_membuf brands;
    fmp4_membuf tracks;
    fmp4_membuf emsgs;

    fmp4_membuf alloc_track;
    fmp4_membuf alloc_emsg;

    size_t moof_offset;
    uint32_t fragments;
    const fmp4_allocator* allocator;
};

typedef struct fmp4_mux fmp4_mux;
#endif
