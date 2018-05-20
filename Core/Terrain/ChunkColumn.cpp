#include "WorldLoader.h"

ChunkCol::~ChunkCol() {
  for (int i = 0; i < CHUNK_PER_COLUMN; i++) {
    if (_chunks[i] != NULL) {
      delete _chunks[i];
    }
  }
  for (int i = 0; i < 4; i++) {
    if (_neigh[i] != NULL) {
      _neigh[i]->bye(static_cast<Directions>(i ^ 1), this);
    }
  }
  _frag->setChunkCol(modulo(_ccx, COLUMN_PER_FRAGMENT), modulo(_ccy, COLUMN_PER_FRAGMENT), NULL);
}

void ChunkCol::get(DataElement* to) {
  for (int c = 0; c < CHUNK_PER_COLUMN; c++) {
    DataElement* nce = new DataElement();
    _chunks[c]->get(nce);
    to->addChild(nce);
  }
}

void ChunkCol::set(const DataElement* from) {
  for(int c = 0; c < CHUNK_PER_COLUMN; c++) {
    if (_chunks[c]) {
      throw 1;
      return;
    }
    _chunks[c]->set(from->_children[c]);
  }
}

void ChunkCol::setChunk(Chunk* c, int cz) {
  if (_chunks[cz] != NULL) {
    throw 1;
    delete _chunks[cz];
  }
  c->_col = this;
  _chunks[cz] = c;
  c->_state = Chunk_Loaded;
  c->link();
}

void ChunkCol::link() {
  int lccx = modulo(_ccx, COLUMN_PER_FRAGMENT);
  int lccy = modulo(_ccy, COLUMN_PER_FRAGMENT);

  if (0 < lccx) {
    ChunkCol* potentialNeigh = _frag->getChunkCol(lccx - 1, lccy);
    if (potentialNeigh != NULL) {
      potentialNeigh->hello(Dir_PX, this);
      _neigh[Dir_MX] = potentialNeigh;
    }
  } else {
    Fragment* neighFragment = _frag->_neigh[Dir_MX];
    if (neighFragment != NULL) {
      ChunkCol* potentialNeigh = neighFragment->getChunkCol(COLUMN_PER_FRAGMENT - 1, lccy);
      if (potentialNeigh != NULL) {
        potentialNeigh->hello(Dir_PX, this);
        _neigh[Dir_MX] = potentialNeigh;
      }
    }
  }
  if (lccx < COLUMN_PER_FRAGMENT - 1) {
    ChunkCol* potentialNeigh = _frag->getChunkCol(lccx + 1, lccy);
    if (potentialNeigh != NULL) {
      potentialNeigh->hello(Dir_MX, this);
      _neigh[Dir_PX] = potentialNeigh;
    }
  } else {
    Fragment* neighFragment = _frag->_neigh[Dir_PX];
    if (neighFragment != NULL) {
      ChunkCol* potentialNeigh = neighFragment->getChunkCol(0, lccy);
      if (potentialNeigh != NULL) {
        potentialNeigh->hello(Dir_MX, this);
        _neigh[Dir_PX] = potentialNeigh;
      }
    }
  }

  if (0 < lccy) {
    ChunkCol* potentialNeigh = _frag->getChunkCol(lccx, lccy - 1);
    if (potentialNeigh != NULL) {
      potentialNeigh->hello(Dir_PY, this);
      _neigh[Dir_MY] = potentialNeigh;
    }
  } else {
    Fragment* neighFragment = _frag->_neigh[Dir_MY];
    if (neighFragment != NULL) {
      ChunkCol* potentialNeigh = neighFragment->getChunkCol(lccx, COLUMN_PER_FRAGMENT - 1);
      if (potentialNeigh != NULL) {
        potentialNeigh->hello(Dir_PY, this);
        _neigh[Dir_MY] = potentialNeigh;
      }
    }
  }
  if (lccy < COLUMN_PER_FRAGMENT - 1) {
    ChunkCol* potentialNeigh = _frag->getChunkCol(lccx, lccy + 1);
    if (potentialNeigh != NULL) {
      potentialNeigh->hello(Dir_MY, this);
      _neigh[Dir_PY] = potentialNeigh;
    }
  } else {
    Fragment* neighFragment = _frag->_neigh[Dir_PY];
    if (neighFragment != NULL) {
      ChunkCol* potentialNeigh = neighFragment->getChunkCol(lccx, 0);
      if (potentialNeigh != NULL) {
        potentialNeigh->hello(Dir_MY, this);
        _neigh[Dir_PY] = potentialNeigh;
      }
    }
  }

  for (int i = 0; i < CHUNK_PER_COLUMN; i++) {
    if (_chunks[i] != NULL) {
      _chunks[i]->link();
    }
  }
}
void ChunkCol::hello(Directions fromDir, ChunkCol* fromChunk) {
  if(_neigh[fromDir] != fromChunk) {
    _neigh[fromDir] = fromChunk;
  }
}

void ChunkCol::unlink() {
  ChunkCol* potentialNeigh;
  potentialNeigh = _neigh[Dir_MX];
  if (potentialNeigh != NULL) {
    potentialNeigh->bye(Dir_PX, this);
    _neigh[Dir_MX] = NULL;
  }
  potentialNeigh = _neigh[Dir_PX];
  if (potentialNeigh != NULL) {
    potentialNeigh->hello(Dir_MX, this);
    _neigh[Dir_PX] = NULL;
  }

  potentialNeigh = _neigh[Dir_MY];
  if (potentialNeigh != NULL) {
    potentialNeigh->hello(Dir_PY, this);
    _neigh[Dir_MY] = NULL;
  }
  potentialNeigh = _neigh[Dir_PY];
  if (potentialNeigh != NULL) {
    potentialNeigh->hello(Dir_MY, this);
    _neigh[Dir_PY] = NULL;
  }

  for (int i = 0; i < CHUNK_PER_COLUMN; i++) {
    if (_chunks[i] != NULL) {
      _chunks[i]->unlink();
    }
  }
}
void ChunkCol::bye(Directions fromDir, ChunkCol* fromChunk) {
  _neigh[fromDir] = NULL;
}