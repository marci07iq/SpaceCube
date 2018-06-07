#pragma once

#include "../Physics/Physics.h"
class ChunkCol;

enum ChunkState {
  Chunk_NULL = 0,
  Chunk_Loaded = 1,
  Chunk_Linked = 2,
  Chunk_ToRender = 3,
  Chunk_Rendered = 4,
  Chunk_ToUnRender = 5,
  Chunk_UnRendered = 6,
  Chunk_Unlinked = 7
};

class Chunk {
  Chunk* _neigh[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
public:
  int _cx, _cy, _cz;
  Block _blocks[BLOCK_PER_CHUNK][BLOCK_PER_CHUNK][BLOCK_PER_CHUNK];
  ChunkCol* _col;
  ChunkState _state;
  Chunk(int cx, int cy, int cz, ChunkCol* col);
  ~Chunk();

  void set(DataElement* from);
  void get(DataElement* to);

  void link();
  void hello(Directions fromDir, Chunk* fromChunk);

  void unlink();
  void bye(Directions fromDir, Chunk* fromChunk);

  Chunk* getNeigh(Directions dir) {
    return _neigh[dir];
  }

  void setBlock(int bx, int by, int bz, Block to);

#ifdef M_CLIENT
  GLuint _chunk_vao;
  GLuint _chunk_pos_vbo;
  GLuint _chunk_tex_vbo;
  GLuint _chunk_col_vbo;
  GLuint _chunk_mov_vbo;
  int _quads;

  inline uint8_t getNeed(uint8_t x, uint8_t y, uint8_t z) {
    uint8_t res = 1 << Dir_All;
    if (0 < x) {
      res |= (1 << Dir_MX) & blockProperties[_blocks[x - 1][y][z]._ID]._ptr->_val.getNeeds(_blocks[x - 1][y][z]);
    } else if (_neigh[Dir_MX]) {
      res |= (1 << Dir_MX) & blockProperties[_neigh[Dir_MX]->_blocks[BLOCK_PER_CHUNK - 1][y][z]._ID]._ptr->_val.getNeeds(_neigh[Dir_MX]->_blocks[BLOCK_PER_CHUNK - 1][y][z]);
    }

    if (x < BLOCK_PER_CHUNK - 1) {
      res |= (1 << Dir_PX) & blockProperties[_blocks[x + 1][y][z]._ID]._ptr->_val.getNeeds(_blocks[x + 1][y][z]);
    } else if (_neigh[Dir_PX]) {
      res |= (1 << Dir_PX) & blockProperties[_neigh[Dir_PX]->_blocks[0][y][z]._ID]._ptr->_val.getNeeds(_neigh[Dir_PX]->_blocks[0][y][z]);
    }

    if (0 < y) {
      res |= (1 << Dir_MY) & blockProperties[_blocks[x][y - 1][z]._ID]._ptr->_val.getNeeds(_blocks[x][y - 1][z]);
    } else if (_neigh[Dir_MY]) {
      res |= (1 << Dir_MY) & blockProperties[_neigh[Dir_MY]->_blocks[x][BLOCK_PER_CHUNK - 1][z]._ID]._ptr->_val.getNeeds(_neigh[Dir_MY]->_blocks[x][BLOCK_PER_CHUNK - 1][z]);
    }

    if (y < BLOCK_PER_CHUNK - 1) {
      res |= (1 << Dir_PY) & blockProperties[_blocks[x][y + 1][z]._ID]._ptr->_val.getNeeds(_blocks[x][y + 1][z]);
    } else if (_neigh[Dir_PY]) {
      res |= (1 << Dir_PY) & blockProperties[_neigh[Dir_PY]->_blocks[x][0][z]._ID]._ptr->_val.getNeeds(_neigh[Dir_PY]->_blocks[x][0][z]);
    }

    if (0 < z) {
      res |= (1 << Dir_MZ) & blockProperties[_blocks[x][y][z - 1]._ID]._ptr->_val.getNeeds(_blocks[x][y][z - 1]);
    } else if (_neigh[Dir_MZ]) {
      res |= (1 << Dir_MZ) & blockProperties[_neigh[Dir_MZ]->_blocks[x][y][BLOCK_PER_CHUNK - 1]._ID]._ptr->_val.getNeeds(_neigh[Dir_MZ]->_blocks[x][y][BLOCK_PER_CHUNK - 1]);
    }

    if (z < BLOCK_PER_CHUNK - 1) {
      res |= (1 << Dir_PZ) & blockProperties[_blocks[x][y][z + 1]._ID]._ptr->_val.getNeeds(_blocks[x][y][z + 1]);
    } else if (_neigh[Dir_PZ]) {
      res |= (1 << Dir_PZ) & blockProperties[_neigh[Dir_PZ]->_blocks[x][y][0]._ID]._ptr->_val.getNeeds(_neigh[Dir_PZ]->_blocks[x][y][0]);
    }

    return res;
  }

  void unBuildChunk();
  void buildChunk();
#endif
};