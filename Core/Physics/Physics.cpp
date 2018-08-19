#include "../Terrain/WorldLoader.h"

struct movement {
  mVec3 pos;
  mpsVec3 vel;
};

template<typename T>
void sortPairInc(T& a, T& b) {
  if (a > b) {
    swap(a, b);
  }
}

template<typename T>
void sortPairDec(T& a, T& b) {
  if (a > b) {
    swap(a, b);
  }
}

/*pair<time_type_s, int> getIntersectTime(PhysCube& lhs, PhysCube& rhs, mpsVec3 relVel) {
  vec3<time_type_s> intersectA = (rhs.nc - lhs.pc) / relVel;
  vec3<time_type_s> intersectB = (rhs.pc - lhs.nc) / relVel;

  vec3<time_type_s> intersectStart = vecSwitch(intersectA > intersectB, intersectA, intersectB);
  vec3<time_type_s> intersectEnd = vecSwitch(intersectA < intersectB, intersectA, intersectB);

  time_type_s intersectS = intersectStart.x;
  int lastAxis = 0;
  if(intersectS < intersectStart.y) {
    intersectS = intersectStart.y;
    lastAxis = 1;
  }
  if (intersectS < intersectStart.z) {
    intersectS = intersectStart.z;
    lastAxis = 2;
  }

  time_type_s intersectE = intersectEnd.min();

  if (intersectE > intersectS) {
    return { INFINITY, lastAxis };
  }
  return{ intersectS, lastAxis };
}*/

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

void tickEntity(Entity* ent, time_type_s deltaT) {
  time_type_s stepLength = deltaT;

  mVec3 newPos = ent->getFeetCenter();

  PhysCube entc = ent->getPhysCube();
  PhysCube endc = entc;

  mpsVec3 vel = ent->getVelocity();
  getBlock(newPos.x, newPos.y, newPos.z, ent->getDim(), ent->_inWorld);
  vel += (ent->_selfAccel * sVec3(max<double>(ent->_friction[2], 0.3), max<double>(ent->_friction[2], 0.3), ent->_friction[2])  + G) * deltaT;

  mVec3 offset = vel * deltaT;

  endc.offset(offset);

  PhysCube boundry = getUnion(entc, endc);

  iVec3 iBegBot = { int(floor(boundry.nc.x)),int(floor(boundry.nc.y)) ,int(floor(boundry.nc.z)) };
  iVec3 iBegTop = { int(floor(boundry.pc.x)),int(floor(boundry.pc.y)) ,int(floor(boundry.pc.z)) };
  mass_type_kg enclosedMass = 0;
  float drag = 1; //no restrict

  list<PhysCube> cubes;
  //cout << "ET";
  for (int i = iBegBot.x; i <= iBegTop.x; i++) {
    for (int j = iBegBot.y; j <= iBegTop.y; j++) {
      for (int k = iBegBot.z; k <= iBegTop.z; k++) {
        bool s = false;
        BlockPos b[7] = {NULL,NULL, NULL, NULL, NULL, NULL, NULL};
        //cout << "S";
        b[6] = getBlock(i, j, k, ent->getDim(), s);
        //cout << "F";
        if (s) {
          for (int l = 0; l < 6; l++) {
            blockNeighbour(b[6], static_cast<Directions>(l), b[l]);
          }

          blockProperties[b[6].b->_ID].getPhysics(b, cubes);
        }
      }
    }
  }

  /*volume_type_mmm fullVol = getVol(entc);

  for(auto&& it : cubes) {
    volume_type_mmm interv = getIntersectVol(entc, it);
    if(interv) {
      enclosedMass += it.density * interv;

      drag *= pow(1-it.drag, interv / fullVol); //if even one object has 1 drag, output = 0.
    }
  }
  return {G * (1 - enclosedMass / ent->getMass()), drag};*/

  /*vec3<time_type_s> colls = {INFINITY, INFINITY ,INFINITY };
  for(auto&& it : cubes) {
    pair<time_type_s, int> coll = getIntersectTime(entc, it, vel);
  }*/

  //vec3<time_type_s> movTime = vec3<time_type_s>(deltaT);
  //mpsVec3 nVel = vel;

  /*for (auto&& it : cubes) {
    if (getIntersectVol(it, endc) > 0) {
      vec3<time_type_s> nhitTime = vecSwitch(
        bVec3(getIntersectVol(it, endcx) > 0, getIntersectVol(it, endcy) > 0, getIntersectVol(it, endcz) > 0),
        vecSwitch(
          vel < mpsVec3(0), (it.pc - entc.nc) / vel,
          vecSwitch(
            vel > mpsVec3(0),
            (it.nc - entc.pc) / vel,
            vec3<time_type_s>(-100000))),
          vec3<time_type_s>(-0.0));
      int lastHit = 0;
      time_type_s hitTime = nhitTime.x;
      if (hitTime < nhitTime.y) {
        hitTime = nhitTime.y;
        lastHit = 1;
      }
      if (hitTime < nhitTime.z) {
        hitTime = nhitTime.z;
        lastHit = 2;
      }

      cout << hitTime << endl;

      if (hitTime < movTime[lastHit]) {
        movTime[lastHit] = hitTime;
        nVel[lastHit] = 0;
        cout << lastHit << endl;
      }
    }
  }*/

  /*
  for (auto&& it : cubes) {
    if(vel.x != 0) {
      time_type_s newHit = (vel.x < 0) ? (it.pc.x - entc.nc.x) / vel.x : (it.nc.x - entc.pc.x) / vel.x;
      if (newHit < movTime.x) {
        movTime.x = newHit;
        nVel.x = 0;
      }
    }
    if (vel.y != 0) {
      time_type_s newHit = (vel.y < 0) ? (it.pc.y - entc.nc.y) / vel.y : (it.nc.y - entc.pc.y) / vel.y;
      if (newHit < movTime.y) {
        movTime.y = newHit;
        nVel.y = 0;
      }
    }
    if (vel.z != 0) {
      time_type_s newHit = (vel.z < 0) ? (it.pc.z - entc.nc.z) / vel.z : (it.nc.z - entc.pc.z) / vel.z;
      if (newHit < movTime.z) {
        movTime.z = newHit;
        nVel.z = 0;
      }
    }
  }
  */

  ent->_friction = sVec3(0);
  mpsVec3 newVel = vel;

  if (cubes.size() == 0 && !ent->_inWorld) {
    ent->_friction = sVec3(1);
  }

  while (deltaT > 0 && bor(newVel!=mpsVec3(0))) {
    int blockAxis = -1;
    time_type_s collideAt = deltaT;
    for (auto&& it : cubes) {
      vec3<time_type_s> enterTimes;
      vec3<time_type_s> exitTimes;
      for(int ax = 0; ax < 3; ax++) {
        if (newVel[ax] == 0) {
          if (max(entc.nc[ax], it.nc[ax]) < min(entc.pc[ax], it.pc[ax])) {
            enterTimes[ax] = -1e100;
            exitTimes[ax] = deltaT;
          } else {
            break;
          }
        } else {
          if (newVel[ax] < 0) {
            enterTimes[ax] = (it.pc[ax] - entc.nc[ax]) / newVel[ax];
            exitTimes[ax] = (it.nc[ax] - entc.pc[ax]) / newVel[ax];
          } else {
            enterTimes[ax] = (it.nc[ax] - entc.pc[ax]) / newVel[ax];
            exitTimes[ax] = (it.pc[ax] - entc.nc[ax]) / newVel[ax];
          }
        }
      }
      if (-0.001 < enterTimes.maxV() && enterTimes.maxV() < exitTimes.minV()) {
        time_type_s lastEnter = enterTimes.x;
        int lastEnterDir = 0;
        for (int i = 1; i < 3; i++) {
          if (lastEnter < enterTimes[i]) {
            lastEnter = enterTimes[i];
            lastEnterDir = i;
          }
        }

        if (lastEnter < collideAt) {
          collideAt = lastEnter;
          blockAxis = lastEnterDir;
        }
        
      }
    }
    newPos += newVel * collideAt;
    entc.offset(newVel * collideAt);
    if (blockAxis != -1) {
      newVel[blockAxis] = 0;
      ent->_friction[blockAxis] = 1;
    }
    deltaT -= collideAt;
  }

  if (ent->_friction[2] > 0) {
    newVel *= {pow(E, - 20 * stepLength * abs(newVel.x)), pow(E, -20 * stepLength * abs(newVel.y)), pow(E, -0 * stepLength * abs(newVel.z))};
    newVel = vecSwitch(newVel < mpsVec3(0.1) & newVel > mpsVec3(-0.1) & bVec3(true, true, false), mpsVec3(0), newVel);
  } else {
    newVel *= {pow(E, - 5 * stepLength * abs(newVel.x)), pow(E, -5 * stepLength * abs(newVel.y)), pow(E, -0.01 * stepLength * abs(newVel.z))};
  }

  ent->setPos(newPos);
  ent->setVel(newVel);
}

void tickPhysics(time_type_s tickTime) {
  for (auto&& it : entities) {
    /*mVec3 posShift = {0,0,0};
    mpsVec3 vel = it->getVelocity();
    pair<mpssVec3, float> props = getProps(it, posShift);
    vel *= exp(-tickTime/props.second);
    posShift = vel * tickTime;*/
    //it.second->movPos(it.second->getVelocity() * tickTime);
    tickEntity(it.second, tickTime);
  }
}