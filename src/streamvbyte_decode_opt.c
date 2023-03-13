#include "streamvbyte.h"
#include "streamvbyte_isadetection.h"

#include <string.h> // for memcpy
#include "streamvbyte_shuffle_tables_decode.h"
#include "streamvbyte_x64_decode_opt.c"

static inline uint32_t _decode_data_opt(const uint8_t **dataPtrPtr, uint8_t code)
{
  const uint8_t *dataPtr = *dataPtrPtr;
  uint32_t val;

  if (code == 0)
  { // 1 byte
    val = (uint32_t)*dataPtr;
    dataPtr += 1;
  }
  else if (code == 1)
  { // 2 bytes
    val = 0;
    memcpy(&val, dataPtr, 2); // assumes little endian
    dataPtr += 2;
  }
  else if (code == 2)
  { // 3 bytes
    val = 0;
    memcpy(&val, dataPtr, 3); // assumes little endian
    dataPtr += 3;
  }
  else
  { // code == 3
    memcpy(&val, dataPtr, 4);
    dataPtr += 4;
  }

  *dataPtrPtr = dataPtr;
  return val;
}
static const uint8_t *svb_decode_scalar_opt(uint32_t *outPtr, const uint8_t *keyPtr,
                                            const uint8_t *dataPtr,
                                            uint32_t count)
{
  if (count == 0)
    return dataPtr; // no reads or writes if no data

  uint8_t shift = 0;
  uint32_t key = *keyPtr++;
  for (uint32_t c = 0; c < count; c++)
  {
    if (shift == 8)
    {
      shift = 0;
      key = *keyPtr++;
    }
    uint32_t val = _decode_data_opt(&dataPtr, (key >> shift) & 0x3);
    *outPtr++ = val;
    shift += 2;
  }

  return dataPtr; // pointer to first unused byte after end
}

// Read count 32-bit integers in maskedvbyte format from in, storing the result
// in out.  Returns the number of bytes read.
size_t streamvbyte_decode_opt(const uint8_t *in, uint32_t *out, uint32_t count)
{
  if (count == 0)
    return 0;

  const uint8_t *keyPtr = in;               // full list of keys is next
  uint32_t keyLen = ((count + 3) / 4);      // 2-bits per key (rounded up)
  const uint8_t *dataPtr = keyPtr + keyLen; // data starts at end of keys

  if(streamvbyte_sse41()) {
    dataPtr = svb_decode_sse41_simple_opt(out, keyPtr, dataPtr, count);
    out += count & ~ 31;
    keyPtr += (count/4) & ~ 7;
    count &= 31;
  }

  return svb_decode_scalar_opt(out, keyPtr, dataPtr, count) - in;
}