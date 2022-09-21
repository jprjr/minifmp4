#include "membuf.h"
#include "fmp4_string.h"

#ifndef FMP4_MEMBUF_BLOCKSIZE
#define FMP4_MEMBUF_BLOCKSIZE 4096
#endif

FMP4_PRIVATE
void
fmp4_membuf_init(fmp4_membuf* buf, const fmp4_allocator* allocator) {
    buf->x = NULL;
    buf->a = 0;
    buf->len = 0;
    buf->allocator = allocator;
}

FMP4_PRIVATE
void
fmp4_membuf_reset(fmp4_membuf* buf) {
    buf->len = 0;
}

FMP4_PRIVATE
void
fmp4_membuf_free(fmp4_membuf* buf) {
    if(buf->x != NULL) buf->allocator->free(buf->x, buf->allocator->userdata);
    buf->x = NULL;
    buf->a = 0;
    buf->len = 0;
}

FMP4_PRIVATE
fmp4_result
fmp4_membuf_ready(fmp4_membuf* buf, size_t len) {
    uint8_t* t;
    size_t a;
    if(len > buf->a) {
        a = (len + (FMP4_MEMBUF_BLOCKSIZE-1)) & -FMP4_MEMBUF_BLOCKSIZE;
        t = buf->allocator->realloc(buf->x, a, buf->allocator->userdata);
        if(t == NULL) return FMP4_OUTOFMEM;
        buf->x = t;
        buf->a = a;
    }
    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_membuf_readyplus(fmp4_membuf* buf, size_t len) {
    return fmp4_membuf_ready(buf, buf->len + len);
}

FMP4_PRIVATE
fmp4_result
fmp4_membuf_cat(fmp4_membuf* buf, const void* src, size_t len) {
    fmp4_result r;
    if( (r = fmp4_membuf_readyplus(buf, len)) != 0) return r;
    memcpy(&buf->x[buf->len], src, len);
    buf->len += len;
    return FMP4_OK;
}

FMP4_PRIVATE
fmp4_result
fmp4_membuf_uncat(fmp4_membuf* buf, void* dest, size_t len) {
    if(buf->len < len) return FMP4_MEMUNDERFLOW;
    memcpy(dest,&buf->x[buf->len - len],len);
    buf->len -= len;
    return FMP4_OK;
}

#undef FMP4_MEMBUF_BLOCKSIZE

