#include "id3.h"

#include <stdlib.h>
#include <string.h>

static void pack_uint32_syncsafe(uint8_t *output, uint32_t val) {
    output[0] = (uint8_t)((val & 0x0FE00000) >> 21);
    output[1] = (uint8_t)((val & 0x001FC000) >> 14);
    output[2] = (uint8_t)((val & 0x00003F80) >> 7);
    output[3] = (uint8_t)((val & 0x0000007F));
}

static void pack_uint16be(uint8_t *output, uint16_t val) {
    output[0] = (uint8_t)(val >> 8);
    output[1] = (uint8_t)(val     );
}

static void id3_expand(id3* i, size_t len) {
    if(i->a >= i->len + len) return;
    i->data = (uint8_t*)realloc(i->data, i->len + len);
    if(i->data == NULL) abort();
    i->a = i->len + len;
}

void id3_init(id3* i) {
    i->data = NULL;
    i->a    = 0;
    id3_reset(i);
}

void id3_reset(id3* i) {
    i->len = 0;
    id3_expand(i,10);
    i->len = 10;

    i->data[0] = 'I';
    i->data[1] = 'D',
    i->data[2] = '3',
    i->data[3] = 0x04; /* ID3 v2.4 */
    i->data[4] = 0x00;
    i->data[5] = 0x00;
}

void id3_add_text_frame(id3* i, const char frame[4], uint16_t flags, uint8_t encoding, const uint8_t* data, uint32_t len) {
    id3_expand(i, 10 + 1 + len);

    i->data[i->len++] = frame[0];
    i->data[i->len++] = frame[1];
    i->data[i->len++] = frame[2];
    i->data[i->len++] = frame[3];
    pack_uint32_syncsafe(&i->data[i->len], len+1);
    i->len += 4;
    pack_uint16be(&i->data[i->len], flags);
    i->len += 2;
    i->data[i->len++] = encoding;
    memcpy(&i->data[i->len],data,len);
    i->len += len;

    pack_uint32_syncsafe(&i->data[6],i->len - 10);
}

void id3_add_tag(id3* i, const char tag[4], const char *val) {
    id3_add_text_frame(i,tag,0,ID3_UTF8,(const uint8_t*)val,strlen(val)+1);
}

void id3_free(id3* i) {
    if(i->data != NULL) free(i->data);
    i->data = NULL;
}
