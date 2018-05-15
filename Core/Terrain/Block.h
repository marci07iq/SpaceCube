#pragma once

#ifdef M_CLIENT
#include "BlockModel.h"
#endif
#ifdef M_SERVER
#include "../Definitions.h"
#endif

class Fragment;
class ChunkCol;
class Chunk;

class Entity;


struct Block {
  block_id_t _ID;
  meta_id_t _meta;

  Block();

  Block(block_id_t ID);

  Block(block_id_t ID, meta_id_t meta);
};

struct BlockPos {
  Chunk* c;
  int lbx, lby, lbz; //Local block coordinates in chunk
  Block& b;
};

void writeBlock(unsigned char* to, int id, Block& b);

void readBlock(unsigned char* from, int id, Block& b);

typedef void(*onBlockUpdate)(BlockPos&);
typedef void(*onBlockTick)(BlockPos&);
typedef void(*onBlockInteract)(BlockPos&, Entity*, int, void*);
typedef void(*onBlockLook)(BlockPos&, Entity*, int, void*);
typedef void(*onBlockBreak)(BlockPos&, Entity*, int, void*);
#ifdef M_CLIENT
typedef void(*getBlockModel)(BlockPos&, BlockNeeds, list<QuadFace>&);
typedef BlockNeeds(*getBlockNeeds)(Block&);
typedef vec2<float>(*getBlockTex)(BlockPos&, uint32_t);
#endif
struct BlockProperies {
  onBlockUpdate onUpdate;
  onBlockTick onTick;
  onBlockInteract onInteract;
  onBlockLook onLook;
  onBlockBreak onBreak;
#ifdef M_CLIENT
  getBlockModel getModel;
  getBlockNeeds getNeeds;
  getBlockTex getTex;
#endif
};

extern vector<BlockProperies> blockProperties;

#ifdef M_CLIENT
void loadBlocks();
#endif