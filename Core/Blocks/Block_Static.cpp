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
void getStoredBlockPhysics(BlockPos[7], list<PhysCube>& l) {

}

#ifdef M_CLIENT
BlockNeeds getEmptyNeeds(Block&) {
  return 127;
}
BlockNeeds getSolidNeeds(Block&) {
  return 0;
}

void getStoredModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo) {
  BlockProperies& prop = blockProperties[b[6].b->_ID]._ptr->_val;

  for (auto&& it : blockModels[b[6].b->_ID]._ptr->_val.faces) {
    if (it.type & n) {
      QuadFace res;

      fVec3 location = {
        b[6].c->_cx * BLOCK_PER_CHUNK + b[6].lbx * 1.0f,
        b[6].c->_cy * BLOCK_PER_CHUNK + b[6].lby * 1.0f,
        b[6].c->_cz * BLOCK_PER_CHUNK + b[6].lbz * 1.0f };
      res.vbl = it.base.vbl + location;
      res.vbr = it.base.vbr + location;
      res.vtl = it.base.vtl + location;
      res.vtr = it.base.vtr + location;

      vec2<float> texCoord = prop.getTex(b[6], it.faceID);
      res.tbl = (it.base.tbl + texCoord) *TEXTURE_SIZE;
      res.tbr = (it.base.tbr + texCoord) *TEXTURE_SIZE;
      res.ttl = (it.base.ttl + texCoord) *TEXTURE_SIZE;
      res.ttr = (it.base.ttr + texCoord) *TEXTURE_SIZE;

      res.rbl = 0;
      res.rbr = 0;
      res.rtl = 0;
      res.rtr = 0;

      res.recolor = it.base.recolor;

      addTo.push_back(res);
    }
  }
}

vec2<float> getStoredTex(BlockPos& b, uint32_t texID) {
  return{ 0.0f, 1.0f*blockProperties[b.b->_ID]._ptr->_val.textures[texID] };
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
