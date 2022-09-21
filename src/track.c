#include "track.h"
#include "membuf.h"
#include "flags.h"
#include "fmp4_string.h"
#include "init.h"
#include "close.h"
#include "loudness.h"

FMP4_API
fmp4_loudness*
fmp4_track_new_loudness(fmp4_track* track) {
    fmp4_loudness* loudness = fmp4_loudness_new(track->allocator);
    if(loudness == NULL) return loudness;

    if(fmp4_membuf_cat(&track->alloc_loudness,&loudness,sizeof(fmp4_loudness*)) != FMP4_OK) {
        fmp4_loudness_free(loudness);
        return NULL;
    }

    if(fmp4_track_add_loudness(track,loudness) != FMP4_OK) {
        fmp4_loudness_free(loudness);
        fmp4_membuf_uncat(&track->alloc_loudness, &loudness, sizeof(fmp4_loudness*));
        return NULL;
    }

    return loudness;
}

FMP4_API
fmp4_result
fmp4_track_add_sample(fmp4_track* track, const void* data, const fmp4_sample_info* info) {
    fmp4_result res;

    if((res = fmp4_membuf_cat(&track->sample_info,info,sizeof(fmp4_sample_info))) != FMP4_OK) return res;
    if((res = fmp4_membuf_cat(&track->mdat,data,info->size)) != FMP4_OK) return res;

    if(track->sample_info.len == sizeof(fmp4_sample_info)) {
        /* this is the first sample, always record the first sample's flags */
        track->first_sample_flags = fmp4_encode_sample_flags(&info->flags);
        track->trun_sample_flags_set = 0;

        /* set the duration flag, used to see if all samples use the same duration */
        track->trun_sample_duration_set = 1;
        track->trun_sample_duration = info->duration;

        /* set the size flag, used to see if all samples use the same duration */
        track->trun_sample_size_set = 1;
        track->trun_sample_size = info->size;

        /* lastly reset the sample count */
        track->trun_sample_count       = 0;
    } else {
        if(track->sample_info.len == (sizeof(fmp4_sample_info) * 2)) {
            /* second sample - set the sample_flags, similar to duration */
            track->trun_sample_flags_set = 1;
            track->trun_sample_flags = fmp4_encode_sample_flags(&info->flags);
        } else {
            /* third+ sample - where we check that all sample flags matched */
            if(track->trun_sample_flags_set == 1) {
                if(track->trun_sample_flags != fmp4_encode_sample_flags(&info->flags)) {
                    /* flags mis-match, we'll need to report all flags in the
                     * track fragment header */
                    track->trun_sample_flags_set = 0;
                }
            }
        }

        /* on all samples - check the duration */
        if(track->trun_sample_duration_set == 1) {
            if(track->trun_sample_duration != info->duration) {
                /* we had a mis-match occur, we'll need to
                 * report all durations in the track header */
                track->trun_sample_duration_set = 0;
            }
        }

        /* on all samples - check the size */
        if(track->trun_sample_size_set == 1) {
            if(track->trun_sample_size != info->size) {
                /* we had a mis-match occur, we'll need to
                 * report all sizes in the track header */
                track->trun_sample_size_set = 0;
            }
        }
    }

    track->trun_sample_count += (uint64_t)info->duration;
    return FMP4_OK;
}

FMP4_API
fmp4_result
fmp4_track_add_loudness(fmp4_track* track, const fmp4_loudness* loudness) {
    return fmp4_membuf_cat(&track->loudness, &loudness, sizeof(fmp4_loudness*));
}


/* validates the track has valid init-related data */
FMP4_API
fmp4_result
fmp4_track_validate_init(const fmp4_track* track) {
    fmp4_result res;
    size_t i;
    size_t len;
    fmp4_loudness** loudnesses;

    switch(track->stream_type) {
        case FMP4_STREAM_TYPE_FORBIDDEN: return FMP4_STREAMINVALID;
        case FMP4_STREAM_TYPE_AUDIO: {
            if(track->info.audio.channels == 0) return FMP4_CHANNELSINVALID;
            switch(track->codec) {
                case FMP4_CODEC_UNDEFINED: return FMP4_CODECINVALID;
                case FMP4_CODEC_MP4A:
                    if(track->object_type == FMP4_OBJECT_TYPE_FORBIDDEN) return FMP4_OBJECTINVALID;
                    /* fall-through */
                case FMP4_CODEC_ALAC: /* fall-through */
                case FMP4_CODEC_FLAC: /* fall-through */
                default: {
                    if(track->dsi.len == 0) return FMP4_MISSINGDSI;
                    break;
                }
            }
            break;
        }
        default: break;
    }
    if(track->time_scale == 0) return FMP4_TIMESCALEINVALID;

    loudnesses = (fmp4_loudness**)track->loudness.x;
    len = track->loudness.len / sizeof(fmp4_loudness*);
    for(i=0;i<len;i++) {
        if( (res = fmp4_loudness_validate(loudnesses[i])) != FMP4_OK) return res;
    }

    return FMP4_OK;
}

/* validates the track has valid media-related data */
FMP4_API
fmp4_result
fmp4_track_validate_segment(const fmp4_track* track) {
    if(track->sample_info.len == 0) return FMP4_NOSAMPLES;
    return FMP4_OK;
}
