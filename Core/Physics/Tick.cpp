#include "Tick.h"

uint64_t nextSubTick = 0;

//Physics, entities
void subTick(uint64_t id) {
  subtickPhysics();
}

//Blocks
void tick(uint64_t id) {

}

void doTicks(uint64_t subTickTillIncl) {
  while (nextSubTick < subTickTillIncl) {
    subTick(nextSubTick);
    //if (nextSubTick % SC_SUBTICK_PER_TICK == 0) {
    tick(nextSubTick / SC_SUBTICK_PER_TICK);
    //}
    ++nextSubTick;
  }
}
