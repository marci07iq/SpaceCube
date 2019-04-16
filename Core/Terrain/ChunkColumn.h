#pragma once

#include "Chunk.h"

class ChunkCol {
private:
  Chunk* _chunks[CHUNK_PER_COLUMN];
public:
  list<guid_t> _loaders;
  list<Entity*> _entities;
  int _ccx, _ccy;
  Fragment* _frag;
 
  ChunkCol* _neigh[4] = { NULL, NULL, NULL, NULL };

  ChunkCol(int ccx, int ccy, Fragment* frag) {
    _ccx = ccx;
    _ccy = ccy;
    for (int i = 0; i < 4; i++) {
      _neigh[i] = NULL;
    }
    for (int i = 0; i < CHUNK_PER_COLUMN; i++) {
        _chunks[i] = NULL;
    }
    _frag = frag;
  }

  ~ChunkCol();

  void get(DataElement* to);

  void set(const DataElement* from);

  void setChunk(Chunk* c, int cz);

  void link();
  void hello(Directions fromDir, ChunkCol* fromChunk);
  void unlink();
  void bye(Directions fromDir, ChunkCol* fromChunk);

  inline Chunk* getChunk(int cz) {
    if(0 <= cz && cz < CHUNK_PER_COLUMN) {
      return _chunks[cz];
    }
    return NULL;
  }
};
