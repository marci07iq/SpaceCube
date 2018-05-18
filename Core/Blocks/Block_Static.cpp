#include "../Terrain/WorldLoader.h"

#ifdef M_CLIENT
BlockNeeds getEmptyNeeds(Block&) {
  return 127;
}
BlockNeeds getSolidNeeds(Block&) {
  return 0;
}

void getStoredModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo) {
  BlockProperies& prop = blockProperties[b[6].b->_ID];

  for (auto&& it : blockModels[b[6].b->_ID].faces) {
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

      res.recolor = it.base.recolor;

      addTo.push_back(res);
    }
  }
}

vec2<float> getStoredTex(BlockPos& b, uint32_t texID) {
  return{ 0.0f, 1.0f*blockProperties[b.b->_ID].textures[texID] };
}
#endif