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

vector<onBlockUpdate> blockUpdates;
vector<onBlockTick> blockTicks;
vector<onBlockLook> blockLooks;
vector<onBlockInteract> blockInteracts;
vector<onBlockBreak> blockBreaks;

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
map<string, BlockModel> models;
vector<BlockModel> blockModels;


void loadModel(string folder, string name) {
  ifstream in(folder + name + ".modl", ios::in);
  BlockModel m;
  if(in.is_open()) {
    StoredQuadFace f;
    int type;
    while(in >> f.base.vbl >> f.base.vtl >> f.base.vtr >> f.base.vbr >> f.base.tbl >> f.base.ttl >> f.base.ttr >> f.base.tbr >> f.base.recolor.r >> f.base.recolor.g >> f.base.recolor.b >> f.base.recolor.a >> type >> f.faceID) {
      f.type = type;
      m.faces.push_back(f);
    }
  }
  models[name] = m;
  in.close();
}

void loadBlocks() {
  ifstream in("Textures/blocks.dat");

  int id = 0;
  string modname;
  int needs;

  while (in >> modname) {
    blockProperties.push_back(BlockProperies());
    blockModels.push_back(BlockModel());

    if (!models.count(modname)) {
      loadModel("Textures/", modname);
    }
    int size = models[modname].faces.size();
    for (int i = 0; i < size; i++) {
      int a;
      in >> a;
      blockProperties[id].textures.push_back(a);
    }
    blockModels[id] = models[modname];
    ++id;
  }

  in.close();

  blockProperties[0].getNeeds = getEmptyNeeds;
  blockProperties[1].getNeeds = getSolidNeeds;
  blockProperties[2].getNeeds = getSolidNeeds;
  blockProperties[3].getNeeds = getSolidNeeds;
  blockProperties[4].getNeeds = getEmptyNeeds;
  blockProperties[5].getNeeds = getEmptyNeeds;
  
  blockProperties[0].getModel = getStoredModel;
  blockProperties[1].getModel = getStoredModel;
  blockProperties[2].getModel = getStoredModel;
  blockProperties[3].getModel = getStoredModel;
  blockProperties[4].getModel = getConnectedModel;
  blockProperties[5].getModel = getStoredModel;

  blockProperties[0].getTex = getStoredTex;
  blockProperties[1].getTex = getStoredTex;
  blockProperties[2].getTex = getStoredTex;
  blockProperties[3].getTex = getStoredTex;
  blockProperties[4].getTex = getStoredTex;
  blockProperties[5].getTex = getStoredTex;
}
#endif