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

BlockModel& getEmptyModel(Block&) {
  return empty_model;
}
BlockNeeds getEmptyNeeds(Block&) {
  return 127;
}

BlockModel& getSolidModel(Block&) {
  return solid_model;
}
BlockNeeds getSolidNeeds(Block&) {
  return 0;
}

void loadBlocks() {
  solid_model.faces.push_back({ { 0,1,0 },{ 0,1,1 },{ 0,0,1 },{ 0,0,0 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 1,0,0,1 }, 1 << Dir_MX });
  solid_model.faces.push_back({ { 1,0,0 },{ 1,0,1 },{ 1,1,1 },{ 1,1,0 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 1,0,1,1 }, 1 << Dir_PX });
  solid_model.faces.push_back({ { 0,0,0 },{ 0,0,1 },{ 1,0,1 },{ 1,0,0 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 0,1,0,1 }, 1 << Dir_MY });
  solid_model.faces.push_back({ { 1,1,0 },{ 1,1,1 },{ 0,1,1 },{ 0,1,0 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 0,1,1,1 }, 1 << Dir_PY });
  solid_model.faces.push_back({ { 1,1,0 },{ 1,0,0 },{ 0,0,0 },{ 0,1,0 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 1,1,0,1 }, 1 << Dir_MZ });
  solid_model.faces.push_back({ { 0,0,1 },{ 0,1,1 },{ 1,1,1 },{ 1,0,1 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 },{ 1,1,1,1 }, 1 << Dir_PZ });
  BlockProperies prop_air;
  prop_air.getModel = getEmptyModel;
  prop_air.getNeeds = getEmptyNeeds;
  BlockProperies prop_sol;
  prop_sol.getModel = getSolidModel;
  prop_sol.getNeeds = getSolidNeeds;
  blockProperties = {prop_air, prop_sol};
}
#endif