#include "streamvbyte_isadetection.h"
#ifdef STREAMVBYTE_X64



STREAMVBYTE_TARGET_SSE41
static inline void _write_sse41_opt(uint32_t *out, __m128i Vec) {
  _mm_storeu_si128((__m128i *)out, Vec);
}
STREAMVBYTE_UNTARGET_REGION


STREAMVBYTE_TARGET_SSE41
static inline void _decode_sse41_opt(uint8_t **key,
                                  const uint8_t *__restrict__ *dataPtrPtr, uint32_t **outPtrPtr) {
  uint8_t len;
  __m128i Data = _mm_loadu_si128((__m128i *)*dataPtrPtr);
  uint8_t *pshuf = (uint8_t *) &shuffleTable[**key];
  __m128i Shuf = *(__m128i *)pshuf;
#ifdef AVOIDLENGTHLOOKUP
  // this avoids the dependency on lengthTable,
  // see https://github.com/lemire/streamvbyte/issues/12
  len = pshuf[12 + (key >> 6)] + 1;
#else
  len = lengthTable[**key];
#endif
  Data = _mm_shuffle_epi8(Data, Shuf);
  *dataPtrPtr += len;

  _write_sse41_opt(*outPtrPtr, Data);
  *outPtrPtr += 4;
  *key += 1;
}
STREAMVBYTE_UNTARGET_REGION



STREAMVBYTE_TARGET_SSE41
const uint8_t *svb_decode_sse41_simple_opt(uint32_t *out,
                                     const uint8_t *__restrict__ keyPtr,
                                     const uint8_t *__restrict__ dataPtr,
                                     uint64_t count) {

  uint64_t keybytes = count / 4; // number of key bytes

  uint8_t *keyPtr8 = (uint8_t *)keyPtr;

  if (keybytes >= 1) {
    for (; keybytes > 0; keybytes--) {
      _decode_sse41_opt(&keyPtr8, &dataPtr, &out);
    }
  }
  return dataPtr;
}
STREAMVBYTE_UNTARGET_REGION
#endif
