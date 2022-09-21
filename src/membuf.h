#ifndef MEMBUF_H
#define MEMBUF_H
#include "defines.h"
#include "structs.h"

FMP4_PRIVATE
void
fmp4_membuf_init(fmp4_membuf* buf, const fmp4_allocator* allocator);

FMP4_PRIVATE
void
fmp4_membuf_reset(fmp4_membuf* buf);

FMP4_PRIVATE
void
fmp4_membuf_free(fmp4_membuf* buf);

FMP4_PRIVATE
fmp4_result
fmp4_membuf_ready(fmp4_membuf* buf, size_t len);

FMP4_PRIVATE
fmp4_result
fmp4_membuf_readyplus(fmp4_membuf* buf, size_t len);

/* appends data to the end of the buffer */
FMP4_PRIVATE
fmp4_result
fmp4_membuf_cat(fmp4_membuf* buf, const void* src, size_t len);

/* removes data from the end of the buffer */
FMP4_PRIVATE
fmp4_result
fmp4_membuf_uncat(fmp4_membuf* buf, void* dest, size_t len);

#define fmp4_membuf_push(buf, a) fmp4_membuf_cat(buf, &(a), sizeof(a))
#define fmp4_membuf_pop(buf, a) fmp4_membuf_uncat(buf, &(a), sizeof(a))
#endif
