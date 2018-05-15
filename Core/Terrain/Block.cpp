#include "WorldLoader.h"

Block::Block() {
}

Block::Block(block_id_t ID) :
  _ID(ID) {
}

Block::Block(block_id_t ID, meta_id_t meta) :
  _ID(ID),
  _meta(meta) {
}

vector<BlockProperies> blockProperties;

void writeBlock(unsigned char * to, int id, Block & b) {
  to[id] = b._ID >> 0;
  to[id + 1] = b._ID >> 8;
  to[id + 2] = b._meta >> 0;
  to[id + 3] = b._meta >> 8;
  to[id + 4] = b._meta >> 16;
  to[id + 5] = b._meta >> 24;
}

void readBlock(unsigned char * from, int id, Block & b) {
  b._ID = 1 * from[id] + 256 * from[id + 1];
  b._meta = 1 * from[id + 2] + 256 * from[id + 3] + 65536 * from[id + 4] + 16777216 * from[id + 5];
}

#ifdef M_CLIENT
BlockModel empty_model;
BlockModel solid_model;

void getEmptyModel(BlockPos& b, BlockNeeds n, list<QuadFace>& addTo) {
}
BlockNeeds getEmptyNeeds(Block&) {
  return 127;
}
vec2<float> getEmptyTex(BlockPos& b, uint32_t texID) {
  return {0,0};
}

void getSolidModel(BlockPos& b, BlockNeeds n, list<QuadFace>& addTo) {
  BlockProperies& prop = blockProperties[b.b._ID];


  for (auto&& it : solid_model.faces) {
    if (it.type & n) {
      QuadFace res;

      fVec3 location = {
      b.c->_cx * BLOCK_PER_CHUNK + b.lbx * 1.0f,
      b.c->_cy * BLOCK_PER_CHUNK + b.lby * 1.0f,
      b.c->_cz * BLOCK_PER_CHUNK + b.lbz * 1.0f};
      res.vbl = it.base.vbl + location;
      res.vbr = it.base.vbr + location;
      res.vtl = it.base.vtl + location;
      res.vtr = it.base.vtr + location;

      vec2<float> texCoord = prop.getTex(b, it.texId);
      res.tbl = (it.base.tbl + texCoord) * vec2<float>{1.0f, 0.25f} * TEXTURE_SIZE;
      res.tbr = (it.base.tbr + texCoord) * vec2<float>{1.0f, 0.25f}  * TEXTURE_SIZE;
      res.ttl = (it.base.ttl + texCoord) * vec2<float>{1.0f, 0.25f}  * TEXTURE_SIZE;
      res.ttr = (it.base.ttr + texCoord) * vec2<float>{1.0f, 0.25f}  * TEXTURE_SIZE;

      res.recolor = it.base.recolor;

      addTo.push_back(res);
    }
  }
}
BlockNeeds getSolidNeeds(Block&) {
  return 0;
}
vec2<float> getSolidTex(BlockPos& b, uint32_t texID) {
  return{ 0,texID*1.0f };
}

void loadBlocks() {
  solid_model.faces.push_back({ { { 0,1,0 },{ 0,1,1 },{ 0,0,1 },{ 0,0,0 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 1,0,0,1 }}, 1 << Dir_MX, 1});
  solid_model.faces.push_back({ { { 1,0,0 },{ 1,0,1 },{ 1,1,1 },{ 1,1,0 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 1,0,1,1 }}, 1 << Dir_PX, 1});
  solid_model.faces.push_back({ { { 0,0,0 },{ 0,0,1 },{ 1,0,1 },{ 1,0,0 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 0,1,0,1 }}, 1 << Dir_MY, 1});
  solid_model.faces.push_back({ { { 1,1,0 },{ 1,1,1 },{ 0,1,1 },{ 0,1,0 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 0,1,1,1 }}, 1 << Dir_PY, 1});
  solid_model.faces.push_back({ { { 1,1,0 },{ 1,0,0 },{ 0,0,0 },{ 0,1,0 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 1,1,0,1 }}, 1 << Dir_MZ, 2});
  solid_model.faces.push_back({ { { 0,0,1 },{ 0,1,1 },{ 1,1,1 },{ 1,0,1 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 1,1,1,1 }}, 1 << Dir_PZ, 0});
  BlockProperies prop_air;
  prop_air.getModel = getEmptyModel;
  prop_air.getNeeds = getEmptyNeeds;
  BlockProperies prop_sol;
  prop_sol.getModel = getSolidModel;
  prop_sol.getNeeds = getSolidNeeds;
  prop_sol.getTex = getSolidTex;
  blockProperties = {prop_air, prop_sol};
}
#endif