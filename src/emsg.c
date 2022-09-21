#include "emsg.h"

FMP4_API
fmp4_result
fmp4_emsg_validate(const fmp4_emsg* emsg) {
    if(emsg->version > 1) return FMP4_INVALIDEMSGVER;
    if(emsg->timescale == 0) return FMP4_TIMESCALEINVALID;
    if(emsg->scheme_id_uri.len == 0) return FMP4_EMSGSCHEMENOTSET;
    if(emsg->value.len == 0) return FMP4_EMSGVALUENOTSET;
    if(emsg->message.len == 0) return FMP4_EMSGMESSAGENOTSET;

    return FMP4_OK;
}
