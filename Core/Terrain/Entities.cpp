#include "WorldLoader.h"

void Entity::EntityTryMoveWith(mVec3 & with) {
  _pos += with;
  onMove(_pos);
}

void Entity::EntityTryMoveTo(mVec3 & to) {
  _pos = to;
  onMove(_pos);
}

void Entity::EntityTryLook(sVec3 & at) {
  _lookDir = at;
  onLook(_lookDir);
}

Entity::Entity(guid_t guid) {
  _guid = guid;
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
  oGFunc(_size, ne);
  to->addChild(ne);
  ne = new DataElement();
  oGFunc(_headOffset, ne);
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
    _speed = 10;
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
  switch (Id) {
    case PacketChunk:
      int cx, cz;
      vSFunc(cx, Data->_children[0]);
      vSFunc(cz, Data->_children[1]);

      ChunkCol* cc = findLoadColumn(cx, cz, 0);
      for (int k = 0; k < CHUNK_PER_COLUMN; k++) {
        if (cc->getChunk(k) != NULL) {
          sendChunk(cc->getChunk(k));
        }
      }
      return false;
      break;
  }
  return false;
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
  }
  return false;
}
#endif