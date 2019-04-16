#include "../Terrain/WorldLoader.h"

void onDefaultBlockUpdate(BlockPos &) {

}
void onDefaultBlockTick(BlockPos &) {

}
void onDefaultBlockInteract(BlockPos &, Entity *, int, void *) {

}
void onDefaultBlockLook(BlockPos &, Entity *, int, void *) {

}
void onDefaultBlockBreak(BlockPos &, Entity *, int, void *) {

}

void getStoredPhysics(BlockPos b[7], list<PhysCube>& l) {

  fVec3 location = mVec3(b[6].lbx + b[6].c->_cx * BLOCK_PER_CHUNK, b[6].lby + b[6].c->_cy * BLOCK_PER_CHUNK, b[6].lbz + b[6].c->_cz * BLOCK_PER_CHUNK);

  for (auto&& it : blockModels[b[6].b->_ID].cubes) {
    PhysCube newC = it;
    newC.nc += location;
    newC.pc += location;
    l.push_back(newC);
  }
}

#ifdef M_CLIENT
BlockNeeds getEmptyNeeds(Block&) {
  return 127;
}
BlockNeeds getSolidNeeds(Block&) {
  return 0;
}
BlockNeeds getSlabNeeds(Block&) {
  return 95; //Not top faces
}

void getStoredModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo) {
  BlockProperies& prop = blockProperties[b[6].b->_ID];
  fVec3 location = {
    b[6].c->_cx * BLOCK_PER_CHUNK + b[6].lbx * 1.0f,
    b[6].c->_cy * BLOCK_PER_CHUNK + b[6].lby * 1.0f,
    b[6].c->_cz * BLOCK_PER_CHUNK + b[6].lbz * 1.0f };

  for (auto&& it : blockModels[b[6].b->_ID].faces) {
    if (it.type & n) {
      QuadFace res;

      res.vbl = it.base.vbl + location;
      res.vbr = it.base.vbr + location;
      res.vtl = it.base.vtl + location;
      res.vtr = it.base.vtr + location;

      vec2<float> texCoord = prop.getTex(b[6], it.faceID);
      res.tbl = (it.base.tbl + texCoord) *TEXTURE_SIZE;
      res.tbr = (it.base.tbr + texCoord) *TEXTURE_SIZE;
      res.ttl = (it.base.ttl + texCoord) *TEXTURE_SIZE;
      res.ttr = (it.base.ttr + texCoord) *TEXTURE_SIZE;

      addTo.push_back(res);
    }
  }
}

void addTreeBranchModels(list<QuadFace>& addTo, fVec3 cent, float size, int direction, iVec2 texCoord, iVec2 midTexCoord, iVec2 endTexCoord, iVec2 leafTexCoord, bool log) {
  fVec3 depth;
  fVec3 width;
  fVec3 height;
  fVec3 widthF;
  fVec3 heightF;
  
  if (direction / 2 == 0) {
    depth.x = -0.5;
    width.y = size;
    height.z = size;
    widthF.y = 0.5;
    heightF.z = 0.5;
  }
  if (direction / 2 == 1) {
    depth.y = -0.5;
    width.z = size;
    height.x = size;
    widthF.z = 0.5;
    heightF.x = 0.5;
  }
  if (direction / 2 == 2) {
    depth.z = -0.5;
    width.x = size;
    height.y = size;
    widthF.x = 0.5;
    heightF.y = 0.5;
  }
  if (direction % 2) {
    depth *= -1;
    width *= -1;
    height *= -1;
  }

  QuadFace res;

  //if(log) {
    res.tbl = (fVec2(0.5f - size, 0.0f) + texCoord) *TEXTURE_SIZE;
    res.ttl = (fVec2(0.5f - size, 0.5f) + texCoord) *TEXTURE_SIZE;
    res.ttr = (fVec2(0.5f + size, 0.5f) + texCoord) *TEXTURE_SIZE;
    res.tbr = (fVec2(0.5f + size, 0.0f) + texCoord) *TEXTURE_SIZE;

    //Walls
    res.vbl = cent + height + width;
    res.vtl = cent + height + width + depth;
    res.vtr = cent + height - width + depth;
    res.vbr = cent + height - width;
    addTo.push_back(res);

    res.vbl = cent - height + width;
    res.vtl = cent - height + width + depth;
    res.vtr = cent - height - width + depth;
    res.vbr = cent - height - width;
    addTo.push_back(res);

    res.vbl = cent + width + height;
    res.vtl = cent + width + height + depth;
    res.vtr = cent + width - height + depth;
    res.vbr = cent + width - height;
    addTo.push_back(res);

    res.vbl = cent - width + height;
    res.vtl = cent - width + height + depth;
    res.vtr = cent - width - height + depth;
    res.vbr = cent - width - height;
    addTo.push_back(res);

    //Inner end
    res.tbl = (fVec2(0.5f - size, 0.5f - size) + endTexCoord) *TEXTURE_SIZE;
    res.ttl = (fVec2(0.5f - size, 0.5f + size) + endTexCoord) *TEXTURE_SIZE;
    res.ttr = (fVec2(0.5f + size, 0.5f + size) + endTexCoord) *TEXTURE_SIZE;
    res.tbr = (fVec2(0.5f + size, 0.5f - size) + endTexCoord) *TEXTURE_SIZE;

    res.vbl = cent - width + height;
    res.vtl = cent + width + height;
    res.vtr = cent + width - height;
    res.vbr = cent - width - height;
    addTo.push_back(res);

    //OpenEnd
    res.tbl = (fVec2(0.5f - size, 0.5f - size) + midTexCoord) *TEXTURE_SIZE;
    res.ttl = (fVec2(0.5f - size, 0.5f + size) + midTexCoord) *TEXTURE_SIZE;
    res.ttr = (fVec2(0.5f + size, 0.5f + size) + midTexCoord) *TEXTURE_SIZE;
    res.tbr = (fVec2(0.5f + size, 0.5f - size) + midTexCoord) *TEXTURE_SIZE;

    res.vbl = cent - width + height + depth;
    res.vtl = cent + width + height + depth;
    res.vtr = cent + width - height + depth;
    res.vbr = cent - width - height + depth;
    addTo.push_back(res);
  //} else {
    //Leaf
    if(size < 0.5) {
      res.tbl = (fVec2(0.0f, 0.0f) + leafTexCoord) *TEXTURE_SIZE;
      res.ttl = (fVec2(0.0f, 1.0f) + leafTexCoord) *TEXTURE_SIZE;
      res.ttr = (fVec2(1.0f, 1.0f) + leafTexCoord) *TEXTURE_SIZE;
      res.tbr = (fVec2(1.0f, 0.0f) + leafTexCoord) *TEXTURE_SIZE;

      res.vbl = cent - widthF + heightF + depth * 0.75;
      res.vtl = cent + widthF + heightF + depth * 0.75;
      res.vtr = cent + widthF - heightF + depth * 0.75;
      res.vbr = cent - widthF - heightF + depth * 0.75;
      addTo.push_back(res);
    }
  //}
}

void getWoodModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo) {
  BlockProperies& prop = blockProperties[b[6].b->_ID];

  int core = 0;
  for (int i = 0; i < 12; i+=2) {
    core = max((b[6].b->_meta >> i) & 3, core);
  }

  fVec3 location = {
    b[6].c->_cx * BLOCK_PER_CHUNK + b[6].lbx * 1.0f + 0.5f,
    b[6].c->_cy * BLOCK_PER_CHUNK + b[6].lby * 1.0f + 0.5f,
    b[6].c->_cz * BLOCK_PER_CHUNK + b[6].lbz * 1.0f + 0.5f };

  for (int i = 0; i < 6; i++) {
    addTreeBranchModels(addTo, location, ((1 << ((b[6].b->_meta >> (2 * i)) & 3)) & 14) / 16.0, i, prop.getTex(b[6], 0), prop.getTex(b[6], 3), prop.getTex(b[6], 4), prop.getTex(b[6], 5), core == 3);
  }
}

vec2<float> getStoredTex(BlockPos& b, uint32_t texID) {
  return{ 0.0f, 1.0f*blockProperties[b.b->_ID].textures[texID] };
}
#endif

void actionDestoryNothing(BlockPos &) {

}

void onUpdateNothing(BlockPos &) {

}

void onUpdateNature(BlockPos &p) {
  BlockPos np;
  if (blockNeighbour(p, Dir_MZ, np)) {
    if(np.b->_ID != 3) { //Destory if not on grass
      p.c->setBlock(p.lbx, p.lby, p.lbz, Block(0));
    }
  }
}
