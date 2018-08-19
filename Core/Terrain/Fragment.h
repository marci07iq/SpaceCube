#pragma once

#include "ChunkColumn.h"

struct FragmentMeta {

};

class Fragment {
  int _fx, _fy;
  int _dim;
  ChunkCol* _cols[COLUMN_PER_FRAGMENT][COLUMN_PER_FRAGMENT];
public:
  Fragment* _neigh[4];

  Fragment() {
    for (int i = 0; i < COLUMN_PER_FRAGMENT; i++) {
      for (int j = 0; j < COLUMN_PER_FRAGMENT; j++) {
        _cols[i][j] = NULL;
      }
    }
  }

  void link();
  void hello(Directions fromDir, Fragment* fromChunk);
  void unlink();
  void bye(Directions fromDir, Fragment* fromChunk);

  ChunkCol* getChunkCol(int lccx, int lccy) {
    return _cols[lccx][lccy];
  }

  void setChunkCol(int lccx, int lccy, ChunkCol* cc) {
    _cols[lccx][lccy] = cc;
  }

  Fragment(int fx, int fy, int dim);

  const int& dim() {
    return _dim;
  }
  const int& fx() {
    return _fx;
  }
  const int& fy() {
    return _fy;
  }


#ifdef M_SERVER
  void prepareFile(fstream& reg, int& fileSize);

  void save();
 
  void load();
#endif
};