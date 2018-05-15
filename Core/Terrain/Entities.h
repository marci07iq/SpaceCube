#pragma once

#include "Block.h"

typedef void(*onEntityMove)(mVec3& to);

typedef void(*onEntityLook)(sVec3& at);

class Entity {
public:
  mVec3 _pos; //center
  mVec3 _headOffset;
  mVec3 _size; //from center to +,+,+ corner
  guid_t _guid;
  int _type;
  mass_type_kg _mass;
  mpssVec3 _velocity;
  sVec3 _lookDir;
  vel_type_mpers _speed;

  void EntityTryMoveWith(mVec3& with);
  void EntityTryMoveTo(mVec3& to);
  onEntityMove onMove;

  void EntityTryLook(sVec3& at);
  onEntityLook onLook;

  Entity(guid_t guid);

  void set(DataElement* from);
  void get(DataElement* to);
};

class Player : public Entity {
public:
  Player(guid_t guid);

  void loadFile();

  void saveFile();
};

class NetBinder : public Player {
public:
#ifdef M_CLIENT
  NetworkC* connection;
  bool recivePacket(DataElement* Data, int Id, NetworkC* thisptr);
#endif
#ifdef M_SERVER
  NetworkS* connection;
  void sendChunk(Chunk* what);
  bool recivePacket(DataElement* Data, int Id, NetworkS* thisptr, NetBinder* connected);
#endif
  NetBinder(guid_t guid);

};