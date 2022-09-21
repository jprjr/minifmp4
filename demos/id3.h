#ifndef DEMO_ID3H
#define DEMO_ID3H

#include <stdint.h>
#include <stddef.h>

struct id3 {
    uint8_t *data;
    size_t len;
    size_t a;
};

typedef struct id3 id3;

#define ID3_ISO88591 0x00
/* skipping UTF-16, not supporting those */
#define ID3_UTF8 0x03

#ifdef __cplusplus
extern "C" {
#endif

/* everything in these demos just returns void
 * because we'll just call abort() if we fail
 * to allocate memory */

void id3_init(id3* i);
void id3_reset(id3* i);
void id3_add_text_frame(id3* i, const char frame[4], uint16_t flags, uint8_t encoding, const uint8_t* data, uint32_t len);
void id3_free(id3* i);

/* just going to assume all tags are UTF-8 here */
void id3_add_tag(id3* i, const char tag[4], const char* val);

#ifdef __cplusplus
}
#endif

#endif
