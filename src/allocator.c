#include "allocator.h"
#include "fmp4_stdlib.h"

FMP4_PRIVATE
void*
fmp4_default_malloc(size_t len, void* userdata) {
    (void)userdata;
    return malloc(len);
}

FMP4_PRIVATE
void*
fmp4_default_realloc(void* ptr, size_t len, void* userdata) {
    (void)userdata;
    return realloc(ptr, len);
}

FMP4_PRIVATE
void
fmp4_default_free(void* ptr, void* userdata) {
    (void)userdata;
    free(ptr);
}

FMP4_PRIVATE
const fmp4_allocator fmp4_default_allocator = {
    NULL,
    fmp4_default_malloc,
    fmp4_default_realloc,
    fmp4_default_free
};
