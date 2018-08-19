#include "WorldLoader.h"

Block::Block() {
  _raw = 0;
}

Block::Block(uint16_t ID) {
  _raw = 0;
  _ID = ID;
}

Block::Block(uint16_t ID, uint64_t meta) {
  _raw = 0;
  _ID = ID;
  _meta = meta;
}

vector<BlockProperies> blockProperties;

void writeBlock(unsigned char * to, int id, Block & b) {
  *(reinterpret_cast<uint64_t*>(to + id)) = b._raw;
}

void readBlock(unsigned char * from, int id, Block & b) {
  b._raw = *(reinterpret_cast<uint64_t*>(from + id));
}

map<string, BlockModel> models;
vector<BlockModel> blockModels;

void loadModel(string folder, string name) {
  ifstream in(folder + name + ".modl", ios::in);
  BlockModel m;
  if(in.is_open()) {
    int faces, cubes;
    in >> faces >> cubes;
    StoredQuadFace f;
    int type;
    while(faces--) {
      in >> f.base.vbl >> f.base.vtl >> f.base.vtr >> f.base.vbr >> f.base.tbl >> f.base.ttl >> f.base.ttr >> f.base.tbr >> type >> f.faceID;
      #ifdef M_CLIENT
      f.type = type;
      #endif
      m.faces.push_back(f);
    }
    PhysCube newCube;
    while (cubes--) {
      in >> newCube.nc.x >> newCube.nc.y >> newCube.nc.z >> newCube.pc.x >> newCube.pc.y >> newCube.pc.z;
      m.cubes.push_back(newCube);
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
  uint64_t idCode = 0;
  string idName;
  string dispName;
  string modelName;
  int needs;

  while (in >> idName >> dispName >> modelName) {
    blockProperties.resize(idCode+1);
    blockModels.resize(idCode+1);
    cout << idCode << " " << idName << " " << dispName << " " << modelName << " " << endl;
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
  blockProperties[6].actionDestory = actionDestoryNothing;

  blockProperties[0].onUpdate = onUpdateNothing;
  blockProperties[1].onUpdate = onUpdateNothing;
  blockProperties[2].onUpdate = onUpdateNothing;
  blockProperties[3].onUpdate = onUpdateNothing;
  blockProperties[4].onUpdate = onUpdateNothing;
  blockProperties[5].onUpdate = onUpdateNature;
  blockProperties[6].onUpdate = onUpdateNothing;

  #endif

  #ifdef M_CLIENT
  blockProperties[0].getNeeds = getEmptyNeeds;
  blockProperties[1].getNeeds = getSolidNeeds;
  blockProperties[2].getNeeds = getSolidNeeds;
  blockProperties[3].getNeeds = getSolidNeeds;
  blockProperties[4].getNeeds = getEmptyNeeds;
  blockProperties[5].getNeeds = getEmptyNeeds;
  blockProperties[6].getNeeds = getSolidNeeds;


  blockProperties[0].getModel = getStoredModel;
  blockProperties[1].getModel = getStoredModel;
  blockProperties[2].getModel = getStoredModel;
  blockProperties[3].getModel = getStoredModel;
  blockProperties[4].getModel = getConnectedModel;
  blockProperties[5].getModel = getStoredModel;
  blockProperties[6].getModel = getWoodModel;


  blockProperties[0].getTex = getStoredTex;
  blockProperties[1].getTex = getStoredTex;
  blockProperties[2].getTex = getStoredTex;
  blockProperties[3].getTex = getStoredTex;
  blockProperties[4].getTex = getStoredTex;
  blockProperties[5].getTex = getStoredTex;
  blockProperties[6].getTex = getStoredTex;

  #endif

  blockProperties[0].getPhysics = getStoredPhysics;
  blockProperties[1].getPhysics = getStoredPhysics;
  blockProperties[2].getPhysics = getStoredPhysics;
  blockProperties[3].getPhysics = getStoredPhysics;
  blockProperties[4].getPhysics = getStoredPhysics;
  blockProperties[5].getPhysics = getStoredPhysics;
  blockProperties[6].getPhysics = getStoredPhysics;
}
