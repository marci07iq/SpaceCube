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

Chunk::~Chunk() {
  for (int i = 0; i < 6; i++) {
    if (_neigh[i] != NULL) {
      _neigh[i]->bye(static_cast<Directions>(i ^ 1), this);
    }
  }
#ifdef M_CLIENT
  unBuildChunk();
#endif
}

void Chunk::set(DataElement* from) {
  for (int i = 0; i < BLOCK_PER_CHUNK; i++) {
    for (int j = 0; j < BLOCK_PER_CHUNK; j++) {
      for (int k = 0; k < BLOCK_PER_CHUNK; k++) {
        int rawid = k + BLOCK_PER_CHUNK * (j + BLOCK_PER_CHUNK * i);
        readBlock(from->_core->_data, rawid * BLOCK_BYTES, _blocks[i][j][k]);
      }
    }
  }
#ifdef M_CLIENT
  _state = Chunk_ToRender;
#endif
}
void Chunk::get(DataElement* to) {
  delete to->_core;
  to->_core = new DataPair(BLOCK_PER_CHUNK *BLOCK_PER_CHUNK *BLOCK_PER_CHUNK * BLOCK_BYTES);
  for (int i = 0; i < BLOCK_PER_CHUNK; i++) {
    for (int j = 0; j < BLOCK_PER_CHUNK; j++) {
      for (int k = 0; k < BLOCK_PER_CHUNK; k++) {
        int rawid = k + BLOCK_PER_CHUNK * (j + BLOCK_PER_CHUNK * i);
        writeBlock(to->_core->_data, rawid * BLOCK_BYTES, _blocks[i][j][k]);
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

void Chunk::setBlock(int bx, int by, int bz, Block to) {
#ifdef M_CLIENT
  _blocks[bx][by][bz] = to;

  if (bx == 0 && _neigh[Dir_MX]) {
    _neigh[Dir_MX]->_state = Chunk_ToRender;
  }
  if (bx == BLOCK_PER_CHUNK - 1 && _neigh[Dir_PX]) {
    _neigh[Dir_PX]->_state = Chunk_ToRender;
  }
  if (by == 0 && _neigh[Dir_MY]) {
    _neigh[Dir_MY]->_state = Chunk_ToRender;
  }
  if (by == BLOCK_PER_CHUNK - 1 && _neigh[Dir_PY]) {
    _neigh[Dir_PY]->_state = Chunk_ToRender;
  }
  if (bz == 0 && _neigh[Dir_MZ]) {
    _neigh[Dir_MZ]->_state = Chunk_ToRender;
  }
  if (bz == BLOCK_PER_CHUNK - 1 && _neigh[Dir_PZ]) {
    _neigh[Dir_PZ]->_state = Chunk_ToRender;
  }
  _state = Chunk_ToRender;
#endif
#ifdef M_SERVER
  bool found;
  BlockPos p;
  p.b = &(_blocks[bx][by][bz]);
  p.c = this;
  p.lbx = bx;
  p.lby = by;
  p.lbz = bz;

  blockProperties[p.b->_ID].actionDestory(p);
  _blocks[bx][by][bz] = to;
  blockProperties[p.b->_ID].onUpdate(p);
  for(int l = 0; l < 6; l++) {
    BlockPos np;
    if(blockNeighbour(p, static_cast<Directions>(l), np)) {
      blockProperties[np.b->_ID].onUpdate(np);
    }
  }

  for (auto&& it : _col->_loaders) {
    if (it % 2) { //Player
      ((NetBinder*)entities[it])->sendBlock(iVec3(bx, by, bz) + iVec3(_cx, _cy, _cz) * BLOCK_PER_CHUNK, *(p.b));
    }
  }

  
#endif
}

#ifdef M_CLIENT
void Chunk::unBuildChunk() {
  if (_chunk_pos_vbo) {
    glDeleteBuffers(1, &_chunk_pos_vbo);
    _chunk_pos_vbo = 0;
  }
  if (_chunk_tex_vbo) {
    glDeleteBuffers(1, &_chunk_tex_vbo);
    _chunk_tex_vbo = 0;
  }
  if (_chunk_col_vbo) {
    glDeleteBuffers(1, &_chunk_col_vbo);
    _chunk_col_vbo = 0;
  }
  if (_chunk_mov_vbo) {
    glDeleteBuffers(1, &_chunk_mov_vbo);
    _chunk_mov_vbo = 0;
  }
  if (_chunk_vao) {
    glDeleteVertexArrays(1, &_chunk_vao);
    _chunk_vao = 0;
  }
}
void Chunk::buildChunk() {
  _state = Chunk_Rendered;

  BlockPos neighbours[7];

  list<QuadFace> quads;
  for (uint8_t i = 0; i < BLOCK_PER_CHUNK; i++) {
    for (uint8_t j = 0; j < BLOCK_PER_CHUNK; j++) {
      for (uint8_t k = 0; k < BLOCK_PER_CHUNK; k++) {
        neighbours[6] = { this, i,j,k, &(_blocks[i][j][k]) };
        BlockNeeds n = 1 << Dir_All;
        for (int l = 0; l < 6; l++) {
          bool b = blockNeighbour(neighbours[6], static_cast<Directions>(l), neighbours[l]);
          if(b) {
            n |= (1 << l) & blockProperties[neighbours[l].b->_ID].getNeeds(*neighbours[l].b);
          }
        }
        blockProperties[_blocks[i][j][k]._ID].getModel(neighbours, n, quads);
      }
    }
  }
  float* vert = new float[quads.size() * 3 * 6];
  float* tex = new float[quads.size() * 2 * 6];
  float* col = new float[quads.size() * 4 * 6];
  float* mov = new float[quads.size() * 6];
  int i = 0;
  for (auto&& it : quads) {
    vert[18 * i +  0] = it.vbl.x;
    vert[18 * i +  1] = it.vbl.y;
    vert[18 * i +  2] = it.vbl.z;
    vert[18 * i +  3] = it.vtl.x;
    vert[18 * i +  4] = it.vtl.y;
    vert[18 * i +  5] = it.vtl.z;
    vert[18 * i +  6] = it.vbr.x;
    vert[18 * i +  7] = it.vbr.y;
    vert[18 * i +  8] = it.vbr.z;
    vert[18 * i +  9] = it.vtl.x;
    vert[18 * i + 10] = it.vtl.y;
    vert[18 * i + 11] = it.vtl.z;
    vert[18 * i + 12] = it.vtr.x;
    vert[18 * i + 13] = it.vtr.y;
    vert[18 * i + 14] = it.vtr.z;
    vert[18 * i + 15] = it.vbr.x;
    vert[18 * i + 16] = it.vbr.y;
    vert[18 * i + 17] = it.vbr.z;

    tex[12 * i + 0]  = it.tbl.x;
    tex[12 * i + 1]  = it.tbl.y;
    tex[12 * i + 2]  = it.ttl.x;
    tex[12 * i + 3]  = it.ttl.y;
    tex[12 * i + 4]  = it.tbr.x;
    tex[12 * i + 5]  = it.tbr.y;
    tex[12 * i + 6]  = it.ttl.x;
    tex[12 * i + 7]  = it.ttl.y;
    tex[12 * i + 8]  = it.ttr.x;
    tex[12 * i + 9]  = it.ttr.y;
    tex[12 * i + 10] = it.tbr.x;
    tex[12 * i + 11] = it.tbr.y;

    for (int j = 0; j < 6; j++) {
      col[24 * i + 4 * j + 0] = it.recolor.r;
      col[24 * i + 4 * j + 1] = it.recolor.g;
      col[24 * i + 4 * j + 2] = it.recolor.b;
      col[24 * i + 4 * j + 3] = it.recolor.a;
    }

    mov[6 * i + 0] = it.rbl;
    mov[6 * i + 1] = it.rtl;
    mov[6 * i + 2] = it.rbr;
    mov[6 * i + 3] = it.rtl;
    mov[6 * i + 4] = it.rtr;
    mov[6 * i + 5] = it.rbr;

    ++i;
  }

  unBuildChunk();

  glGenBuffers(1, &_chunk_pos_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_pos_vbo);
  glBufferData(GL_ARRAY_BUFFER, quads.size() * 3 * 6 * sizeof(float), vert, GL_STATIC_DRAW);

  glGenBuffers(1, &_chunk_tex_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_tex_vbo);
  glBufferData(GL_ARRAY_BUFFER, quads.size() * 2 * 6 * sizeof(float), tex, GL_STATIC_DRAW);

  glGenBuffers(1, &_chunk_col_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_col_vbo);
  glBufferData(GL_ARRAY_BUFFER, quads.size() * 4 * 6 * sizeof(float), col, GL_STATIC_DRAW);

  glGenBuffers(1, &_chunk_mov_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_mov_vbo);
  glBufferData(GL_ARRAY_BUFFER, quads.size() * 6 * sizeof(float), mov, GL_STATIC_DRAW);

  glGenVertexArrays(1, &_chunk_vao);
  glBindVertexArray(_chunk_vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_pos_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_tex_vbo);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_col_vbo);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(3);
  glBindBuffer(GL_ARRAY_BUFFER, _chunk_mov_vbo);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, NULL);

  delete[quads.size() * 3 * 6] vert;
  delete[quads.size() * 2 * 6] tex;
  delete[quads.size() * 4 * 6] col;
  delete[quads.size() * 6] mov;

  _quads = quads.size()*6;
}
#endif