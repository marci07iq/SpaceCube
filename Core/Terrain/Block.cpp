#include "WorldLoader.h"

Block::Block() {
}

Block::Block(block_id_t ID) :
  _ID(ID) {
}

BinTree<BlockProperies> blockProperties;

void writeBlock(unsigned char * to, int id, Block & b) {
  *(reinterpret_cast<block_id_t*>(to + id)) = b._ID;
}

void readBlock(unsigned char * from, int id, Block & b) {
  b._ID = *(reinterpret_cast<block_id_t*>(from + id));
}

map<string, BlockModel> models;
BinTree<BlockModel> blockModels;

void loadModel(string folder, string name) {
  ifstream in(folder + name + ".modl", ios::in);
  BlockModel m;
  if(in.is_open()) {
    StoredQuadFace f;
    int type;
    while(in >> f.base.vbl >> f.base.vtl >> f.base.vtr >> f.base.vbr >> f.base.tbl >> f.base.ttl >> f.base.ttr >> f.base.tbr >> f.base.recolor.r >> f.base.recolor.g >> f.base.recolor.b >> f.base.recolor.a >> type >> f.faceID) {
      #ifdef M_CLIENT
      f.type = type;
      #endif
      m.faces.push_back(f);
    }
  }
  models[name] = m;
  in.close();
}

uint64_t hexTo64(string& name) {
  uint64_t res = 0;
  for (auto&& it : name) {
    if ('0' <= it && it <= '9') {
      res <<= 4;
      res += it - '0';
    }
    if ('a' <= it && it <= 'f') {
      res <<= 4;
      res += it - 'a' + 10;
    }
    if ('A' <= it && it <= 'F') {
      res <<= 4;
      res += it - 'A' + 10;
    }
  }
  return res;
}

void loadBlocks() {
  ifstream in("Textures/blocks.dat");

  string idHexCode;
  uint64_t idCode;
  size_t idCodeLen;
  string idName;
  string dispName;
  string modelName;
  int needs;

  while (in >> idHexCode >> idCodeLen >> idName >> dispName >> modelName) {
    idCode = hexTo64(idHexCode);
    cout << idHexCode << " " << idCode << " " << idCodeLen << " " << idName << " " << dispName << " " << modelName << " " << endl;
    if (!models.count(modelName)) {
      loadModel("Textures/", modelName);
    }
    int size = models[modelName].faces.size();
    auto it = blockProperties[make_pair(idCode, idCodeLen)];
    for (int i = 0; i < size; i++) {
      int a;
      in >> a;
      #ifdef M_CLIENT
      it.operator->().textures.push_back(a);
      #endif
    }
    blockModels[make_pair(idCode, idCodeLen)].operator->() = models[modelName];
  }

  in.close();

  #ifdef M_CLIENT
  blockProperties[make_pair(0x0000000000000000,64)].operator->().getNeeds = getEmptyNeeds;
  blockProperties[make_pair(0x0000000000000001,01)].operator->().getNeeds = getSolidNeeds;
  blockProperties[make_pair(0x1000000000000000,64)].operator->().getNeeds = getSolidNeeds;
  blockProperties[make_pair(0x2000000000000000,64)].operator->().getNeeds = getSolidNeeds;
  blockProperties[make_pair(0x3000000000000000,64)].operator->().getNeeds = getEmptyNeeds;
  blockProperties[make_pair(0x4000000000000000,64)].operator->().getNeeds = getEmptyNeeds;
  
  blockProperties[make_pair(0x0000000000000000, 64)].operator->().getModel = getStoredModel;
  blockProperties[make_pair(0x0000000000000001, 01)].operator->().getModel = getStoredModel;
  blockProperties[make_pair(0x1000000000000000, 64)].operator->().getModel = getStoredModel;
  blockProperties[make_pair(0x2000000000000000, 64)].operator->().getModel = getStoredModel;
  blockProperties[make_pair(0x3000000000000000, 64)].operator->().getModel = getConnectedModel;
  blockProperties[make_pair(0x4000000000000000, 64)].operator->().getModel = getStoredModel;

  blockProperties[make_pair(0x0000000000000000, 64)].operator->().getTex = getStoredTex;
  blockProperties[make_pair(0x0000000000000001, 01)].operator->().getTex = getStoredTex;
  blockProperties[make_pair(0x1000000000000000, 64)].operator->().getTex = getStoredTex;
  blockProperties[make_pair(0x2000000000000000, 64)].operator->().getTex = getStoredTex;
  blockProperties[make_pair(0x3000000000000000, 64)].operator->().getTex = getStoredTex;
  blockProperties[make_pair(0x4000000000000000, 64)].operator->().getTex = getStoredTex;
  #endif
}
