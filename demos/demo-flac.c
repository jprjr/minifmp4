/* a demo program that reads in a FLAC file and
 * re-muxes into mp4, without re-encoding.
 *
 * Pretty finnicky, I don't want to implement a
 * full-blown parser so I rely on sync codes and
 * fixed block sizes, and default sample rates.
 * You basically have to decode a FLAC file to
 * truly find the frame boundaries, or wrap it
 * in Ogg.
 *
 * The last block of a FLAC file can be less than
 * the fixed block size, this demo will drop
 * the last block in that case.
 *
 * This *should* work with fairly standard
 * FLAC files made using the reference encoder
 * but YMMV */

#define MINIFMP4_IMPLEMENTATION
#include "../minifmp4.h"

#include "flac.h"
#include <stdio.h>
#include <stdlib.h>

static size_t filewrite(const void* data, size_t len, void* userdata) {
    return fwrite(data, 1, len, (FILE *)userdata);
}

int main(int argc, const char* argv[]) {
    int ret;
    FILE *input;
    FILE *output;
    uint8_t* frame_data;
    size_t frame_size;
    fmp4_mux* mux;
    fmp4_track* track;
    fmp4_sample_info info;

    flac_info finfo;

    ret = 1;
    input = NULL;
    output = NULL;
    frame_data = NULL;
    frame_size = 0;
    mux = NULL;
    track = NULL;

    if(argc < 3) {
        fprintf(stderr,"Usage: %s /path/to/input /path/to/output\n",argv[0]);
        goto cleanup;
    }

    if((input = fopen(argv[1],"rb")) == NULL) {
        fprintf(stderr,"unable to open %s for input\n",argv[1]);
        goto cleanup;
    }

    if((output = fopen(argv[2],"wb")) == NULL) {
        fprintf(stderr,"unable to open %s for output\n",argv[2]);
        goto cleanup;
    }

    if(flac_grab_info(input, &finfo) != 0) goto cleanup;
    if(finfo.min_block_size == 0 || finfo.max_block_size == 0 || finfo.min_block_size != finfo.max_block_size) {
        fprintf(stderr,"this only works with fixed-size blocks\n");
        goto cleanup;
    }
    if(finfo.min_frame_size == 0 || finfo.max_frame_size == 0) {
        fprintf(stderr,"this only works with known min/max frame sizes\n");
        goto cleanup;
    }

    switch(finfo.sample_rate) {
        case 8000: /* fall-through */
        case 16000: /* fall-through */
        case 22050: /* fall-through */
        case 24000: /* fall-through */
        case 32000: /* fall-through */
        case 44100: /* fall-through */
        case 48000: /* fall-through */
        case 88200: /* fall-through */
        case 96000: /* fall-through */
        case 176400: /* fall-through */
        case 192000: break;
        default: {
            fprintf(stderr,"Unsupported sample rate: %u\n",finfo.sample_rate);
            goto cleanup;
        }
    }

    fmp4_sample_info_init(&info);
    info.duration = finfo.min_block_size;

    mux = fmp4_mux_new(NULL);
    if(mux == NULL) {
        fprintf(stderr,"error allocating muxer\n");
        goto cleanup;
    }
    track = fmp4_mux_new_track(mux);
    if(track == NULL) {
        fprintf(stderr,"error allocating track\n");
        goto cleanup;
    }

    fmp4_track_set_language(track, "und");
    fmp4_track_set_time_scale(track, finfo.sample_rate);
    fmp4_track_set_stream_type(track, FMP4_STREAM_TYPE_AUDIO);
    fmp4_track_set_codec(track, FMP4_CODEC_FLAC);
    fmp4_track_set_audio_channels(track, finfo.channels);
    fmp4_track_set_dsi(track, finfo.streaminfo, sizeof(finfo.streaminfo));
    fmp4_track_set_default_sample_info(track, &info);

    if(fmp4_mux_write_init(mux, filewrite, output) != FMP4_OK) {
        fprintf(stderr,"error writing init\n");
        goto cleanup;
    }

    while( (frame_data = flac_grab_frame(input,&frame_size,finfo.min_block_size, finfo.min_frame_size, finfo.sample_rate,finfo.channels)) != NULL) {
        if(frame_size > finfo.max_frame_size) {
            fprintf(stderr,"error, frame size too big\n");
            goto cleanup;
        }
        info.size = frame_size;
        if(fmp4_track_add_sample(track,frame_data,&info) != FMP4_OK) {
            fprintf(stderr,"error adding flac frame\n");
            goto cleanup;
        }
        free(frame_data);
        frame_data = NULL;
    }

    if(fmp4_mux_write_segment(mux, filewrite, output) != FMP4_OK) {
        fprintf(stderr,"error writing segment\n");
        goto cleanup;
    }

    ret = 0;

    cleanup:
    if(input != NULL) fclose(input);
    if(output != NULL) fclose(output);
    if(mux != NULL) fmp4_mux_free(mux);
    if(frame_data != NULL) free(frame_data);
    return ret;
}
