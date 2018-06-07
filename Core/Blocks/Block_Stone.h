#pragma once

#include "../Terrain/Block.h"

inline uint16_t getSize(uint32_t meta);

inline uint16_t getMaterial(uint32_t meta);

inline uint16_t getMineral(uint32_t meta);

inline uint16_t getMineralQuantity(uint32_t meta);

inline uint16_t getWater(uint32_t meta);

inline uint16_t getFoilage(uint32_t meta);

#ifdef M_CLIENT
#endif