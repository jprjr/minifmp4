#include "measurement.h"

FMP4_API
fmp4_result
fmp4_measurement_validate(const fmp4_measurement* measurement) {
    if(measurement->method == 0)      return FMP4_METHODINVALID;
    if(measurement->system == 0)      return FMP4_SYSTEMINVALID;
    if(measurement->reliability == 0) return FMP4_RELIABILITYINVALID;
    return FMP4_OK;
}
