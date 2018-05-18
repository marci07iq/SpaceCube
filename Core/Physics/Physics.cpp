#include "../Terrain/WorldLoader.h"

struct movement {
  mVec3 pos;
  mpsVec3 vel;
};

PhysCube getUnion(PhysCube& lhs, PhysCube& rhs) {
  PhysCube res;
  res.nc = vecSwitch(lhs.nc < rhs.nc, lhs.nc, rhs.nc);
  res.pc = vecSwitch(lhs.pc > rhs.pc, lhs.pc, rhs.pc);
  return res;
}

volume_type_mmm getIntersectVol(PhysCube& lhs, PhysCube& rhs) {
  distance_type_m xIntersect = max<distance_type_m>(0, min(lhs.pc.x, rhs.pc.x) - max(lhs.nc.x, rhs.nc.x));
  distance_type_m yIntersect = max<distance_type_m>(0, min(lhs.pc.y, rhs.pc.y) - max(lhs.nc.y, rhs.nc.y));
  distance_type_m zIntersect = max<distance_type_m>(0, min(lhs.pc.z, rhs.pc.z) - max(lhs.nc.z, rhs.nc.z));
  return xIntersect * yIntersect * zIntersect;
}

volume_type_mmm getVol(PhysCube& c) {
  return (c.pc.x-c.nc.x) * (c.pc.y - c.nc.y) * (c.pc.z - c.nc.z);
}

pair<mpssVec3, float> tickEntity(Entity* ent, mVec3 shift) {
  PhysCube entc = ent->getPhysCube();
  entc.nc += shift;
  entc.pc += shift;
  iVec3 iBegBot = iVec3(floor(entc.nc.x), floor(entc.nc.y), floor(entc.nc.z));
  iVec3 iBegTop = iVec3(floor(entc.pc.x), floor(entc.pc.y), floor(entc.pc.z));

  mass_type_kg enclosedMass = 0;
  float drag = 1; //no restrict
  
  list<PhysCube> cubes;

  for (int i = iBegBot.x; i <= iBegTop.x; i++) {
    for (int j = iBegBot.y; j <= iBegTop.y; j++) {
      for (int k = iBegBot.z; k <= iBegTop.z; k++) {
        bool s = false;
        BlockPos b[7];
        b[6] = getBlock(i, j, k, ent->getDim(), s);
        if (s) {
          for (int l = 0; l < 6; l++) {
            blockNeighbour(b[6], static_cast<Directions>(l), b[l]);
          }

          blockProperties[b[6].b->_ID].getPhysics(b, cubes);
        }
      }
    }
  }

  volume_type_mmm fullVol = getVol(entc);

  for(auto&& it : cubes) {
    volume_type_mmm interv = getIntersectVol(entc, it);
    if(interv) {
      enclosedMass += it.density * interv;

      drag *= pow(1-it.drag, interv / fullVol); //if even one object has 1 drag, output = 0.
    }
  }
  return {G * (1 - enclosedMass / ent->getMass()), drag};
}

void tickPhysics(time_type_s tickTime) {
  for (auto&& it : entities) {
    /*mVec3 posShift = {0,0,0};
    mpsVec3 vel = it->getVelocity();
    pair<mpssVec3, float> props = getProps(it, posShift);
    vel *= exp(-tickTime/props.second);
    posShift = vel * tickTime;*/
    it->movPos(it->getVelocity() * tickTime);
  }
}
