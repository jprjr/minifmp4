#include "adts.h"

int adts_grab_header(FILE* input, adts_header* header) {
    uint8_t buffer[7]; /* we just throw away CRC data */
    uint16_t audioConfig;

    if(fread(buffer,1,7,input) != 7) return -1;

    header->syncword = (((uint16_t)buffer[0]) << 4) | (((uint16_t)buffer[1]) >> 4);
    header->version =  (buffer[1] & 0x08) >> 3;
    header->layer =    (buffer[1] & 0x06) >> 1;
    header->no_crc =   (buffer[1] & 0x01);
    header->profile =  (buffer[2] & 0xC0) >> 6;
    header->freq_index =  (buffer[2] & 0x3C) >> 2;
    header->private =  (buffer[2] & 0x02) >> 1;
    header->ch_config = ((buffer[2] & 0x01) << 2) | ((buffer[3] & 0xC0) >> 6);
    header->original = (buffer[3] & 0x20) >> 5;
    header->home = (buffer[3] & 0x10) >> 4;
    header->copyright_bit = (buffer[3] & 0x08) >> 3;
    header->copyright_start = (buffer[3] & 0x04) >> 2;
    header->frame_length = ((buffer[3] & 0x02) << 11) | ((buffer[4] << 3)) | ((buffer[5] & 0xE0) >> 5);
    header->buffer_fullness = ((buffer[5] & 0x1F) << 2) | ((buffer[6] & 0xFC) >> 2);
    header->frames = buffer[6] & 0x03;
    if(header->no_crc == 0) {
        if(fread(buffer,1,2,input) != 2) return -1;
        header->crc = buffer[0] << 8 | buffer[1];
        header->frame_length -= 9;
    } else {
        header->frame_length -= 7;
        header->crc = 0;
    }

    audioConfig = (0x02 << 11) | (header->freq_index << 7) | (header->ch_config << 3) | 0;
    header->aacAudioConfig[0] = (audioConfig >> 8) & 0xFF;
    header->aacAudioConfig[1] = (audioConfig >> 0) & 0xFF;

    return 0;
}

