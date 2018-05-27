#include "../Terrain/WorldLoader.h"

#ifdef M_CLIENT
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
      res.tbl = (it.base.tbl + texCoord) * TEXTURE_SIZE;
      res.tbr = (it.base.tbr + texCoord) * TEXTURE_SIZE;
      res.ttl = (it.base.ttl + texCoord) * TEXTURE_SIZE;
      res.ttr = (it.base.ttr + texCoord) * TEXTURE_SIZE;

      res.recolor = it.base.recolor;

      /*res.rbl = 0;
      res.rtl = 0;
      res.rtr = 0;
      res.rbr = 0;*/

      //if(it.faceID < 4) {
        //if (!(notMe & 31 & ((it.type) | rotateCW(it.type)))) {
          res.rbl = 0.153 + locationRandomF(0, 1, 0, res.vbl.x / 10, res.vbl.y / 10, res.vbl.z / 10)*0.002;
        //}
        //if (!(notMe & 31 & ((it.type) | rotateCW(it.type)))) {
          res.rtl = 0.153 + locationRandomF(0, 1, 0, res.vtl.x / 10, res.vtl.y / 10, res.vtl.z / 10)*0.002;
        //}
        //if (!(notMe & 31 & ((it.type) | rotateCCW(it.type)))) {
          res.rtr = 0.153 + locationRandomF(0, 1, 0, res.vtr.x / 10, res.vtr.y / 10, res.vtr.z / 10)*0.002;
        //}
        //if (!(notMe & 31 & ((it.type) | rotateCCW(it.type)))) {
          res.rbr = 0.153 + locationRandomF(0, 1, 0, res.vbr.x / 10, res.vbr.y / 10, res.vbr.z / 10)*0.002;
        //}
      /*} else {
        if (!(notMe & 5)) {
          res.rbl = 0.202 + locationRandom(0, 1, 0, res.vbl.x, res.vbl.y, res.vbl.z)*0.002;
        }
        if (!(notMe & 9)) {
          res.rtl = 0.202 + locationRandom(0, 1, 0, res.vtl.x, res.vtl.y, res.vtl.z)*0.002;
        }
        if (!(notMe & 10)) {
          res.rtr = 0.202 + locationRandom(0, 1, 0, res.vtr.x, res.vtr.y, res.vtr.z)*0.002;
        }
        if (!(notMe & 6)) {
          res.rbr = 0.202 + locationRandom(0, 1, 0, res.vbr.x, res.vbr.y, res.vbr.z)*0.002;
        }
      }*/


      addTo.push_back(res);
    }
  }
}
#endif