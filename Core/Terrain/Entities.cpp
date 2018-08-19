#include "WorldLoader.h"
#include "Entities.h"

/*void Entity::EntityTryMoveWith(mVec3 with) {
  _pos += with;
  if(onMove) {
    onMove(_pos);
  }
}

void Entity::EntityTryMoveTo(mVec3 to) {
  _pos = to;
  if (onMove) {
    onMove(_pos);
  }
}*/

void Entity::setPos(mVec3 pos) {
#ifdef M_CLIENT
  int ncx = floor(pos.x / BLOCK_PER_CHUNK);
  int ncy = floor(pos.y / BLOCK_PER_CHUNK);
  int ocx = floor(_pos.x / BLOCK_PER_CHUNK);
  int ocy = floor(_pos.y / BLOCK_PER_CHUNK);
#endif
  _pos = pos;
#ifdef M_CLIENT
  if(ncx != ocx || ncy != ocy) {
    loadChunks();
  }
#endif
}

void Entity::movPos(mVec3 pos) {
  setPos(_pos + pos);
}

void Entity::setVel(mpsVec3 vel) {
  _velocity = vel;
}

void Entity::EntityTryLook(sVec3 at) {
  _lookDir = at;
  /*if(onLook) {
    onLook(_lookDir);
  }*/
}

Entity::Entity(guid_t guid) {
  _guid = guid;
  _size = mVec3(0.4, 0.4, 0.8);
  _headOffset = mVec3(0,0,1.5);
  cout << _size << endl;
}

void Entity::set(DataElement * from) {
  oSFunc(_pos, from->_children[0]);
  oSFunc(_headOffset, from->_children[1]);
  oSFunc(_size, from->_children[2]);
  vSFunc(_guid, from->_children[3]);
  vSFunc(_type, from->_children[4]);
  vSFunc(_mass, from->_children[5]);
  oSFunc(_velocity, from->_children[6]);
  oSFunc(_lookDir, from->_children[7]);
  vSFunc(_speed, from->_children[8]);

}

void Entity::get(DataElement * to) {
  DataElement* ne;
  ne = new DataElement();
  oGFunc(_pos, ne);
  to->addChild(ne);
  ne = new DataElement();
  oGFunc(_headOffset, ne);
  to->addChild(ne);
  ne = new DataElement();
  oGFunc(_size, ne);
  to->addChild(ne);
  ne = new DataElement();
  vGFunc(_guid, ne);
  to->addChild(ne);
  ne = new DataElement();
  vGFunc(_type, ne);
  to->addChild(ne);
  ne = new DataElement();
  vGFunc(_mass, ne);
  to->addChild(ne);
  ne = new DataElement();
  oGFunc(_velocity, ne);
  to->addChild(ne);
  ne = new DataElement();
  oGFunc(_lookDir, ne);
  to->addChild(ne);
  ne = new DataElement();
  vGFunc(_speed, ne);
  to->addChild(ne);
}

void Entity::loadChunks() {
  int ncx = floor(_pos.x / BLOCK_PER_CHUNK);
  int ncy = floor(_pos.y / BLOCK_PER_CHUNK);
  for (int wcx = ncx - CHUNK_LOAD_RADIUS; wcx <= ncx + CHUNK_LOAD_RADIUS; wcx++) {
    for (int wcy = ncy - CHUNK_LOAD_RADIUS; wcy <= ncy + CHUNK_LOAD_RADIUS; wcy++) {
      findLoadChunkCol(wcx, wcy, _dim);
    }
  }
}

PhysCube Entity::getPhysCube() {
  PhysCube ans;
  ans.bounce = 0;
  ans.drag = 1;
  ans.invMass = 1 / _mass;
  ans.nc = _pos - _size * mVec3(1,1,0);
  ans.pc = _pos + _size * mVec3(1,1,2);
  return ans;
}

Player::Player(guid_t guid) : Entity(guid) {
  //_guid = guid;
}

void Player::loadFile() {
  DataElement* me = new DataElement();
  if(loadFromFile(me, to_string(_guid) + ".ent")) {
    set(me);
  } else {
    _lookDir = fVec3(0.3,0.3,-0.9).norm();
    _pos ={0, 0, 50};
    _speed = 50;
    saveFile();
  }
  delete me;

}

void Player::saveFile() {
  DataElement* me;
  me = new DataElement();
  get(me);
  saveToFile(me, to_string(_guid) + ".ent");
}

NetBinder::NetBinder(guid_t guid) : Player(guid) {
  //_guid = guid;
}

void NetBinder::sendBlock(iVec3 pos, Block& what) {
  cout << pos << " " << what._raw << endl;
  DataElement* res = new DataElement();
  DataElement* nd;
  nd = new DataElement();
  oGFunc(pos, nd);
  res->addChild(nd);
  nd = new DataElement();
  vGFunc(what._raw, nd);
  res->addChild(nd);

  connection->SendData(res, PacketBlock);
}

#ifdef M_SERVER
void NetBinder::sendChunk(Chunk* what) {

  DataElement* res = new DataElement();
  DataElement* nd;
  nd = new DataElement();
  vGFunc(what->_cx, nd);
  res->addChild(nd);
  nd = new DataElement();
  vGFunc(what->_cy, nd);
  res->addChild(nd);
  nd = new DataElement();
  vGFunc(what->_cz, nd);
  res->addChild(nd);

  what->get(res);

  connection->SendData(res, PacketChunk);
}
bool NetBinder::recivePacket(DataElement * Data, int Id, NetworkS * thisptr, NetBinder * connected) {
  ChunkCol* cc = NULL;
  switch (Id) {
    case PacketChunk:
      int cx, cz;
      vSFunc(cx, Data->_children[0]);
      vSFunc(cz, Data->_children[1]);

      cc = findLoadChunkCol(cx, cz, 0);
      cc->_loaders.push_back(connected->getGUID());
      for (int k = 0; k < CHUNK_PER_COLUMN; k++) {
        if (cc->getChunk(k) != NULL) {
          sendChunk(cc->getChunk(k));
        }
      }
      return false;
      break;
    case PacketBlock:
      iVec3 pos;
      Block b;
      oSFunc(pos, Data->_children[0]);
      vSFunc(b._raw, Data->_children[1]);
      setBlock(pos, 0, b);
      return false;
      break;
  }
  return true;
}
#endif
#ifdef M_CLIENT

bool NetBinder::recivePacket(DataElement * Data, int Id, NetworkC * thisptr) {
  switch (Id) {
    case PacketChunk:
      int cx, cy, cz;
      vSFunc(cx, Data->_children[0]);
      vSFunc(cy, Data->_children[1]);
      vSFunc(cz, Data->_children[2]);
      setChunk(cx, cy, cz, 0, Data);
      return false;
      break;
    case PacketBlock:
      iVec3 pos;
      Block b;
      oSFunc(pos, Data->_children[0]);
      vSFunc(b._raw, Data->_children[1]);
      setBlock(pos, 0, b);
      break;
  }
  return false;
}
#endif

map<guid_t, Entity*> entities;
