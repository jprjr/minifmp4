#ifndef DEMO_FLAC
#define DEMO_FLAC

#include <stdint.h>
#include <stdio.h>

struct flac_info {
    uint16_t min_block_size;
    uint16_t max_block_size;
    uint32_t min_frame_size;
    uint32_t max_frame_size;
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bps;
    uint64_t total_samples;
    uint8_t md5[16];
    uint8_t streaminfo[38]; /* streaminfo block used as DSI */
};

typedef struct flac_info flac_info;

#ifdef __cplusplus
extern "C" {
#endif

int flac_grab_info(FILE* input, flac_info* info);
int32_t flac_block_size(uint8_t val);
int32_t flac_sample_rate(uint8_t val);
uint8_t* flac_grab_frame(FILE* input, size_t* len, uint16_t fixed, uint32_t min_frame_size, uint32_t srate, uint8_t channels);

#ifdef __cplusplus
}
#endif

#endif
