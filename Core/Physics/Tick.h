#pragma once

#include "../Terrain/WorldLoader.h"

//Physics, entities
void subTick(uint64_t id);

//Blocks
void tick(uint64_t id);

void doTicks(uint64_t subTickTillIncl);