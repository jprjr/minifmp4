#include "flac.h"
#include <string.h>
#include <stdlib.h>

static uint32_t unpack_uint32be(const uint8_t* b) {
    return (((uint32_t)b[0])<<24) |
           (((uint32_t)b[1])<<16) |
           (((uint32_t)b[2])<<8 ) |
           (((uint32_t)b[3])    );
}

static uint32_t unpack_uint24be(const uint8_t* b) {
    return (((uint32_t)b[0])<<16) |
           (((uint32_t)b[1])<<8 ) |
           (((uint32_t)b[2])    );
}

static uint32_t unpack_uint16be(const uint8_t* b) {
    return (((uint32_t)b[0])<<8) |
           (((uint32_t)b[1]));
}

int32_t flac_block_size(uint8_t val) {
    switch(val & 0x0F) {
        case 0x00: return -1;
        case 0x01: return 192;
        case 0x02: return 576;
        case 0x03: return 1152;
        case 0x04: return 2304;
        case 0x05: return 4608;
        case 0x06: return -8;
        case 0x07: return -16;
        case 0x08: return 256;
        case 0x09: return 512;
        case 0x0A: return 1024;
        case 0x0B: return 2048;
        case 0x0C: return 4096;
        case 0x0D: return 8192;
        case 0x0E: return 16384;
        case 0x0F: return 32768;
        default: break;
    }
    return -1;
}

int32_t flac_sample_rate(uint8_t val) {
    switch(val & 0x0F) {
        case 0x00: return 0; /* get from STREAMINFO */
        case 0x01: return 88200;
        case 0x02: return 176400;
        case 0x03: return 192000;
        case 0x04: return 8000;
        case 0x05: return 16000;
        case 0x06: return 22050;
        case 0x07: return 24000;
        case 0x08: return 32000;
        case 0x09: return 44100;
        case 0x0A: return 48000;
        case 0x0B: return 96000;
        case 0x0C: return -8; /* get 8-bit rate in kHz from header */
        case 0x0D: return -16; /*  get 16-bit rate in Hz from header */
        case 0x0E: return -32; /* get 16-bit rate in kHz from header */
        case 0x0F: return -1; /* invalid */
        default: break;
    }
    return -1;
}

uint8_t*
flac_grab_frame(FILE* input, size_t* len, uint16_t fixed, uint32_t min_frame_size, uint32_t srate, uint8_t channels) {
    /* assumption is FILE is seekable! */
    size_t sync_start = 0;
    size_t sync_end   = 0;
    size_t size = 0;
    uint8_t* buffer = NULL;
    uint8_t testbuf[2];
    uint16_t sync = 0;
    uint16_t mask = 0xFFFE;
    uint16_t match = 0xFFF8;
    int32_t block_size = 0;
    int32_t sample_rate = 0;
    uint8_t channel_assignment = 0;
    int seeking;

    if(!fixed) match = 0xFFF9;

    /* search for the first sync code */
    if(fread(testbuf,1,2,input) != 2) return NULL;

    find_sync_start:

    sync = unpack_uint16be(testbuf);
    while( (sync & mask) != match) {
        testbuf[0] = testbuf[1];
        if( fread(&testbuf[1],1,1,input) != 1) return NULL;
        sync = unpack_uint16be(testbuf);
    }

    sync_start = ftell(input) - 2;

    /* read the block size + sample rate */
    testbuf[0] = testbuf[1];
    if(fread(&testbuf[1],1,1,input) != 1) return NULL;
    block_size = flac_block_size(testbuf[1] >> 4);
    sample_rate = flac_sample_rate(testbuf[1] & 0x0F);

    if(block_size <= 0) goto find_sync_start;
    if((uint16_t)block_size != fixed) goto find_sync_start;
    if(sample_rate <= 0) goto find_sync_start;
    if((uint32_t)sample_rate != srate) goto find_sync_start;

    /* read the channel assignment + sample size + reserved */
    testbuf[0] = testbuf[1];
    if(fread(&testbuf[1],1,1,input) != 1) return NULL;
    channel_assignment = (testbuf[1] >> 4) + 1;

    if(channel_assignment > 8) channel_assignment = 2;
    if(channel_assignment != channels) {
        fprintf(stderr,"error, channel mis-match\n");
        return NULL;
    }

    /* read the first UTF-8 number */
    testbuf[0] = testbuf[1];
    if(fread(&testbuf[1],1,1,input) != 1) return NULL;
    /* read the rest */
    while( (testbuf[1] & 0x80) ) {
        testbuf[0] = testbuf[1];
        if(fread(&testbuf[1],1,1,input) != 1) return NULL;
    }

    /* read the crc8 */
    if(fread(testbuf,1,1,input) != 1) return NULL;

    seeking = 1;
    sync = 0;
    testbuf[0] = 0;
    testbuf[1] = 0;

    while(seeking) {
        testbuf[0] = testbuf[1];
        if( fread(&testbuf[1],1,1,input) != 1) break;

        sync_end = ftell(input);
        sync = unpack_uint16be(testbuf);

        if( (sync & mask) == match) {
            if(sync_end - sync_start - 2 < min_frame_size) continue;
            /* read the next byte and check for the right blocksize and
             * sample rate */
            testbuf[0] = testbuf[1];
            if(fread(testbuf,1,1,input) != 1) return NULL;

            block_size = flac_block_size(testbuf[0] >> 4);
            sample_rate = flac_sample_rate(testbuf[0] & 0x0F);

            if(block_size <= 0) continue;
            if(sample_rate <= 0) continue;
            if((uint16_t)block_size != fixed) continue;
            if((uint32_t)sample_rate != srate) continue;
            fseek(input,-1,SEEK_CUR);
            seeking = 0;
            break;
        }
    }

    sync_end = ftell(input);

    /* if we saw the sync code we'll need to go back 2 bytes */
    if( (sync & mask) == match) sync_end -= 2;

    size = sync_end - sync_start;
    buffer = (uint8_t*)malloc(size);
    if(buffer == NULL) return NULL;
    if(fseek(input,sync_start,SEEK_SET) != 0) {
        free(buffer);
        return NULL;
    }
    if(fread(buffer,1,size,input) != size) {
        free(buffer);
        return NULL;
    }
    *len = size;

    return buffer;
}



int flac_grab_info(FILE* input, flac_info* info) {
    uint8_t buffer[42];
    uint32_t len;
    size_t pos;
    uint8_t last_block;
    if(fread(buffer,1,42,input) != 42) return -1;
    if(buffer[0] != 'f' ||
       buffer[1] != 'L' ||
       buffer[2] != 'a' ||
       buffer[3] != 'C') {
        fprintf(stderr,"error: missing fLaC signature\n");
        return -1;
    }
    if((buffer[4] & 0x7F) != 0) {
        fprintf(stderr,"error: first block is not a streaminfo block\n");
        return -1;
    }
    len = unpack_uint24be(&buffer[5]);
    if(len != 34) {
        fprintf(stderr,"error: STREAMINFO block is a weird size\n");
        return -1;
    }
    memcpy(info->streaminfo,&buffer[4],38);
    info->streaminfo[0] |= 0x80; /* set the last-metadata-block flag */

    pos = 4;
    info->min_block_size = unpack_uint16be(&info->streaminfo[pos]);
    pos += 2;
    info->max_block_size = unpack_uint16be(&info->streaminfo[pos]);
    pos += 2;
    info->min_frame_size = unpack_uint24be(&info->streaminfo[pos]);
    pos += 3;
    info->max_frame_size = unpack_uint24be(&info->streaminfo[pos]);
    pos += 3;
    info->sample_rate = unpack_uint24be(&info->streaminfo[pos]) >> 4;
    pos += 2; /* only read 20 bits */

    info->channels = ((info->streaminfo[pos] & 0x0E) >> 1);
    info->channels++;
    info->bps = (info->streaminfo[pos] & 0x01) << 4;
    pos++;
    info->bps += (info->streaminfo[pos] & 0xF0) >> 4;
    info->bps++;

    info->total_samples = info->streaminfo[pos] & 0x0F;
    info->total_samples <<= 32;
    pos++;
    info->total_samples += unpack_uint32be(&info->streaminfo[pos]);
    pos += 4;

    last_block = buffer[4] & 0x80;
    while(!last_block) {
        if(fread(buffer,1,4,input) != 4) return -1;
        last_block = buffer[0] & 0x80;
        len = unpack_uint24be(&buffer[1]);
        fseek(input,len,SEEK_CUR);
    }

    return 0;
}
