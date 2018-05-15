#include "WorldLoader.h"

Fragment::Fragment(int fx, int fy, int dim) :
  _fx(fx),
  _fy(fy) {
  _dim = dim;
  for (int i = 0; i < 4; i++) {
    _neigh[i] = NULL;
  }
  for (int i = 0; i < COLUMN_PER_FRAGMENT; i++) {
    for (int j = 0; j < COLUMN_PER_FRAGMENT; j++) {
      _cols[i][j] = NULL;
    }
  }
}

void Fragment::link() {

  Fragment* potentialNeigh = findFragment(_fx - 1, _fy, _dim);
  if (potentialNeigh != NULL) {
    potentialNeigh->hello(Dir_PX, this);
    _neigh[Dir_MX] = potentialNeigh;
  }

  potentialNeigh = findFragment(_fx + 1, _fy, _dim);
  if (potentialNeigh != NULL) {
    potentialNeigh->hello(Dir_MX, this);
    _neigh[Dir_PX] = potentialNeigh;
  }

  potentialNeigh = findFragment(_fx, _fy - 1, _dim);
  if (potentialNeigh != NULL) {
    potentialNeigh->hello(Dir_PY, this);
    _neigh[Dir_MY] = potentialNeigh;
  }
  potentialNeigh = findFragment(_fx, _fy + 1, _dim);
  if (potentialNeigh != NULL) {
    potentialNeigh->hello(Dir_MY, this);
    _neigh[Dir_PY] = potentialNeigh;
  }
  

  for (int i = 0; i < COLUMN_PER_FRAGMENT; i++) {
    for (int j = 0; j < COLUMN_PER_FRAGMENT; j++) {
      if (_cols[i][j] != NULL) {
        _cols[i][j]->link();
      }
    }
  }
}
void Fragment::hello(Directions fromDir, Fragment* fromChunk) {
  if (_neigh[fromDir] != fromChunk) {
    _neigh[fromDir] = fromChunk;
  }
}

void Fragment::unlink() {

  if (_neigh[Dir_MX] != NULL) {
    _neigh[Dir_MX]->bye(Dir_PX, this);
    _neigh[Dir_MX] = NULL;
  }
  if (_neigh[Dir_PX] != NULL) {
    _neigh[Dir_PX]->bye(Dir_MX, this);
    _neigh[Dir_PX] = NULL;
  }

  if (_neigh[Dir_MY] != NULL) {
    _neigh[Dir_MY]->bye(Dir_PY, this);
    _neigh[Dir_MY] = NULL;
  }
  if (_neigh[Dir_PY] != NULL) {
    _neigh[Dir_PY]->bye(Dir_MY, this);
    _neigh[Dir_PY] = NULL;
  }


  for (int i = 0; i < COLUMN_PER_FRAGMENT; i++) {
    for (int j = 0; j < COLUMN_PER_FRAGMENT; j++) {
      if (_cols[i][j] != NULL) {
        _cols[i][j]->unlink();
      }
    }
  }
}
void Fragment::bye(Directions fromDir, Fragment* fromChunk) {
  _neigh[fromDir] = NULL;
}
#ifdef M_SERVER
void Fragment::prepareFile(fstream & reg, int & fileSize) {
  fileSize = 4 * 2 * FRAGMENT_PER_REGION*FRAGMENT_PER_REGION;
  char* res = new char[fileSize];
  for (int i = 0; i < fileSize; i++) {
    res[i] = 0;
  }
  reg.write(res, fileSize);
  delete[fileSize] res;
}

void Fragment::save() {
  int rx = floorDiv(_fx, FRAGMENT_PER_REGION);
  int ry = floorDiv(_fy, FRAGMENT_PER_REGION);
  int lfx = modulo(_fx, FRAGMENT_PER_REGION);
  int lfy = modulo(_fy, FRAGMENT_PER_REGION);
  cout << "Saving frag " << _fx << " " << _fy << endl;
  cout << "File " << rx << " " << ry << endl;
  cout << "Fragment " << lfx << " " << lfy << endl;
  string fname = "dim_" + to_string(_dim) + "_" + to_string(rx) + "_" + to_string(ry) + ".map";
  fstream reg;
  reg.open(fname, ios::in | ios::out | ios::app);
  reg.seekg(0, ios::end);
  int fileSize = reg.tellg();
  reg.close();

  reg.open(fname, ios::in | ios::out | ios::binary);

  if (fileSize <= 0) {
    prepareFile(reg, fileSize);
  }

  reg.seekg(8 * (lfx*FRAGMENT_PER_REGION + lfy));

  unsigned char* rawdata = new unsigned char[6*COLUMN_PER_FRAGMENT*COLUMN_PER_FRAGMENT*CHUNK_PER_COLUMN*BLOCK_PER_CHUNK*BLOCK_PER_CHUNK*BLOCK_PER_CHUNK];

  for (int ci = 0; ci < COLUMN_PER_FRAGMENT; ci++) {
    for (int cj = 0; cj < COLUMN_PER_FRAGMENT; cj++) {
      for (int ck = 0; ck < CHUNK_PER_COLUMN; ck++) {
        for (int bi = 0; bi < BLOCK_PER_CHUNK; bi++) {
          for (int bj = 0; bj < BLOCK_PER_CHUNK; bj++) {
            for (int bk = 0; bk < BLOCK_PER_CHUNK; bk++) {
              int rawid = bk + BLOCK_PER_CHUNK * (bj + BLOCK_PER_CHUNK * (bi + BLOCK_PER_CHUNK * (ck + CHUNK_PER_COLUMN * (cj + COLUMN_PER_FRAGMENT * (ci)))));
              rawid *= 6;
              writeBlock(rawdata, rawid, _cols[ci][cj]->getChunk(ck)->_blocks[bi][bj][bk]);
            }
          }
        }
      }
    }
  }

  int pos, len;
  reg.read(reinterpret_cast<char *>(&pos), sizeof(pos));
  reg.read(reinterpret_cast<char *>(&len), sizeof(len));

  if (pos != 0) {
    if (len == 6*COLUMN_PER_FRAGMENT*COLUMN_PER_FRAGMENT*CHUNK_PER_COLUMN*BLOCK_PER_CHUNK*BLOCK_PER_CHUNK*BLOCK_PER_CHUNK) {
      reg.seekg(pos);
      reg.write(reinterpret_cast<char*>(rawdata), len);
    } else {
      //Corrupt file;
      throw 1;
    }
  } else {
    pos = fileSize;
    len = 6*COLUMN_PER_FRAGMENT*COLUMN_PER_FRAGMENT*CHUNK_PER_COLUMN*BLOCK_PER_CHUNK*BLOCK_PER_CHUNK*BLOCK_PER_CHUNK;
    reg.seekg(8 * (lfx*FRAGMENT_PER_REGION + lfy));
    reg.write(reinterpret_cast<char *>(&pos), sizeof(pos));
    reg.write(reinterpret_cast<char *>(&len), sizeof(len));
    reg.seekg(pos);
    reg.write(reinterpret_cast<char*>(rawdata), len);
  }

  delete[6*COLUMN_PER_FRAGMENT*COLUMN_PER_FRAGMENT*CHUNK_PER_COLUMN*BLOCK_PER_CHUNK*BLOCK_PER_CHUNK*BLOCK_PER_CHUNK] rawdata;
  reg.close();
}

void Fragment::load() {
  int rx = floorDiv(_fx, FRAGMENT_PER_REGION);
  int ry = floorDiv(_fy, FRAGMENT_PER_REGION);
  int lfx = modulo(_fx, FRAGMENT_PER_REGION);
  int lfy = modulo(_fy, FRAGMENT_PER_REGION);
  string fname = "dim_" + to_string(_dim) + "_" + to_string(rx) + "_" + to_string(ry) + ".map";
  ifstream reg(fname, ios::in | ios::binary);
  if (reg.good()) {
    reg.seekg(8 * (lfx*FRAGMENT_PER_REGION + lfy));
    int pos, len;
    reg.read(reinterpret_cast<char *>(&pos), sizeof(pos));
    reg.read(reinterpret_cast<char *>(&len), sizeof(len));

    if (pos != 0) {
      reg.seekg(pos);

      char* bytes = new char[len];

      reg.read(bytes, len);

      unsigned char* rawdata = reinterpret_cast<unsigned char*>(bytes);
      int rawlen = len;

      for (int ci = 0; ci < COLUMN_PER_FRAGMENT; ci++) {
        for (int cj = 0; cj < COLUMN_PER_FRAGMENT; cj++) {
          ChunkCol* ncc = new ChunkCol(_fx * CHUNK_PER_COLUMN + ci, _fy * CHUNK_PER_COLUMN + cj, this);
          for (int ck = 0; ck < CHUNK_PER_COLUMN; ck++) {
            Chunk* nc = new Chunk(COLUMN_PER_FRAGMENT*_fx + ci, COLUMN_PER_FRAGMENT*_fy + cj, ck, ncc);
            for (int bi = 0; bi < BLOCK_PER_CHUNK; bi++) {
              for (int bj = 0; bj < BLOCK_PER_CHUNK; bj++) {
                for (int bk = 0; bk < BLOCK_PER_CHUNK; bk++) {
                  int rawid = bk + BLOCK_PER_CHUNK * (bj + BLOCK_PER_CHUNK * (bi + BLOCK_PER_CHUNK * (ck + CHUNK_PER_COLUMN * (cj + COLUMN_PER_FRAGMENT * (ci)))));
                  rawid *= 6;
                  readBlock(rawdata, rawid, nc->_blocks[bi][bj][bk]);
                }
              }
            }
            ncc->setChunk(nc, ck);
          }
          setChunkCol(ci, cj, ncc);
        }
      }

      delete[len] bytes;
      link();
      return;
    }
  }
  reg.close();

  Mapgen gen;
  gen.res = this;
  gen.dim = 0;
  gen.generateFragment(_fx, _fy);
  save();
  link();
}
#endif