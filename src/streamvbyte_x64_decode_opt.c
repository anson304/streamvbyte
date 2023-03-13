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

  len = lengthTable[**key];

  __m128i ShuffledData = _mm_shuffle_epi8(Data, Shuf);

  _write_sse41_opt(*outPtrPtr, ShuffledData);
  *outPtrPtr += 4;
  *key += 1;
  *dataPtrPtr += len;

  uint8_t len2 = lengthTable[**key];

  // if (len + len2 <= 16) {
    pshuf = (uint8_t *) &shuffleTable[**key];
    Shuf = *(__m128i *)pshuf;

    ShuffledData = _mm_shuffle_epi8(Data, Shuf);
    _write_sse41_opt(*outPtrPtr, ShuffledData);
    *outPtrPtr += 4;
    *key += 1;
    *dataPtrPtr += len2;
  // }
  
}
STREAMVBYTE_UNTARGET_REGION



STREAMVBYTE_TARGET_SSE41
const uint8_t *svb_decode_sse41_simple_opt(uint32_t *out,
                                     const uint8_t *__restrict__ keyPtr,
                                     const uint8_t *__restrict__ dataPtr,
                                     uint64_t count) {

  uint64_t keybytes = count / 4; // number of key bytes

  uint8_t *keyEnd = (size_t)keyPtr + keybytes;

  uint8_t *keyPtr8 = (uint8_t *)keyPtr;

  if (keybytes >= 1) {
    while (keyPtr8 < keyEnd) {
      _decode_sse41_opt(&keyPtr8, &dataPtr, &out);
    }
  }
  return dataPtr;
}
STREAMVBYTE_UNTARGET_REGION
#endif
