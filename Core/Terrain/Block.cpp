#include "WorldLoader.h"

Block::Block() {
}

Block::Block(block_id_t ID) :
  _ID(ID) {
}

vector<BlockProperies> blockProperties;

void writeBlock(unsigned char * to, int id, Block & b) {
  *(reinterpret_cast<block_id_t*>(to + id)) = b._ID;
}

void readBlock(unsigned char * from, int id, Block & b) {
  b._ID = *(reinterpret_cast<block_id_t*>(from + id));
}

map<string, BlockModel> models;
vector<BlockModel> blockModels;

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
  uint64_t idCode = 0;
  size_t idCodeLen;
  string idName;
  string dispName;
  string modelName;
  int needs;

  while (in >> idHexCode >> idCodeLen >> idName >> dispName >> modelName) {
    blockProperties.resize(idCode+1);
    blockModels.resize(idCode+1);
    cout << idHexCode << " " << idCode << " " << idCodeLen << " " << idName << " " << dispName << " " << modelName << " " << endl;
    if (!models.count(modelName)) {
      loadModel("Textures/", modelName);
    }
    int size = models[modelName].faces.size();
    auto&& it = blockProperties[idCode];
    for (int i = 0; i < size; i++) {
      int a;
      in >> a;
      #ifdef M_CLIENT
      it.textures.push_back(a);
      #endif
    }
    blockModels[idCode] = models[modelName];
    idCode++;
  }

  in.close();

  #ifdef M_SERVER
  blockProperties[0].actionDestory = actionDestoryNothing;
  blockProperties[1].actionDestory = actionDestoryNothing;
  blockProperties[2].actionDestory = actionDestoryNothing;
  blockProperties[3].actionDestory = actionDestoryNothing;
  blockProperties[4].actionDestory = actionDestoryNothing;
  blockProperties[5].actionDestory = actionDestoryNothing;

  blockProperties[0].onUpdate = onUpdateNothing;
  blockProperties[1].onUpdate = onUpdateNothing;
  blockProperties[2].onUpdate = onUpdateNothing;
  blockProperties[3].onUpdate = onUpdateNothing;
  blockProperties[4].onUpdate = onUpdateNothing;
  blockProperties[5].onUpdate = onUpdateNature;
  #endif

  #ifdef M_CLIENT
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
  #endif
}
