#pragma once

#include "../Blocks/Block_Types.h"

typedef void(*onEntityMove)(mVec3& to);

typedef void(*onEntityLook)(sVec3& at);

class Entity {
protected:
  mVec3 _pos; //center
  mVec3 _headOffset;
  mVec3 _size; //from center to +,+,+ corner
  guid_t _guid;
  uint32_t _type;
  mass_type_kg _mass;
  mpsVec3 _velocity;
  mpssVec3 _selfAccel;
  sVec3 _lookDir;
  vel_type_mpers _speed;
  float _turnDir;
  int _dim;

  onEntityMove onMove;
  onEntityLook onLook;
public:
  Entity(guid_t guid);

  inline mVec3& getCenter() {
    return _pos;
  }
  inline mVec3& getFeetCenter() {
    return _pos - mVec3(0, 0, _size.z);
  }
  inline mVec3& getHead() {
    return _pos + rotateVecZ(_headOffset, _turnDir);
  }
  inline mVec3& getHalfSize() {
    return _size;
  }
  inline guid_t& getGUID() {
    return _guid;
  }
  inline uint32_t& getType() {
    return _type;
  }
  inline mass_type_kg& getMass() {
    return _mass;
  }
  inline mpsVec3& getVelocity() {
    return _velocity;
  }
  inline sVec3& getLook() {
    return _lookDir;
  }
  inline vel_type_mpers& getSpeed() {
    return _speed;
  }
  inline float& getTurn() {
    return _turnDir;
  }
  inline int& getDim() {
    return _dim;
  }

  void setPos(mVec3 pos);
  void movPos(mVec3 pos);

  void setVel(mpsVec3 vel);

  void EntityTryLook(sVec3 at);

  void set(DataElement* from);
  void get(DataElement* to);

  PhysCube getPhysCube();
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

extern set<Entity*> entities;