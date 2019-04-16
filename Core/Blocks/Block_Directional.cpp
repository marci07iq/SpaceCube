#include "../Terrain/WorldLoader.h"

//Around axis (0,0,1), (0.5 0.5 0), right hand rule: CCW
//+x +z -x -z
fVec3 rotateVec(fVec3 what, int with) {
  if (with & 2) what.x = 1-what.x;
  if (with & 3 == 1 || with & 3 == 2) what.y = 1-what.y;
  if (with & 1) {
    swap(what.x, what.y);
  }
  return what;
}

void getRotatedPhysics(BlockPos b[7], list<PhysCube>& l) {

  fVec3 location = mVec3(b[6].lbx + b[6].c->_cx * BLOCK_PER_CHUNK, b[6].lby + b[6].c->_cy * BLOCK_PER_CHUNK, b[6].lbz + b[6].c->_cz * BLOCK_PER_CHUNK);
  int rot = b[6].b->_meta & 3;

  for (auto&& it : blockModels[b[6].b->_ID].cubes) {
    PhysCube newC = it;
    newC.nc = rotateVec(newC.nc, rot) + location;
    newC.pc = rotateVec(newC.pc, rot) + location;
    l.push_back(newC);
  }
}

#ifdef M_CLIENT

BlockNeeds getStairNeeds(Block&) {

}

void getRotatedModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo) {
  BlockProperies& prop = blockProperties[b[6].b->_ID];

  for (auto&& it : blockModels[b[6].b->_ID].faces) {
    fVec3 location = {
      b[6].c->_cx * BLOCK_PER_CHUNK + b[6].lbx * 1.0f,
      b[6].c->_cy * BLOCK_PER_CHUNK + b[6].lby * 1.0f,
      b[6].c->_cz * BLOCK_PER_CHUNK + b[6].lbz * 1.0f };

    int rot = b[6].b->_meta & 3;

    if (it.type & n) {
      QuadFace res;
      res.vbl = rotateVec(it.base.vbl, rot) + location;
      res.vbr = rotateVec(it.base.vbr, rot) + location;
      res.vtl = rotateVec(it.base.vtl, rot) + location;
      res.vtr = rotateVec(it.base.vtr, rot) + location;

      vec2<float> texCoord = prop.getTex(b[6], it.faceID);
      res.tbl = (it.base.tbl + texCoord) * TEXTURE_SIZE;
      res.tbr = (it.base.tbr + texCoord) * TEXTURE_SIZE;
      res.ttl = (it.base.ttl + texCoord) * TEXTURE_SIZE;
      res.ttr = (it.base.ttr + texCoord) * TEXTURE_SIZE;

      addTo.push_back(res);
    }
  }
}

#endif