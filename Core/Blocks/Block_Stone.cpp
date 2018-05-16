#include "../Terrain/WorldLoader.h"

inline uint16_t getSize(uint32_t meta) {
  return meta & 0x0000000f;
}

inline uint16_t getMaterial(uint32_t meta) {
  return (meta >> 4) & 0x000003ff;
}

inline uint16_t getMineral(uint32_t meta) {
  return (meta >> 14) & 0x000003ff;
}

inline uint16_t getMineralQuantity(uint32_t meta) {
  return (meta >> 24) & 0x0000000f;
}

inline uint16_t getWater(uint32_t meta) {
  return (meta >> 28) & 0x00000003;
}

inline uint16_t getFoilage(uint32_t meta) {
  return (meta >> 30) & 0x00000003;
}

#ifdef M_CLIENT
#endif