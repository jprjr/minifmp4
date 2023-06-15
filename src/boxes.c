#include "boxes.h"
#include "membuf.h"
#include "pack.h"
#include "flags.h"

#define BOX_ID(a, b, c, d) (((uint32_t)(a) << 24) | ((b) << 16) | ((c) << 8) | (d))

FMP4_PRIVATE
fmp4_result
fmp4_box_begin(fmp4_mux* mux, fmp4_membuf* buffer, fmp4_box_id box_id) {
    fmp4_result res;
    if( (res = fmp4_membuf_readyplus(buffer, sizeof(fmp4_box_id) + sizeof(buffer->len))) != FMP4_OK) return res;

    if( (res = fmp4_membuf_push(&mux->stack, buffer->len)) != FMP4_OK) return res;
    if( (res = fmp4_membuf_push(&mux->stack, box_id)) != FMP4_OK) return res;

    buffer->len += 4; /* leave space to write the size later */

    fmp4_pack_uint32be(&buffer->x[buffer->len], box_id);
    buffer->len += 4;

    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_box_end(fmp4_mux* mux, fmp4_membuf* buffer, fmp4_box_id box_id) {
    fmp4_result res;
    uint32_t id;
    size_t pos;

    if( (res = fmp4_membuf_pop(&mux->stack, id)) != FMP4_OK) return res;
    if(id != box_id) return FMP4_STACKERROR;

    if( (res = fmp4_membuf_pop(&mux->stack, pos)) != FMP4_OK) return res;
    if(buffer->len - pos > 0xFFFFFFFF) return FMP4_BOXTOOBIG;

    fmp4_pack_uint32be(&buffer->x[pos], (uint32_t) buffer->len - pos);
    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_es_tag_begin(fmp4_mux* mux, fmp4_membuf* buffer, uint32_t tag_id) {
    fmp4_result res;
    if( (res = fmp4_membuf_readyplus(buffer, sizeof(uint32_t) + sizeof(size_t))) != FMP4_OK) return res;

    buffer->x[buffer->len++] = (uint8_t)(tag_id & 0xFF);

    /* record our current position and tag */
    if( (res = fmp4_membuf_push(&mux->stack, buffer->len)) != FMP4_OK) return res;
    if( (res = fmp4_membuf_push(&mux->stack, tag_id)) != FMP4_OK) return res;

    buffer->len += 4; /* leave space to write the size later */

    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_es_tag_end(fmp4_mux* mux, fmp4_membuf* buffer, uint32_t tag_id) {
    fmp4_result res;
    uint32_t id;
    size_t pos;
    size_t len;

    if( (res = fmp4_membuf_pop(&mux->stack, id)) != FMP4_OK) return res;
    if(id != tag_id) return FMP4_STACKERROR;

    if( (res = fmp4_membuf_pop(&mux->stack, pos)) != FMP4_OK) return res;
    len = buffer->len - pos;
    if(len <= 4) return FMP4_STACKERROR;
    len -= 4; /* length of the size field is not included */

    if(len > 0x0FFFFFFF) return FMP4_ESTAGTOOBIG;

    buffer->x[pos++] = 0x80 | ((len >> 21) & 0x7F);
    buffer->x[pos++] = 0x80 | ((len >> 14) & 0x7F);
    buffer->x[pos++] = 0x80 | ((len >>  7) & 0x7F);
    buffer->x[pos++] = 0x00 | ((len >>  0) & 0x7F);

    return FMP4_OK;
}

#define WRITE_DATA(ptr,len) \
if((res = fmp4_membuf_cat(&mux->buffer,ptr,len)) != FMP4_OK) return res

#define WRITE_UINT64(val) \
if((res = fmp4_membuf_readyplus(&mux->buffer,8)) != FMP4_OK) return res;\
fmp4_pack_uint64be(&mux->buffer.x[mux->buffer.len], val); \
mux->buffer.len += 8

#define WRITE_UINT32(val) \
if((res = fmp4_membuf_readyplus(&mux->buffer,4)) != FMP4_OK) return res;\
fmp4_pack_uint32be(&mux->buffer.x[mux->buffer.len], val); \
mux->buffer.len += 4

#define WRITE_UINT24(val) \
if((res = fmp4_membuf_readyplus(&mux->buffer,3)) != FMP4_OK) return res;\
fmp4_pack_uint24be(&mux->buffer.x[mux->buffer.len], val); \
mux->buffer.len += 3

#define WRITE_UINT16(val) \
if((res = fmp4_membuf_readyplus(&mux->buffer,2)) != FMP4_OK) return res;\
fmp4_pack_uint16be(&mux->buffer.x[mux->buffer.len], val); \
mux->buffer.len += 2

#define WRITE_INT16(val) WRITE_UINT16(((uint16_t)val))

#define WRITE_UINT8(val) \
if((res = fmp4_membuf_readyplus(&mux->buffer,1)) != FMP4_OK) return res;\
mux->buffer.x[mux->buffer.len++] = (uint8_t)val

#define BOX_BEGIN(typ) \
if((res = fmp4_box_begin(mux,&(mux->buffer),typ)) != FMP4_OK) return res

#define BOX_BEGIN_FULL(typ, version, flags) \
if((res = fmp4_box_begin(mux,&(mux->buffer),typ)) != FMP4_OK) return res; \
WRITE_UINT32( (((uint32_t)version) << 24) | flags)

#define BOX_END(typ) \
if((res = fmp4_box_end(mux,&(mux->buffer),typ)) != FMP4_OK) return res

#define ES_TAG_BEGIN(typ) \
if((res = fmp4_es_tag_begin(mux,&(mux->buffer),typ)) != FMP4_OK) return res

#define ES_TAG_END(typ) \
if((res = fmp4_es_tag_end(mux,&(mux->buffer),typ)) != FMP4_OK) return res

FMP4_PRIVATE
fmp4_result
fmp4_box_ftyp(fmp4_mux* mux) {
    fmp4_result res;

    BOX_BEGIN(BOX_ftyp);
    {
        WRITE_DATA(mux->brand_major, 4); /* major brand */
        WRITE_UINT32(mux->brand_minor_version); /* major brand version */
        WRITE_DATA(mux->brand_major, 4); /* major brand, first in the list of compatible brands */
        if(mux->brands.len > 0) WRITE_DATA(mux->brands.x, mux->brands.len); /* compatible brands */
    }
    BOX_END(BOX_ftyp);

    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_box_styp(fmp4_mux* mux) {
    fmp4_result res;

    BOX_BEGIN(BOX_styp);
    {
        WRITE_DATA(mux->brand_major, 4); /* major brand */
        WRITE_UINT32(mux->brand_minor_version); /* major brand version */
        WRITE_DATA(mux->brand_major, 4); /* major brand, first in the list of compatible brands */
        if(mux->brands.len > 0) WRITE_DATA(mux->brands.x, mux->brands.len); /* compatible brands */
    }
    BOX_END(BOX_styp);

    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_box_moov(fmp4_mux* mux) {
    fmp4_result res;
    size_t i;
    size_t len;
    fmp4_track **tracks;

    tracks = (fmp4_track**)mux->tracks.x;
    i = 0;
    len = mux->tracks.len / sizeof(fmp4_track*);

    BOX_BEGIN(BOX_moov);
    {
        BOX_BEGIN_FULL(BOX_mvhd, 0, 0);
        {
            WRITE_UINT32(0); /* creation time */
            WRITE_UINT32(0); /* modification time */
            WRITE_UINT32(1000); /* timescale */
            WRITE_UINT32(0); /* duration */
            WRITE_UINT32(0x00010000); /* rate */
            WRITE_UINT16(0x0100); /* volume */
            WRITE_UINT16(0); /* reserved */
            WRITE_UINT32(0); /* reserved */
            WRITE_UINT32(0); /* reserved */

            /* matrix */
            WRITE_UINT32(0x00010000); WRITE_UINT32(0); WRITE_UINT32(0);
            WRITE_UINT32(0); WRITE_UINT32(0x00010000); WRITE_UINT32(0);
            WRITE_UINT32(0); WRITE_UINT32(0); WRITE_UINT32(0x40000000);

            /* pre_defined[6] */
            WRITE_UINT32(0); WRITE_UINT32(0); WRITE_UINT32(0);
            WRITE_UINT32(0); WRITE_UINT32(0); WRITE_UINT32(0);

            /* next track id */
            WRITE_UINT32(len+1);
        }
        BOX_END(BOX_mvhd);

        for(i=0;i<len;i++) {
            if( (res = fmp4_box_trak(mux,tracks[i],i+1)) != FMP4_OK) return res;
        }

        BOX_BEGIN(BOX_mvex);
        {
            BOX_BEGIN_FULL(BOX_mehd, 0, 0);
            {
                WRITE_UINT32(0); /* duration */
            }
            BOX_END(BOX_mehd);

            for(i=0;i<len;i++) {
            BOX_BEGIN_FULL(BOX_trex, 0, 0);
            {
                WRITE_UINT32(i+1);
                WRITE_UINT32(1); /* default sample description index */
                WRITE_UINT32(tracks[i]->default_sample_info.duration); /* default sample duration */
                WRITE_UINT32(tracks[i]->default_sample_info.size); /* default sample size */
                WRITE_UINT32(fmp4_encode_sample_flags(&tracks[i]->default_sample_info.flags));
            }
            BOX_END(BOX_trex);
            }
        }
        BOX_END(BOX_mvex);
    }
    BOX_END(BOX_moov);

    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_box_trak(fmp4_mux* mux, const fmp4_track* track, uint32_t id) {
    fmp4_result res;

    BOX_BEGIN(BOX_trak);
    {
        BOX_BEGIN_FULL(BOX_tkhd, 0, 0x07);
        {
            WRITE_UINT32(0); /* creation time */
            WRITE_UINT32(0); /* modification time */
            WRITE_UINT32(id);
            WRITE_UINT32(0); /* reserved */
            WRITE_UINT32(0); /* duration */

            /* reserved[2] */
            WRITE_UINT32(0); WRITE_UINT32(0);

            WRITE_UINT16(0); /* layer */
            WRITE_UINT16(0); /* alternate group */
            WRITE_UINT16(0x0100); /* volume */
            WRITE_UINT16(0); /* reserved */

            /* matrix */
            WRITE_UINT32(0x00010000); WRITE_UINT32(0); WRITE_UINT32(0);
            WRITE_UINT32(0); WRITE_UINT32(0x00010000); WRITE_UINT32(0);
            WRITE_UINT32(0); WRITE_UINT32(0); WRITE_UINT32(0x40000000);

            WRITE_UINT32(0); /* width */
            WRITE_UINT32(0); /* height */
        }
        BOX_END(BOX_tkhd);

        if(track->encoder_delay > 0) {
            BOX_BEGIN(BOX_edts);
            {
                BOX_BEGIN_FULL(BOX_elst, 0, 0);
                {
                    WRITE_UINT32(1); /* entry count */
                    WRITE_UINT32(0); /* segment duration */
                    WRITE_UINT32(track->encoder_delay); /* media_time */
                    WRITE_UINT16(1); /* media rate integer */
                    WRITE_UINT16(0); /* media rate fraction */
                }
                BOX_END(BOX_elst);
            }
            BOX_END(BOX_edts);
        }

        BOX_BEGIN(BOX_mdia);
        {
            BOX_BEGIN_FULL(BOX_mdhd, 0, 0);
            {
                WRITE_UINT32(0);
                WRITE_UINT32(0);
                WRITE_UINT32(track->time_scale);
                WRITE_UINT32(0);
                {
                uint16_t lang_code = ((track->language[0] & 31) << 10) | ((track->language[1] & 31) << 5) | (track->language[2] & 31);
                WRITE_UINT16(lang_code);
                }
                WRITE_UINT16(0);
            }
            BOX_END(BOX_mdhd);

            BOX_BEGIN_FULL(BOX_hdlr, 0, 0);
            {
                WRITE_UINT32(0); /* predefined */
                WRITE_UINT32(HDLR_soun); /* handler type */

                /* reserved[3] */
                WRITE_UINT32(0); WRITE_UINT32(0); WRITE_UINT32(0);
                WRITE_DATA("SoundHandler", 13);
            }
            BOX_END(BOX_hdlr);

            BOX_BEGIN(BOX_minf);
            {
                BOX_BEGIN_FULL(BOX_smhd, 0, 0);
                {
                    WRITE_UINT16(0); /* balance */
                    WRITE_UINT16(0); /* reserved */
                }
                BOX_END(BOX_smhd);

                BOX_BEGIN(BOX_dinf);
                {
                    BOX_BEGIN_FULL(BOX_dref, 0, 0);
                    {
                        WRITE_UINT32(1); /* entry count */
                        BOX_BEGIN_FULL(BOX_url, 0, 0x01);
                        BOX_END(BOX_url);
                    }
                    BOX_END(BOX_dref);
                }
                BOX_END(BOX_dinf);

                BOX_BEGIN(BOX_stbl);
                {
                    BOX_BEGIN_FULL(BOX_stsd, 0, 0);
                    {
                        WRITE_UINT32(1); /* entry count */
                        BOX_BEGIN((fmp4_box_id)track->codec);
                        {
                            /* sample entry */
                            WRITE_UINT32(0); WRITE_UINT16(0);
                            WRITE_UINT16(1); /* data reference index */

                            if(track->stream_type == FMP4_STREAM_TYPE_AUDIO) {
                                /* audio sample entry */
                                WRITE_UINT32(0); WRITE_UINT32(0); /* reserved */
                                WRITE_UINT16(track->info.audio.channels);
                                WRITE_UINT16(16);
                                WRITE_UINT32(0);
                                if(track->time_scale < 0x10000) {
                                    WRITE_UINT32((track->time_scale << 16));
                                } else {
                                    WRITE_UINT32(0);
                                }

                                if(track->codec == FMP4_CODEC_MP4A) {
                                    BOX_BEGIN_FULL(BOX_esds, 0, 0);
                                    {
                                        ES_TAG_BEGIN(0x03);
                                        {
                                            WRITE_UINT16(0); /* ES_ID */
                                            WRITE_UINT8(0); /* various ES flags */

                                            ES_TAG_BEGIN(0x04);
                                            {
                                                WRITE_UINT8(track->object_type); /* audio object type */
                                                WRITE_UINT8(track->stream_type << 2); /* stream type audiostream */
                                                WRITE_UINT24(track->info.audio.channels * 6144/8);
                                                WRITE_UINT32(0); /* maxbitrate */
                                                WRITE_UINT32(0); /* average bitrate bps */

                                                if(track->dsi.len > 0) {
                                                ES_TAG_BEGIN(0x05);
                                                {
                                                    WRITE_DATA(track->dsi.x, track->dsi.len);
                                                }
                                                ES_TAG_END(0x05);
                                                }
                                            }
                                            ES_TAG_END(0x04);

                                            ES_TAG_BEGIN(0x06);
                                            {
                                                WRITE_UINT8(2); /* predefined value, required */
                                            }
                                            ES_TAG_END(0x06);
                                        }
                                        ES_TAG_END(0x03);
                                    }
                                    BOX_END(BOX_esds);
                                } else if(track->codec == FMP4_CODEC_ALAC) {
                                    BOX_BEGIN_FULL(BOX_alac, 0, 0);
                                    WRITE_DATA(track->dsi.x, track->dsi.len);
                                    BOX_END(BOX_alac);
                                } else if(track->codec == FMP4_CODEC_FLAC) {
                                    BOX_BEGIN_FULL(BOX_dfLa, 0, 0);
                                    WRITE_DATA(track->dsi.x, track->dsi.len);
                                    BOX_END(BOX_dfLa);
                                } else if(track->codec == FMP4_CODEC_OPUS) {
                                    BOX_BEGIN(BOX_dOps);
                                    WRITE_DATA(track->dsi.x, track->dsi.len);
                                    BOX_END(BOX_dOps);
                                } else if(track->codec == FMP4_CODEC_AC3) {
                                    BOX_BEGIN(BOX_dac3);
                                    WRITE_DATA(track->dsi.x, track->dsi.len);
                                    BOX_END(BOX_dac3);
                                } else if(track->codec == FMP4_CODEC_EAC3) {
                                    BOX_BEGIN(BOX_dec3);
                                    WRITE_DATA(track->dsi.x, track->dsi.len);
                                    BOX_END(BOX_dec3);
                                }
                            }
                        }
                        BOX_END((fmp4_box_id)track->codec);

                    }
                    BOX_END(BOX_stsd);

                    BOX_BEGIN_FULL(BOX_stts, 0, 0);
                    {
                        WRITE_UINT32(0); /* sample cont */
                    }
                    BOX_END(BOX_stts);

                    BOX_BEGIN_FULL(BOX_stsc, 0, 0);
                    {
                        WRITE_UINT32(0); /* sample count */
                    }
                    BOX_END(BOX_stsc);

                    BOX_BEGIN_FULL(BOX_stsz, 0, 0);
                    {
                        WRITE_UINT32(0); /* sample size */
                        WRITE_UINT32(0); /* sample count */
                    }
                    BOX_END(BOX_stsz);

                    BOX_BEGIN_FULL(BOX_stco, 0, 0);
                    {
                        WRITE_UINT32(0); /* sample count */
                    }
                    BOX_END(BOX_stco);

                    if(track->roll_distance != 0) {
                        BOX_BEGIN_FULL(BOX_sgpd, 1, 0);
                        {
                            if(track->roll_type == FMP4_ROLL_TYPE_ROLL) {
                                WRITE_UINT32( BOX_ID('r','o','l','l') ); /* grouping type roll */
                            } else {
                                WRITE_UINT32( BOX_ID('p','r','o','l') ); /* grouping type prol */
                            }
                            WRITE_UINT32(2); /* default length */
                            WRITE_UINT32(1); /* entry count */
                            WRITE_INT16(track->roll_distance); /* roll distance */

                        }
                        BOX_END(BOX_sgpd);
                    }
                }
                BOX_END(BOX_stbl);
            }
            BOX_END(BOX_minf);
        }
        BOX_END(BOX_mdia);

        if((res = fmp4_box_trak_udta(mux,track)) != FMP4_OK) return res;
    }
    BOX_END(BOX_trak);
    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_box_trak_udta(fmp4_mux* mux, const fmp4_track* track) {
    fmp4_result res;

    size_t i;
    size_t len;

    fmp4_loudness** loudnesses;

    if(track->loudness.len == 0) return FMP4_OK; /* no userdata */

    loudnesses = (fmp4_loudness **)track->loudness.x;
    len = track->loudness.len / sizeof(fmp4_loudness*);

    BOX_BEGIN(BOX_udta);
    {
        BOX_BEGIN(BOX_ludt);
        {
            /* we're going to do 2 loops which is kinda lame,
             * first for the track loudnesses, then album */
            for(i=0;i<len;i++) {
              if(loudnesses[i]->type == FMP4_LOUDNESS_TRACK) {
                  if( (res = fmp4_box_loudness(mux,loudnesses[i])) != FMP4_OK) return res;
              }
            }
            for(i=0;i<len;i++) {
              if(loudnesses[i]->type == FMP4_LOUDNESS_ALBUM) {
                  if( (res = fmp4_box_loudness(mux,loudnesses[i])) != FMP4_OK) return res;
              }
            }
        }
        BOX_END(BOX_ludt);
    }
    BOX_END(BOX_udta);

    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_box_loudness(fmp4_mux* mux, const fmp4_loudness* loudness) {
    fmp4_result res;
    size_t i;
    size_t len;
    uint32_t boxtype;

    fmp4_measurement** measurements = (fmp4_measurement**)loudness->measurements.x;
    len = (loudness->measurements.len / sizeof(fmp4_measurement*)) & 0xFF;

    if(loudness->type == FMP4_LOUDNESS_TRACK) {
        boxtype = BOX_tlou;
    } else {
        boxtype = BOX_alou;
    }

    BOX_BEGIN_FULL(boxtype,0, 0);
    {
        /* reserved(3), downmix_ID(7), DRC_set_ID(6) */
        uint16_t rdD = 0 |
          (loudness->downmix_id << 6) |
          (loudness->drc_id & 0x3F);

        /* bs_sample_peak_level(12), bs_true_peak_level(12), */
        /* measurement_system_for_TP (4), reliability_for_TP(4) */
        uint32_t bbmr = 0 |
          ((uint32_t)loudness->sample_peak << (12 + 4 + 4) & 0xFFF00000) |
          ((uint32_t)loudness->true_peak   << (     4 + 4) & 0x000FFF00) |
          (loudness->system                << (         4) & 0x000000F0) |
          (loudness->reliability                           & 0x0000000F);

        WRITE_UINT16(rdD);
        WRITE_UINT32(bbmr);

        WRITE_UINT8((uint8_t)len);
        for(i=0;i<len;i++) {
            WRITE_UINT8(measurements[i]->method);
            WRITE_UINT8(measurements[i]->value);
            WRITE_UINT8((measurements[i]->system << 4) | measurements[i]->reliability);
        }
    }
    BOX_END(boxtype);
    return FMP4_OK;
}


FMP4_PRIVATE
fmp4_result
fmp4_box_emsg(fmp4_mux* mux, const fmp4_emsg* emsg) {
    fmp4_result res;

    BOX_BEGIN_FULL(BOX_emsg, emsg->version, 0);
    {
        if(emsg->version == 0) {
            WRITE_DATA(emsg->scheme_id_uri.x,emsg->scheme_id_uri.len);
            WRITE_DATA(emsg->value.x,emsg->value.len);
            WRITE_UINT32(emsg->timescale);
            WRITE_UINT32(emsg->presentation_time_delta);
            WRITE_UINT32(emsg->event_duration);
            WRITE_UINT32(emsg->id);
        } else if(emsg->version == 1) {
            WRITE_UINT32(emsg->timescale);
            WRITE_UINT64(emsg->presentation_time);
            WRITE_UINT32(emsg->event_duration);
            WRITE_UINT32(emsg->id);
            WRITE_DATA(emsg->scheme_id_uri.x,emsg->scheme_id_uri.len);
            WRITE_DATA(emsg->value.x,emsg->value.len);
        }
        WRITE_DATA(emsg->message.x,emsg->message.len);
    }
    BOX_END(BOX_emsg);

    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_box_moof(fmp4_mux* mux) {
    fmp4_result res;
    size_t i = 0;
    size_t len = mux->tracks.len / sizeof(fmp4_track*);
    fmp4_track** tracks = (fmp4_track**)mux->tracks.x;

    mux->moof_offset = mux->buffer.len;

    BOX_BEGIN(BOX_moof);
    {
        BOX_BEGIN_FULL(BOX_mfhd, 0, 0);
        {
            WRITE_UINT32(++mux->fragments);
        }
        BOX_END(BOX_mfhd);

        for(i=0;i<len;i++) {
            if( (res = fmp4_box_traf(mux, tracks[i], i+1)) != FMP4_OK) return res;
        }
    }
    BOX_END(BOX_moof);

    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_box_traf(fmp4_mux* mux, fmp4_track* track, uint32_t id) {
    fmp4_result res;

    BOX_BEGIN(BOX_traf);
    {
        uint32_t tfhd_flags = 0;
        uint32_t trun_flags = 0;
        uint32_t default_flags = fmp4_encode_sample_flags(&track->default_sample_info.flags);

        tfhd_flags |= 0x020000; /* default base is moof */
        trun_flags |= 0x01; /* data offset present */

        if(track->trun_sample_duration_set) {
            if(track->trun_sample_duration != track->default_sample_info.duration) {
                tfhd_flags |= 0x08; /* default duration present */
            }
        } else {
            /* we had different track durations so include all of them */
            trun_flags |= 0x100; /* sample duration present */
        }

        if(track->trun_sample_size_set) {
            if(track->trun_sample_size != track->default_sample_info.size) {
                tfhd_flags |= 0x10; /* default size present */
            }
        } else {
            /* we had different track sizes so include all of them */
            trun_flags |= 0x200; /* sample size present */
        }

        if(track->trun_sample_flags_set) {
            if(track->trun_sample_flags != default_flags) {
                tfhd_flags |= 0x20; /* default sample flags present */
                /* if our first sample flags match the rest of the trun we won't need
                 * them */
                if(track->first_sample_flags != track->trun_sample_flags) {
                    trun_flags |= 0x04; /* first-sample-flags present */
                }
            } else {
                /* we can inherit track sample flags, check if we need the first
                 * sample flags */
                if(track->first_sample_flags != default_flags) {
                    trun_flags |= 0x04; /* first-sample-flags present */
                }
            }
        } else {
            /* we had differing sample flags so just include all of them */
            trun_flags |= 0x400; /* sample flags present */
        }


        BOX_BEGIN_FULL(BOX_tfhd, 0, tfhd_flags);
        {
            WRITE_UINT32(id);
            if(tfhd_flags & 0x08) {
                WRITE_UINT32(track->trun_sample_duration);
            }
            if(tfhd_flags & 0x10) {
                WRITE_UINT32(track->trun_sample_size);
            }
            if(tfhd_flags & 0x20) {
                WRITE_UINT32(track->trun_sample_flags);
            }
        }
        BOX_END(BOX_tfhd);

        BOX_BEGIN_FULL(BOX_tfdt, 1, 0);
        {
            WRITE_UINT64(track->base_media_decode_time);
        }
        BOX_END(BOX_tfdt);
        track->base_media_decode_time += (uint64_t) track->trun_sample_count;

        BOX_BEGIN_FULL(BOX_trun, 0, trun_flags);
        {
            size_t i;
            size_t len = track->sample_info.len / sizeof(fmp4_sample_info);
            fmp4_sample_info* info = (fmp4_sample_info *)track->sample_info.x;

            WRITE_UINT32((uint32_t)len); /* sample count */
            track->data_offset_pos = mux->buffer.len; /* we'll use this later to write the mdat
                                                         data offset */
            WRITE_UINT32(0); /* filler for data offset */
            if(trun_flags & 0x04) { WRITE_UINT32(track->first_sample_flags); }
            for(i=0;i<len;i++) {
                if(trun_flags & 0x100) {
                    WRITE_UINT32(info[i].duration);
                }
                if(trun_flags & 0x200) {
                    WRITE_UINT32(info[i].size);
                }
                if(trun_flags & 0x400) {
                    WRITE_UINT32(fmp4_encode_sample_flags(&info[i].flags));
                }
            }
        }
        BOX_END(BOX_trun);

        if(track->roll_distance != 0) {
            BOX_BEGIN_FULL(BOX_sbgp, 0, 0);
            {
                if(track->roll_type == FMP4_ROLL_TYPE_ROLL) {
                    WRITE_UINT32( BOX_ID('r','o','l','l') ); /* grouping type roll */
                } else {
                    WRITE_UINT32( BOX_ID('p','r','o','l') ); /* grouping type prol */
                }
                WRITE_UINT32(1); /* entry count */
                WRITE_UINT32(track->sample_info.len / sizeof(fmp4_sample_info)); /* sample count */
                WRITE_UINT32(1); /* group description index */

            }
            BOX_END(BOX_sbgp);
        }
    }
    BOX_END(BOX_traf);
    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_box_mdat(fmp4_mux* mux) {
    fmp4_result res;
    size_t i = 0;
    size_t len = mux->tracks.len / sizeof(fmp4_track*);
    fmp4_track** tracks = (fmp4_track**)mux->tracks.x;

    BOX_BEGIN(BOX_mdat);
    {
        for(i=0;i<len;i++) {
            fmp4_pack_uint32be(&mux->buffer.x[tracks[i]->data_offset_pos], mux->buffer.len - mux->moof_offset);
            WRITE_DATA(tracks[i]->mdat.x, tracks[i]->mdat.len);
        }
    }
    BOX_END(BOX_mdat);

    return FMP4_OK;
}

#undef WRITE_DATA
#undef WRITE_UINT64
#undef WRITE_UINT32
#undef WRITE_UINT24
#undef WRITE_UINT16
#undef WRITE_UINT8
#undef BOX_BEGIN
#undef BOX_BEGIN_FULL
#undef BOX_END
#undef ES_TAG_BEGIN
#undef ES_TAG_END
#undef BOX_ID

