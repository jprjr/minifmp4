#ifndef DEFINES_H
#define DEFINES_H
#if !defined(FMP4_API)
    #ifdef FMP4_DLL
        #ifdef _WIN32
            #define FMP4_DLL_IMPORT  __declspec(dllimport)
            #define FMP4_DLL_EXPORT  __declspec(dllexport)
            #define FMP4_DLL_PRIVATE static
        #else
            #if defined(__GNUC__) && __GNUC__ >= 4
                #define FMP4_DLL_IMPORT  __attribute__((visibility("default")))
                #define FMP4_DLL_EXPORT  __attribute__((visibility("default")))
                #define FMP4_DLL_PRIVATE __attribute__((visibility("hidden")))
            #else
                #define FMP4_DLL_IMPORT
                #define FMP4_DLL_EXPORT
                #define FMP4_DLL_PRIVATE static
            #endif
        #endif

        #ifdef MINIFMP4_IMPLEMENTATION
            #define FMP4_API  FMP4_DLL_EXPORT
        #else
            #define FMP4_API  FMP4_DLL_IMPORT
        #endif
        #define FMP4_PRIVATE FMP4_DLL_PRIVATE
    #else
        #define FMP4_API extern
        #define FMP4_PRIVATE static
    #endif
#endif

enum fmp4_result {
    FMP4_CHANNELSINVALID    = -23,
    FMP4_TIMESCALEINVALID   = -22,
    FMP4_EMSGSCHEMENOTSET   = -21,
    FMP4_EMSGVALUENOTSET    = -20,
    FMP4_EMSGMESSAGENOTSET  = -19,
    FMP4_LOUDNESSNOTSET     = -18,
    FMP4_METHODINVALID      = -17,
    FMP4_RELIABILITYINVALID = -16,
    FMP4_SYSTEMINVALID      = -15,
    FMP4_PEAKINVALID        = -14,
    FMP4_NOSAMPLES          = -13,
    FMP4_NOTRACKS           = -12,
    FMP4_MEMUNDERFLOW       = -11,
    FMP4_INVALIDEMSGVER     = -10,
    FMP4_MISSINGDSI         = -9,
    FMP4_OBJECTINVALID      = -8,
    FMP4_CODECINVALID       = -7,
    FMP4_STREAMINVALID      = -6,
    FMP4_ESTAGTOOBIG        = -5,
    FMP4_BOXTOOBIG          = -4,
    FMP4_STACKERROR         = -3,
    FMP4_WRITEERR           = -2,
    FMP4_OUTOFMEM           = -1,
    FMP4_OK                 =  0,
};

typedef enum fmp4_result fmp4_result;

#endif
