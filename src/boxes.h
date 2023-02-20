#ifndef BOXES_H
#define BOXES_H
#include "defines.h"
#include "structs.h"
#define BOX_ID(a, b, c, d) (((uint32_t)(a) << 24) | ((b) << 16) | ((c) << 8) | (d))

enum fmp4_box_id {
    BOX_ftyp = BOX_ID('f','t','y','p'),
    BOX_styp = BOX_ID('s','t','y','p'),
    BOX_moov = BOX_ID('m','o','o','v'),
    BOX_mvhd = BOX_ID('m','v','h','d'),
    BOX_trak = BOX_ID('t','r','a','k'),
    BOX_tkhd = BOX_ID('t','k','h','d'),
    BOX_mdia = BOX_ID('m','d','i','a'),
    BOX_mdhd = BOX_ID('m','d','h','d'),
    BOX_hdlr = BOX_ID('h','d','l','r'),
    BOX_minf = BOX_ID('m','i','n','f'),
    BOX_smhd = BOX_ID('s','m','h','d'),
    BOX_dinf = BOX_ID('d','i','n','f'),
    BOX_dref = BOX_ID('d','r','e','f'),
    BOX_url  = BOX_ID('u','r','l',' '),
    BOX_stbl = BOX_ID('s','t','b','l'),
    BOX_stsd = BOX_ID('s','t','s','d'),
    BOX_mp4a = BOX_ID('m','p','4','a'),
    BOX_alac = BOX_ID('a','l','a','c'),
    BOX_fLaC = BOX_ID('f','L','a','C'),
    BOX_esds = BOX_ID('e','s','d','s'),
    BOX_dfLa = BOX_ID('d','f','L','a'),
    BOX_stts = BOX_ID('s','t','t','s'),
    BOX_stsc = BOX_ID('s','t','s','c'),
    BOX_stsz = BOX_ID('s','t','s','z'),
    BOX_stco = BOX_ID('s','t','c','o'),
    BOX_mvex = BOX_ID('m','v','e','x'),
    BOX_mehd = BOX_ID('m','e','h','d'),
    BOX_trex = BOX_ID('t','r','e','x'),
    BOX_moof = BOX_ID('m','o','o','f'),
    BOX_mdat = BOX_ID('m','d','a','t'),
    BOX_mfhd = BOX_ID('m','f','h','d'),
    BOX_traf = BOX_ID('t','r','a','f'),
    BOX_tfhd = BOX_ID('t','f','h','d'),
    BOX_tfdt = BOX_ID('t','f','d','t'),
    BOX_trun = BOX_ID('t','r','u','n'),
    BOX_udta = BOX_ID('u','d','t','a'),
    BOX_ludt = BOX_ID('l','u','d','t'),
    BOX_tlou = BOX_ID('t','l','o','u'),
    BOX_alou = BOX_ID('a','l','o','u'),
    BOX_emsg = BOX_ID('e','m','s','g'),
    BOX_edts = BOX_ID('e','d','t','s'),
    BOX_elst = BOX_ID('e','l','s','t'),
    BOX_sgpd = BOX_ID('s','g','p','d'),
    BOX_sbgp = BOX_ID('s','b','g','p'),
    BOX_Opus = BOX_ID('O','p','u','s'),
    BOX_dOps = BOX_ID('d','O','p','s'),
    BOX_ac_3 = BOX_ID('a','c','-','3'),
    BOX_dac3 = BOX_ID('d','a','c','3'),
    BOX_ec_3 = BOX_ID('e','c','-','3'),
    BOX_dec3 = BOX_ID('d','e','c','3'),
};

typedef enum fmp4_box_id fmp4_box_id;

enum fmp4_hdlr_id {
    HDLR_soun = BOX_ID('s','o','u','n'),
};

typedef enum fmp4_hdlr_id fmp4_hdlr_id;

#undef BOX_ID

FMP4_PRIVATE
fmp4_result
fmp4_box_ftyp(fmp4_mux* mux);

FMP4_PRIVATE
fmp4_result
fmp4_box_styp(fmp4_mux* mux);

FMP4_PRIVATE
fmp4_result
fmp4_box_moov(fmp4_mux* mux);

FMP4_PRIVATE
fmp4_result
fmp4_box_trak(fmp4_mux* mux, const fmp4_track* track, uint32_t track_id);

FMP4_PRIVATE
fmp4_result
fmp4_box_trak_udta(fmp4_mux* mux, const fmp4_track* track);

FMP4_PRIVATE
fmp4_result
fmp4_box_loudness(fmp4_mux* mux, const fmp4_loudness* loudness);

FMP4_PRIVATE
fmp4_result
fmp4_box_emsg(fmp4_mux* mux, const fmp4_emsg* emsg);

FMP4_PRIVATE
fmp4_result
fmp4_box_moof(fmp4_mux* mux);

FMP4_PRIVATE
fmp4_result
fmp4_box_traf(fmp4_mux* mux, fmp4_track* track, uint32_t track_id);

FMP4_PRIVATE
fmp4_result
fmp4_box_mdat(fmp4_mux* mux);

FMP4_PRIVATE
fmp4_result
fmp4_box_begin(fmp4_mux* mux, fmp4_membuf* buffer, fmp4_box_id id);

FMP4_PRIVATE
fmp4_result
fmp4_box_end(fmp4_mux* mux, fmp4_membuf* buffer, fmp4_box_id id);

FMP4_PRIVATE
fmp4_result
fmp4_es_tag_begin(fmp4_mux* mux, fmp4_membuf* buffer, uint32_t tag_id);

FMP4_PRIVATE
fmp4_result
fmp4_es_tag_end(fmp4_mux* mux, fmp4_membuf* buffer, uint32_t tag_id);

#endif
