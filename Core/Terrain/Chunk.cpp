#include "WorldLoader.h"

Chunk::Chunk(int cx, int cy, int cz, ChunkCol* col) {
  _cx = cx;
  _cy = cy;
  _cz = cz;
  _state = Chunk_NULL;
  for (int i = 0; i < 6; i++) {
    _neigh[i] = NULL;
  }
  _col = col;
}

void Chunk::set(DataElement* from) {
  for (int i = 0; i < BLOCK_PER_CHUNK; i++) {
    for (int j = 0; j < BLOCK_PER_CHUNK; j++) {
      for (int k = 0; k < BLOCK_PER_CHUNK; k++) {
        int rawid = k + BLOCK_PER_CHUNK * (j + BLOCK_PER_CHUNK * i);
        readBlock(from->_core->_data, rawid * 6, _blocks[i][j][k]);
      }
    }
  }
#ifdef M_CLIENT
  _state = Chunk_ToRender;
#endif
}
void Chunk::get(DataElement* to) {
  delete to->_core;
  to->_core = new DataPair(BLOCK_PER_CHUNK *BLOCK_PER_CHUNK *BLOCK_PER_CHUNK *6);
  for (int i = 0; i < BLOCK_PER_CHUNK; i++) {
    for (int j = 0; j < BLOCK_PER_CHUNK; j++) {
      for (int k = 0; k < BLOCK_PER_CHUNK; k++) {
        int rawid = k + BLOCK_PER_CHUNK * (j + BLOCK_PER_CHUNK * i);
        writeBlock(to->_core->_data, rawid * 6, _blocks[i][j][k]);
      }
    }
  }
}

void Chunk::link() {
  if (_col->_neigh[Dir_MX] != NULL) {
    Chunk* potentialNeigh = _col->_neigh[Dir_MX]->getChunk(_cz);
    if (potentialNeigh != NULL) {
      _neigh[Dir_MX] = potentialNeigh;
      potentialNeigh->hello(Dir_PX, this);
    }
  }
  if (_col->_neigh[Dir_PX] != NULL) {
    Chunk* potentialNeigh = _col->_neigh[Dir_PX]->getChunk(_cz);
    if (potentialNeigh != NULL) {
      _neigh[Dir_PX] = potentialNeigh;
      potentialNeigh->hello(Dir_MX, this);
    }
  }

  if (_col->_neigh[Dir_MY] != NULL) {
    Chunk* potentialNeigh = _col->_neigh[Dir_MY]->getChunk(_cz);
    if (potentialNeigh != NULL) {
      _neigh[Dir_MY] = potentialNeigh;
      potentialNeigh->hello(Dir_PY, this);
    }
  }
  if (_col->_neigh[Dir_PY] != NULL) {
    Chunk* potentialNeigh = _col->_neigh[Dir_PY]->getChunk(_cz);
    if (potentialNeigh != NULL) {
      _neigh[Dir_PY] = potentialNeigh;
      potentialNeigh->hello(Dir_MY, this);
    }
  }

  if (0 < _cz) {
    Chunk* potentialNeigh = _col->getChunk(_cz - 1);
    if (potentialNeigh != NULL) {
      _neigh[Dir_MZ] = potentialNeigh;
      potentialNeigh->hello(Dir_PZ, this);
    }
  }
  if (_cz < CHUNK_PER_COLUMN - 1) {
    Chunk* potentialNeigh = _col->getChunk(_cz + 1);
    if (potentialNeigh != NULL) {
      _neigh[Dir_PZ] = potentialNeigh;
      potentialNeigh->hello(Dir_MZ, this);
    }
  }

  _state = Chunk_Linked;
#ifdef M_CLIENT
  _state = Chunk_ToRender;
#endif
}
void Chunk::hello(Directions fromDir, Chunk* fromChunk) {
  if(_neigh[fromDir] != fromChunk) {
    _neigh[fromDir] = fromChunk;
#ifdef M_CLIENT
     _state = Chunk_ToRender;
#endif
  }
}

void Chunk::unlink() {
  Chunk* potentialNeigh;
  potentialNeigh = _neigh[Dir_MX];
  if (potentialNeigh != NULL) {
    potentialNeigh->bye(Dir_PX, this);
    _neigh[Dir_MX] = NULL;
  }
  potentialNeigh = _neigh[Dir_PX];
  if (potentialNeigh != NULL) {
    potentialNeigh->bye(Dir_MX, this);
    _neigh[Dir_PX] = NULL;
  }
 
  potentialNeigh = _neigh[Dir_MY];
  if (potentialNeigh != NULL) {
    potentialNeigh->bye(Dir_PY, this);
    _neigh[Dir_MY] = NULL;
  }
  potentialNeigh = _neigh[Dir_PY];
  if (potentialNeigh != NULL) {
    potentialNeigh->bye(Dir_MY, this);
    _neigh[Dir_PY] = NULL;
  }
  


  potentialNeigh = _neigh[Dir_MZ];
  if (potentialNeigh != NULL) {
    potentialNeigh->bye(Dir_PZ, this);
    _neigh[Dir_MZ] = NULL;
  }
  potentialNeigh = _neigh[Dir_PZ];
  if (potentialNeigh != NULL) {
    potentialNeigh->bye(Dir_MZ, this);
    _neigh[Dir_PZ] = NULL;
  }

  _state = Chunk_Unlinked;
#ifdef M_CLIENT
  unBuildChunk();
#endif
}
void Chunk::bye(Directions fromDir, Chunk* fromChunk) {
  _neigh[fromDir] = NULL;
}

#ifdef M_CLIENT
void Chunk::unBuildChunk() {
  if (_chunk_pos_vbo) {
    glDeleteBuffers(1, &_chunk_pos_vbo);
    _chunk_pos_vbo = 0;
  }
  if (_chunk_col_vbo) {
    glDeleteBuffers(1, &_chunk_col_vbo);
    _chunk_col_vbo = 0;
  }
  if (_chunk_vao) {
    glDeleteVertexArrays(1, &_chunk_vao);
    _chunk_vao = 0;
  }
}
void Chunk::buildChunk() {
  _state = Chunk_Rendered;

  list<pair<QuadFace, iVec3>> quads;
  for (uint8_t i = 0; i < BLOCK_PER_CHUNK; i++) {
    for (uint8_t j = 0; j < BLOCK_PER_CHUNK; j++) {
      for (uint8_t k = 0; k < BLOCK_PER_CHUNK; k++) {
        BlockNeeds n = getNeed(i, j, k);
        BlockModel& m = blockProperties[_blocks[i][j][k]._ID].getModel(_blocks[i][j][k]);
        for (auto&& it : m.faces) {
          if (it.type & n) {
            quads.push_back({it, {i + BLOCK_PER_CHUNK * _cx, j + BLOCK_PER_CHUNK * _cy, k + BLOCK_PER_CHUNK * _cz }});
          }
        }
      }
    }
  }
  float* vert = new float[quads.size() * 3*6];
  float* col = new float[quads.size() * 4*6];
  int i = 0;
  for (auto&& it : quads) {
    vert[18 * i +  0] = it.first.vbl.x + it.second.x;
    vert[18 * i +  1] = it.first.vbl.y + it.second.y;
    vert[18 * i +  2] = it.first.vbl.z + it.second.z;
    vert[18 * i +  3] = it.first.vtl.x + it.second.x;
    vert[18 * i +  4] = it.first.vtl.y + it.second.y;
    vert[18 * i +  5] = it.first.vtl.z + it.second.z;
    vert[18 * i +  6] = it.first.vbr.x + it.second.x;
    vert[18 * i +  7] = it.first.vbr.y + it.second.y;
    vert[18 * i +  8] = it.first.vbr.z + it.second.z;
    vert[18 * i +  9] = it.first.vtl.x + it.second.x;
    vert[18 * i + 10] = it.first.vtl.y + it.second.y;
    vert[18 * i + 11] = it.first.vtl.z + it.second.z;
    vert[18 * i + 12] = it.first.vtr.x + it.second.x;
    vert[18 * i + 13] = it.first.vtr.y + it.second.y;
    vert[18 * i + 14] = it.first.vtr.z + it.second.z;
    vert[18 * i + 15] = it.first.vbr.x + it.second.x;
    vert[18 * i + 16] = it.first.vbr.y + it.second.y;
    vert[18 * i + 17] = it.first.vbr.z + it.second.z;

    for (int j = 0; j < 24; j += 4) {
      col[24 * i + j + 0] = it.first.recolor.r;
      col[24 * i + j + 1] = it.first.recolor.g;
      col[24 * i + j + 2] = it.first.recolor.b;
      col[24 * i + j + 3] = it.first.recolor.a;
    }
    ++i;
  }

  unBuildChunk();

  glGenBuffers(1, &_chunk_pos_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_pos_vbo);
  glBufferData(GL_ARRAY_BUFFER, quads.size() * 3 * 6 * sizeof(float), vert, GL_STATIC_DRAW);
  

  glGenBuffers(1, &_chunk_col_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_col_vbo);
  glBufferData(GL_ARRAY_BUFFER, quads.size() * 4 * 6 * sizeof(float), col, GL_STATIC_DRAW);


  glGenVertexArrays(1, &_chunk_vao);
  glBindVertexArray(_chunk_vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_pos_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_col_vbo);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

  _quads = quads.size()*6;
}
#endif