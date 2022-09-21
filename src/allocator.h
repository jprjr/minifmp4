#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include "defines.h"
#include "structs.h"

FMP4_PRIVATE
void*
fmp4_default_malloc(size_t len, void* userdata);

FMP4_PRIVATE
void*
fmp4_default_realloc(void* ptr, size_t len, void* userdata);

FMP4_PRIVATE
void
fmp4_default_free(void* ptr, void* userdata);

FMP4_PRIVATE
const fmp4_allocator fmp4_default_allocator;

#endif
