#include "streamvbyte_isadetection.h"
#ifdef STREAMVBYTE_X64
STREAMVBYTE_TARGET_SSE41
static inline __m128i _decode_sse41_opt(uint32_t key,
                                  const uint8_t *__restrict__ *dataPtrPtr) {
  uint8_t len;
  __m128i Data = _mm_loadu_si128((__m128i *)*dataPtrPtr);
  uint8_t *pshuf = (uint8_t *) &shuffleTable[key];
  __m128i Shuf = *(__m128i *)pshuf;
#ifdef AVOIDLENGTHLOOKUP
  // this avoids the dependency on lengthTable,
  // see https://github.com/lemire/streamvbyte/issues/12
  len = pshuf[12 + (key >> 6)] + 1;
#else
  len = lengthTable[key];
#endif
  Data = _mm_shuffle_epi8(Data, Shuf);
  *dataPtrPtr += len;
  return Data;
}
STREAMVBYTE_UNTARGET_REGION


STREAMVBYTE_TARGET_SSE41
static inline void _write_sse41_opt(uint32_t *out, __m128i Vec) {
  _mm_storeu_si128((__m128i *)out, Vec);
}
STREAMVBYTE_UNTARGET_REGION



STREAMVBYTE_TARGET_SSE41
const uint8_t *svb_decode_sse41_simple_opt(uint32_t *out,
                                     const uint8_t *__restrict__ keyPtr,
                                     const uint8_t *__restrict__ dataPtr,
                                     uint64_t count) {

  uint64_t keybytes = count / 4; // number of key bytes

  const uint8_t *keyPtr8 = (const uint8_t *)keyPtr;

  __m128i Data;
  if (keybytes >= 1) {
    for (; keybytes > 0; keybytes--) {
      Data = _decode_sse41_opt(*keyPtr8, &dataPtr);
      _write_sse41_opt(out, Data);
      keyPtr8++;
      out += 4;
    }
  }
  return dataPtr;
}
STREAMVBYTE_UNTARGET_REGION
#endif
