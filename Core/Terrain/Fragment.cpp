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
vector<pair<int, int>> loadHeader(fstream & reg) {
  reg.seekg(0);
  vector<pair<int, int>> res(COLUMN_PER_FRAGMENT * COLUMN_PER_FRAGMENT);
  for (int i = 0; i < COLUMN_PER_FRAGMENT * COLUMN_PER_FRAGMENT; i++) {
    reg.read(reinterpret_cast<char *>(&res[i].first), sizeof(res[i].first));
    reg.read(reinterpret_cast<char *>(&res[i].second), sizeof(res[i].second));
  }
  return res;
}
int allocatePos(vector<pair<int, int>>& header, int len) {
  map<int, int> segs;
  segs[0] = 1;
  segs[COLUMN_PER_FRAGMENT * COLUMN_PER_FRAGMENT * 8] = -1;
  for (int i = 0; i < COLUMN_PER_FRAGMENT * COLUMN_PER_FRAGMENT; i++) {
    segs[header[i].first]++;
    segs[header[i].first + header[i].second]--;
  }

  int lastid = 0;
  int in = 0;
  for (auto&& it : segs) {
    if(in == 0) {
      if (it.first - lastid > len) {
        return lastid;
      }
    }
    if (in != 0) {
      lastid = it.first;
    }
    in += it.second;
  }
  return lastid;
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

  //Read file size
  reg.open(fname, ios::in | ios::out | ios::app);
  reg.seekg(0, ios::end);
  int fileSize = reg.tellg();
  reg.close();

  reg.open(fname, ios::in | ios::out | ios::binary);
  
  //Create empty header if needed
  if (fileSize <= 0) {
    prepareFile(reg, fileSize);
  }

  //Encode data
  DataPair* in = new DataPair(8*COLUMN_PER_FRAGMENT*COLUMN_PER_FRAGMENT*CHUNK_PER_COLUMN*BLOCK_PER_CHUNK*BLOCK_PER_CHUNK*BLOCK_PER_CHUNK);

  for (int ci = 0; ci < COLUMN_PER_FRAGMENT; ci++) {
    for (int cj = 0; cj < COLUMN_PER_FRAGMENT; cj++) {
      for (int ck = 0; ck < CHUNK_PER_COLUMN; ck++) {
        for (int bi = 0; bi < BLOCK_PER_CHUNK; bi++) {
          for (int bj = 0; bj < BLOCK_PER_CHUNK; bj++) {
            for (int bk = 0; bk < BLOCK_PER_CHUNK; bk++) {
              int rawid = bk + BLOCK_PER_CHUNK * (bj + BLOCK_PER_CHUNK * (bi + BLOCK_PER_CHUNK * (ck + CHUNK_PER_COLUMN * (cj + COLUMN_PER_FRAGMENT * (ci)))));
              rawid *= 8;
              writeBlock(in->_data, rawid, _cols[ci][cj]->getChunk(ck)->_blocks[bi][bj][bk]);
            }
          }
        }
      }
    }
  }

  //Compress data
  DataPair** out = new DataPair*;
  compress(in, out);
  delete in;

  //0 out this fragment data
  reg.seekg(8 * (lfx*FRAGMENT_PER_REGION + lfy));
  for(int i = 0; i < 8; i++) {
    reg.write("", 1);
  }
  
  //Allocate new position
  vector<pair<int, int>> header = loadHeader(reg);
  int len = (*out)->_len;
  int pos = allocatePos(header, len);
  
  //Set new position
  reg.seekg(8 * (lfx*FRAGMENT_PER_REGION + lfy));
  reg.write(reinterpret_cast<char *>(&pos), sizeof(pos));
  reg.write(reinterpret_cast<char *>(&len), sizeof(len));
  
  //Write data
  reg.seekg(pos);
  reg.write(reinterpret_cast<char*>((*out)->_data), len);

  //Cleanup
  delete out;
  reg.close();
}

void Fragment::load() {
  int rx = floorDiv(_fx, FRAGMENT_PER_REGION);
  int ry = floorDiv(_fy, FRAGMENT_PER_REGION);
  int lfx = modulo(_fx, FRAGMENT_PER_REGION);
  int lfy = modulo(_fy, FRAGMENT_PER_REGION);
  string fname = "dim_" + to_string(_dim) + "_" + to_string(rx) + "_" + to_string(ry) + ".map";
  ifstream reg(fname, ios::in | ios::binary);

  //If file exists
  if (reg.good()) {
    //Load header data
    int pos, len;
    reg.seekg(8 * (lfx*FRAGMENT_PER_REGION + lfy));
    reg.read(reinterpret_cast<char *>(&pos), sizeof(pos));
    reg.read(reinterpret_cast<char *>(&len), sizeof(len));

    //If exists, load
    if (pos != 0) {

      //Read compressed
      DataPair* in = new DataPair(len);
      reg.seekg(pos);
      reg.read(reinterpret_cast<char*>(in->_data), in->_len);

      //Decode
      DataPair** out = new DataPair*;
      decompress(in, out);
      delete in;

      //Write blocks
      for (int ci = 0; ci < COLUMN_PER_FRAGMENT; ci++) {
        for (int cj = 0; cj < COLUMN_PER_FRAGMENT; cj++) {
          ChunkCol* ncc = new ChunkCol(_fx * CHUNK_PER_COLUMN + ci, _fy * CHUNK_PER_COLUMN + cj, this);
          for (int ck = 0; ck < CHUNK_PER_COLUMN; ck++) {
            Chunk* nc = new Chunk(COLUMN_PER_FRAGMENT*_fx + ci, COLUMN_PER_FRAGMENT*_fy + cj, ck, ncc);
            for (int bi = 0; bi < BLOCK_PER_CHUNK; bi++) {
              for (int bj = 0; bj < BLOCK_PER_CHUNK; bj++) {
                for (int bk = 0; bk < BLOCK_PER_CHUNK; bk++) {
                  int rawid = bk + BLOCK_PER_CHUNK * (bj + BLOCK_PER_CHUNK * (bi + BLOCK_PER_CHUNK * (ck + CHUNK_PER_COLUMN * (cj + COLUMN_PER_FRAGMENT * (ci)))));
                  rawid *= 8;
                  readBlock((*out)->_data, rawid, nc->_blocks[bi][bj][bk]);
                }
              }
            }
            ncc->setChunk(nc, ck);
          }
          setChunkCol(ci, cj, ncc);
        }
      }

      //Clean up
      delete out;
      link();
      return;
    }
  }

  //Clean up
  reg.close();

  Mapgen gen;
  gen.res = this;
  gen.dim = 0;
  gen.generateFragment(_fx, _fy);
  save();
  link();
}
#endif