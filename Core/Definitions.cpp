#include "Definitions.h"

guid_t randomE_GUID() {
  guid_t res = 0;
  for (int i = 0; i < (GUID_L + 7) / 8; i++) {
    res = (res * 256) + ran1() * 256;
  }
  return res & 0xfffffffffffffffe; //Set last bit 0
}

iVec3 jump(iVec3 in, int towards) {
  switch (towards) {
    case Dir_MX:
      return{ in.x - 1, in.y, in.z };
      break;
    case Dir_PX:
      return{ in.x + 1, in.y, in.z };
      break;
    case Dir_MY:
      return{ in.x, in.y - 1, in.z };
      break;
    case Dir_PY:
      return{ in.x, in.y + 1, in.z };
      break;
    case Dir_MZ:
      return{ in.x, in.y, in.z - 1 };
      break;
    case Dir_PZ:
      return{ in.x, in.y, in.z + 1 };
      break;
  }
  return in;
}

string mapname = "test";
