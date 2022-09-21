#include "loudness.h"
#include "membuf.h"
#include "init.h"
#include "close.h"
#include "measurement.h"

FMP4_API
fmp4_measurement*
fmp4_loudness_new_measurement(fmp4_loudness* loudness) {
    fmp4_measurement* measurement = fmp4_measurement_new(loudness->allocator);
    if(measurement == NULL) return measurement;

    if(fmp4_membuf_cat(&loudness->alloc_measurement,&measurement,sizeof(fmp4_measurement*)) != FMP4_OK) {
        fmp4_measurement_free(measurement);
        return NULL;
    }

    if(fmp4_loudness_add_measurement(loudness,measurement) != FMP4_OK) {
        fmp4_measurement_free(measurement);
        fmp4_membuf_uncat(&loudness->alloc_measurement, &measurement, sizeof(fmp4_measurement*));
        return NULL;
    }

    return measurement;
}


FMP4_API
fmp4_result
fmp4_loudness_add_measurement(fmp4_loudness* loudness, const fmp4_measurement* measurement) {
    return fmp4_membuf_cat(&loudness->measurements, &measurement, sizeof(fmp4_measurement*));
}


FMP4_API
fmp4_result
fmp4_loudness_validate(const fmp4_loudness* loudness) {
    fmp4_result res;
    fmp4_measurement** measurements;
    size_t i;
    size_t len;
    if(loudness->type == FMP4_LOUDNESS_UNDEF) return FMP4_LOUDNESSNOTSET;
    if(loudness->sample_peak == 0 && loudness->true_peak == 0) return FMP4_LOUDNESSNOTSET;
    if(loudness->true_peak != 0) {
        if(loudness->system > 5) return FMP4_SYSTEMINVALID;
        if(loudness->reliability > 3) return FMP4_RELIABILITYINVALID;
    }

    measurements = (fmp4_measurement**)loudness->measurements.x;
    len = loudness->measurements.len / sizeof(fmp4_measurement*);
    for(i=0;i<len;i++) {
        if( (res = fmp4_measurement_validate(measurements[i])) != FMP4_OK) return res;
    }

    return FMP4_OK;
}
