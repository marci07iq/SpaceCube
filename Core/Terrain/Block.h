#pragma once

#include "BlockModel.h"

class Fragment;
class ChunkCol;
class Chunk;

class Entity;


struct Block {
  block_id_t _ID;

  Block();

  Block(block_id_t ID);
};

struct BlockPos {
  Chunk* c;
  int lbx, lby, lbz; //Local block coordinates in chunk
  Block* b;
};

void writeBlock(unsigned char* to, int id, Block& b);

void readBlock(unsigned char* from, int id, Block& b);

typedef void(*getBlockPhysics)(BlockPos[7], list<PhysCube>&);
#ifdef M_SERVER
typedef void(*onBlockUpdate)(BlockPos&);
typedef void(*onBlockTick)(BlockPos&);
typedef void(*onBlockInteract)(BlockPos&, Entity*, int, void*);
typedef void(*onBlockLook)(BlockPos&, Entity*, int, void*);
typedef void(*onBlockBreak)(BlockPos&, Entity*, int, void*);
typedef void(*actionBlockDestory)(BlockPos&);
#endif
#ifdef M_CLIENT
typedef void(*getBlockModel)(BlockPos[7], BlockNeeds, list<QuadFace>&);
typedef BlockNeeds(*getBlockNeeds)(Block&);
typedef vec2<float>(*getBlockTex)(BlockPos&, uint32_t);
#endif
struct BlockProperies {
  getBlockPhysics getPhysics;

#ifdef M_SERVER
  onBlockUpdate onUpdate;
  onBlockTick onTick;
  onBlockInteract onInteract;
  onBlockLook onLook;
  onBlockBreak onBreak;
  actionBlockDestory actionDestory;
#endif
#ifdef M_CLIENT
  getBlockModel getModel;
  getBlockNeeds getNeeds;
  getBlockTex getTex;
  vector<int> textures;
#endif
};

extern vector<BlockProperies> blockProperties;

void loadBlocks();

#ifdef M_CLIENT
extern map<string, BlockModel> models;
extern vector<BlockModel> blockModels;

#endif