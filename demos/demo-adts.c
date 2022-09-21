/* a demo program that reads an existing AAC file (ADTS) and
 * re-muxes into HLS with timed ID3 metadata and loudness
 * metadata, basically tries to exercise all the features of
 * the library.
 *
 * ASSUMPTIONS:
 *
 * The AAC data is AAC-LC.
 *
 * The AAC has an encoder delay of 1024 frames and
 * requires decoding 1 pre-roll frame (this is the
 * default for the ffmpeg AAC encoder).
 *
 * The AAC has had the loudness normalized to
 * -18 LUFS with -1 dB TP - this is the default
 * scanning mode of loudgain if you use the -k (no clip)
 * option.
 *
 * You can produce a valid AAC file via loudgain and ffmpeg.
 *
 * Assuming we're working with a FLAC file as our source:
 *
 * loudgain -s e -k source.flac
 * ffmpeg -i af "volume=replaygain=track" -vn -c:a aac source.aac
 */

#define MINIFMP4_IMPLEMENTATION
#include "../minifmp4.h"

#include "adts.h"
#include "id3.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096


static const char* AOID3_SCHEME_ID_URI = "https://aomedia.org/emsg/ID3";
static const char* AOID3_VALUE = "0";

static size_t filewrite(const void* data, size_t len, void* userdata) {
    return fwrite(data, 1, len, (FILE *)userdata);
}

int main(int argc, const char* argv[]) {
    int ret;
    fmp4_mux* mux;
    fmp4_track* track;
    fmp4_emsg* emsg;
    fmp4_loudness* loudness;
    fmp4_measurement* measurement;
    FILE *input;
    FILE *output;
    FILE *hls_output;
    uint8_t *buffer;
    size_t buffer_a;
    char *path;
    unsigned int segment_no;
    unsigned int packet_no;
    unsigned int packets_per_seg;
    unsigned int freq;
    id3 id3_data;

    adts_header header;
    fmp4_sample_info info;

    ret = 1;

    mux = NULL;
    track = NULL;
    loudness = NULL;
    measurement = NULL;
    emsg = NULL;
    input = NULL;
    output = NULL;
    buffer = NULL;
    path = NULL;
    segment_no = 0;
    packet_no = 0;
    packets_per_seg =0;

    id3_init(&id3_data);

    if(argc < 3) {
        fprintf(stderr,"Usage: %s /path/to/input /path/to/output/folder\n", argv[0]);
        goto cleanup;
    }

    fprintf(stderr,"allocating buffer\n");
    buffer = (uint8_t*)malloc(sizeof(uint8_t)*BUFFER_SIZE);
    if(buffer == NULL) abort();
    buffer_a = BUFFER_SIZE;

    fprintf(stderr,"allocating path\n");
    path = (char *)malloc(sizeof(uint8_t)*BUFFER_SIZE);
    if(path == NULL) abort();

    fprintf(stderr,"opening %s\n",argv[1]);
    input = fopen(argv[1],"rb");
    if(input == NULL) {
        fprintf(stderr,"Unable to open %s for reading\n", argv[1]);
        goto cleanup;
    }

    snprintf(path,BUFFER_SIZE,"%s/stream.m3u8",argv[2]);
    fprintf(stderr,"opening %s\n",path);
    hls_output = fopen(path,"wb");
    if(hls_output == NULL) {
        fprintf(stderr,"Unable to open %s for writing\n", path);
        goto cleanup;
    }

    fprintf(stderr,"allocating muxer\n");
    mux = fmp4_mux_new(NULL);
    if(mux == NULL) {
        fprintf(stderr,"error allocating the muxer\n");
        goto cleanup;
    }

    fprintf(stderr,"allocating track\n");
    track = fmp4_mux_new_track(mux);
    if(track == NULL) {
        fprintf(stderr,"error allocating the track\n");
        goto cleanup;
    }

    fprintf(stderr,"allocating emsg\n");
    emsg = fmp4_mux_new_emsg(mux);
    if(emsg == NULL) {
        fprintf(stderr,"error allocating emsg\n");
        goto cleanup;
    }

    fprintf(stderr,"allocating loudness\n");
    loudness = fmp4_track_new_loudness(track);
    if(loudness == NULL) {
        fprintf(stderr,"error allocating the loudness info\n");
        goto cleanup;
    }

    fprintf(stderr,"allocating loudness measurement\n");
    measurement = fmp4_loudness_new_measurement(loudness);
    if(measurement == NULL) {
        fprintf(stderr,"error allocating the loudness measurement\n");
        goto cleanup;
    }

    fprintf(stderr,"grabbing ADTS header\n");
    if(adts_grab_header(input, &header) != 0) {
        fprintf(stderr,"unable to parse ADTS header\n");
        goto cleanup;
    }

    if(header.ch_config == 0) {
        fprintf(stderr,"error: aac file has unsupported channel config\n");
        goto cleanup;
    }

    switch(header.freq_index) {
        case 0: freq = 96000; break;
        case 1: freq = 88200; break;
        case 2: freq = 64000; break;
        case 3: freq = 48000; break;
        case 4: freq = 44100; break;
        case 5: freq = 32000; break;
        case 6: freq = 24000; break;
        case 7: freq = 22050; break;
        case 8: freq = 16000; break;
        case 9: freq = 12000; break;
        case 10: freq = 11025; break;
        case 11: freq = 8000; break;
        case 12: freq = 7350; break;
        default: {
            fprintf(stderr,"error: aac file has unsupported frequency index\n");
            goto cleanup;
        }
    }

    packets_per_seg = (freq * 2) / 1024;

    fmp4_sample_info_init(&info);
    info.duration = 1024;

    fprintf(stderr,"configuring muxer\n");
    fmp4_mux_add_brand(mux, "aid3");

    fprintf(stderr,"configuring loudness\n");
    fmp4_loudness_set_type(loudness, FMP4_LOUDNESS_TRACK);
    fmp4_loudness_set_true_peak(loudness,-1.0f);
    fmp4_loudness_set_system(loudness,1);
    fmp4_loudness_set_reliability(loudness,3);

    fprintf(stderr,"configuring loudness measurement\n");
    fmp4_measurement_set_method(measurement,1);
    fmp4_measurement_set_value(measurement,-23.0f);
    fmp4_measurement_set_system(measurement,1);
    fmp4_measurement_set_reliability(measurement,3);

    fprintf(stderr,"configuring emsg\n");
    emsg->version = 1;
    emsg->timescale = freq;
    emsg->id = 0;
    fmp4_emsg_set_scheme_id_uri(emsg, AOID3_SCHEME_ID_URI);
    fmp4_emsg_set_value(emsg, AOID3_VALUE);

    fprintf(stderr,"configuring track\n");
    fmp4_track_set_language(track, "und");
    fmp4_track_set_time_scale(track, freq);
    fmp4_track_set_stream_type(track, FMP4_STREAM_TYPE_AUDIO);
    fmp4_track_set_codec(track, FMP4_CODEC_MP4A);
    fmp4_track_set_object_type(track, FMP4_OBJECT_TYPE_AAC);
    fmp4_track_set_audio_channels(track, header.ch_config);
    fmp4_track_set_dsi(track, header.aacAudioConfig, 2);
    fmp4_track_set_default_sample_info(track, &info);
    fmp4_track_set_encoder_delay(track,1024);
    fmp4_track_set_roll_distance(track,-1);

    snprintf(path,BUFFER_SIZE,"%s/init.mp4",argv[2]);
    fprintf(stderr,"opening %s\n",path);
    output = fopen(path,"wb");
    if(output == NULL) {
        fprintf(stderr,"error: unable to open %s for output\n",path);
        goto cleanup;
    }

    fprintf(stderr,"writing initialization segment\n");
    fmp4_mux_write_init(mux, filewrite, output);
    fclose(output);
    output = NULL;

    fprintf(hls_output,"#EXTM3U\n");
    fprintf(hls_output,"#EXT-X-TARGETDURATION:2\n");
    fprintf(hls_output,"#EXT-X-VERSION:7\n");
    fprintf(hls_output,"#EXT-X-MEDIA-SEQUENCE:0\n");
    fprintf(hls_output,"#EXT-X-PLAYLIST-TYPE:VOD\n");
    fprintf(hls_output,"#EXT-X-MAP:URI=\"init.mp4\"\n");

    snprintf(path,BUFFER_SIZE,"Segment %u",++segment_no);
    id3_add_tag(&id3_data,"TIT2",path);
    fmp4_emsg_set_message(emsg,id3_data.data,id3_data.len);
    id3_reset(&id3_data);

    for(; ;) {
        if(header.frame_length > buffer_a) {
            buffer = realloc(buffer,buffer_a + BUFFER_SIZE);
            if(buffer == NULL) abort();
            buffer_a += BUFFER_SIZE;
        }
        if(fread(buffer,1,header.frame_length,input) != header.frame_length) {
            fprintf(stderr,"error grabbing ADTS frame\n");
            break;
        }
        info.size = header.frame_length;
        fmp4_track_add_sample(track, buffer, &info);
        packet_no++;

        if(packet_no == packets_per_seg) {
            emsg->presentation_time = fmp4_track_get_base_media_decode_time(track);
            emsg->event_duration = fmp4_track_get_trun_sample_count(track);

            snprintf(path,BUFFER_SIZE,"%s/%08u.m4s",argv[2],segment_no);
            output = fopen(path,"wb");
            fprintf(stderr,"opening %s\n",path);
            if(output == NULL) {
                fprintf(stderr,"error: unable to open %s for output\n", path);
            }

            fmp4_mux_write_segment(mux, filewrite, output);
            fclose(output);
            output = NULL;

            fmp4_mux_add_emsg(mux,emsg);

            fprintf(hls_output,"#EXTINF:%f,\n",((double)packet_no) * 1024.0f / ((double)freq));
            fprintf(hls_output,"%08u.m4s\n",segment_no);

            snprintf(path,BUFFER_SIZE,"Segment %u",++segment_no);
            id3_add_tag(&id3_data,"TIT2",path);
            fmp4_emsg_set_message(emsg,id3_data.data,id3_data.len);
            id3_reset(&id3_data);
            emsg->id++;

            packet_no = 0;
        }

        if(adts_grab_header(input, &header) != 0) {
            fprintf(stderr,"unable to grab ADTS header, assuming EOF\n");
            break;
        }
    }

    if(packet_no > 0) {
        emsg->presentation_time = fmp4_track_get_base_media_decode_time(track);
        emsg->event_duration = track->trun_sample_count;

        snprintf(path,BUFFER_SIZE,"%s/%08u.m4s",argv[2],segment_no);
        fprintf(stderr,"opening %s\n",path);
        output = fopen(path,"wb");
        if(output == NULL) {
            fprintf(stderr,"error: unable to open %s for output\n", path);
        }

        fmp4_mux_write_segment(mux, filewrite, output);
        fclose(output);
        output = NULL;

        fprintf(hls_output,"#EXTINF:%f,\n",((double)packet_no) * 2.0f / ((double)freq));
        fprintf(hls_output,"%08u.m4s\n",segment_no);
    }
    fprintf(hls_output,"#EXT-X-ENDLIST\n");

    ret = 0;
    cleanup:
    fprintf(stderr,"cleaning up\n");

    if(input != NULL) fclose(input);
    if(output != NULL) fclose(output);
    if(hls_output != NULL) fclose(hls_output);
    if(buffer != NULL) free(buffer);
    if(path != NULL) free(path);

    if(mux != NULL) fmp4_mux_free(mux);
    if(track != NULL) fmp4_track_free(track);
    if(loudness != NULL) fmp4_loudness_free(loudness);
    if(measurement != NULL) fmp4_measurement_free(measurement);
    if(emsg != NULL) fmp4_emsg_free(emsg);
    id3_free(&id3_data);

    return ret;
}
