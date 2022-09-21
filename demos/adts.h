#ifndef DEMO_ADTS
#define DEMO_ADTS

#include <stdint.h>
#include <stdio.h>

struct adts_header {
    uint16_t syncword;
    uint8_t version;
    uint8_t layer;
    uint8_t no_crc;
    uint8_t profile;
    uint8_t freq_index;
    uint8_t private;
    uint8_t ch_config;
    uint8_t original;
    uint8_t home;
    uint8_t copyright_bit;
    uint8_t copyright_start;
    uint16_t frame_length;
    uint8_t buffer_fullness;
    uint8_t frames;
    uint16_t crc;
    uint8_t aacAudioConfig[2];
};

typedef struct adts_header adts_header;

#ifdef __cplusplus
extern "C" {
#endif

int adts_grab_header(FILE* input, adts_header* header);

#ifdef __cplusplus
}
#endif

#endif
