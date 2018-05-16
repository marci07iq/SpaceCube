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
map<string, BlockModel> models;
vector<BlockModel> blockModels;

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
      b[6].c->_cz * BLOCK_PER_CHUNK + b[6].lbz * 1.0f};
      res.vbl = it.base.vbl + location;
      res.vbr = it.base.vbr + location;
      res.vtl = it.base.vtl + location;
      res.vtr = it.base.vtr + location;

      vec2<float> texCoord = prop.getTex(b[6], it.faceID);
      res.tbl = (it.base.tbl + texCoord) * vec2<float>{1.0f, 0.2f} * TEXTURE_SIZE;
      res.tbr = (it.base.tbr + texCoord) * vec2<float>{1.0f, 0.2f}  * TEXTURE_SIZE;
      res.ttl = (it.base.ttl + texCoord) * vec2<float>{1.0f, 0.2f}  * TEXTURE_SIZE;
      res.ttr = (it.base.ttr + texCoord) * vec2<float>{1.0f, 0.2f}  * TEXTURE_SIZE;

      res.recolor = it.base.recolor;

      addTo.push_back(res);
    }
  }
}
void getConnectedModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo) {
  BlockProperies& prop = blockProperties[b[6].b->_ID];

  BlockNeeds notMe = 1 << Dir_All;
  for (int i = 0; i < 6; i++) {
    if (b[i].b != NULL && b[i].b->_ID != b[6].b->_ID) {
      notMe |= 1 << i;
    }
  }

  for (auto&& it : blockModels[b[6].b->_ID].faces) {
    if (it.type & n & notMe) {
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
      res.tbl = (it.base.tbl + texCoord) * vec2<float>{1.0f, 0.2f} *TEXTURE_SIZE;
      res.tbr = (it.base.tbr + texCoord) * vec2<float>{1.0f, 0.2f}  *TEXTURE_SIZE;
      res.ttl = (it.base.ttl + texCoord) * vec2<float>{1.0f, 0.2f}  *TEXTURE_SIZE;
      res.ttr = (it.base.ttr + texCoord) * vec2<float>{1.0f, 0.2f}  *TEXTURE_SIZE;

      res.recolor = it.base.recolor;

      addTo.push_back(res);
    }
  }
}

vec2<float> getStoredTex(BlockPos& b, uint32_t texID) {
  return {0.0f, 1.0f*blockProperties[b.b->_ID].textures[texID]};
}


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
  
  blockProperties[0].getModel = getStoredModel;
  blockProperties[1].getModel = getStoredModel;
  blockProperties[2].getModel = getStoredModel;
  blockProperties[3].getModel = getStoredModel;
  blockProperties[4].getModel = getConnectedModel;

  blockProperties[0].getTex = getStoredTex;
  blockProperties[1].getTex = getStoredTex;
  blockProperties[2].getTex = getStoredTex;
  blockProperties[3].getTex = getStoredTex;
  blockProperties[4].getTex = getStoredTex;
}
#endif