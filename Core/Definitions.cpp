#include "Definitions.h"

guid_t randomE_GUID() {
  guid_t res = 0;
  for (int i = 0; i < (GUID_L + 7) / 8; i++) {
    res = (res * 256) + ran1() * 256;
  }
  return res & 0xfffffffffffffffe; //Set last bit 0
}
