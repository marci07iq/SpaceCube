#include "../Terrain/WorldLoader.h"
#include "Mapgen.h"

list<pair<int, double>> Mapgen::noise_weights;

Block& Mapgen::getBlock(iVec3 lplace) {
  return res->getChunkCol(
  lplace.x / BLOCK_PER_CHUNK,
  lplace.y / BLOCK_PER_CHUNK
  )->getChunk(
  lplace.z / BLOCK_PER_CHUNK
  )->_blocks
  [lplace.x % BLOCK_PER_CHUNK]
  [lplace.y % BLOCK_PER_CHUNK]
  [lplace.z % BLOCK_PER_CHUNK];
}

bool Mapgen::isIn(iVec3 & what) {
  return 0 <= what.x && 0 <= what.y && 0 <= what.z && what.x < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT && what.y < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT && what.z < BLOCK_PER_CHUNK * CHUNK_PER_COLUMN;
}

void Mapgen::buildTree(iVec3 lbase) {
  list<pair<iVec3, pair<int, int>>> steps; //pos, size, from
  steps.push_back({lbase,{5, Dir_MZ}});
  while (steps.size()) {
    auto it = steps.front();
    steps.pop_front();

    if (isIn(it.first) && it.second.first > 0) {
      Block& b = getBlock(it.first);
      if (b._ID == 0) {
        b._ID = 6;
        b._meta = min(it.second.first, 3) << (2*it.second.second);
        if (it.second.second == Dir_MZ) { //if up branch
          int nw = round(it.second.first - ran1());
          nw = max(nw, 0);
          steps.push_back({jump(it.first, Dir_PZ), {nw, Dir_MZ}});
          b._meta |= min(nw, 3) << (2 * Dir_PZ);
          if (ran1() < 1.0 / (1 - 0.25 * it.second.first)) {
            int nw = round(it.second.first * ran1());
            nw = max(nw, 0);
            steps.push_back({ jump(it.first, Dir_PX),{ nw, Dir_MX } });
            b._meta |= min(nw, 3) << (2 * Dir_PX);
          }
          if (ran1() < 1.0 / (1 - 0.25 * it.second.first)) {
            int nw = round(it.second.first * ran1());
            nw = max(nw, 0);
            steps.push_back({ jump(it.first, Dir_MX),{ nw, Dir_PX } });
            b._meta |= min(nw, 3) << (2 * Dir_MX);
          }
          if (ran1() < 1.0 / (1 - 0.25 * it.second.first)) {
            int nw = round(it.second.first * ran1());
            nw = max(nw, 0);
            steps.push_back({ jump(it.first, Dir_PY),{ nw, Dir_MY } });
            b._meta |= min(nw, 3) << (2 * Dir_PY);
          }
          if (ran1() < 1.0 / (1 - 0.25 * it.second.first)) {
            int nw = round(it.second.first * ran1());
            nw = max(nw, 0);
            steps.push_back({ jump(it.first, Dir_MY),{ nw, Dir_PY } });
            b._meta |= min(nw, 3) << (2 * Dir_MY);
          }
        }
        if (it.second.second < 4) { //if side branch
          int nw = max(0.0, floor(it.second.first - 1.5*ran1()));
          nw = max(nw, 0);
          steps.push_back({ jump(it.first, it.second.second ^ 1),{ nw, it.second.second } });
          b._meta |= min(nw, 3) << (2 * (it.second.second ^ 1));
          if (ran1() < 1.0 / (1 - 0.25 * it.second.first)) {
            int nw = round(it.second.first * ran1());
            nw = max(nw, 0);
            steps.push_back({ jump(it.first, it.second.second ^ 2),{ nw, it.second.second ^ 3 } });
            b._meta |= min(nw, 3) << (2 * (it.second.second ^ 2));
          }
          if (ran1() < 1.0 / (1 - 0.25 * it.second.first)) {
            int nw = round(it.second.first * ran1());
            nw = max(nw, 0);
            steps.push_back({ jump(it.first, it.second.second ^ 3),{ nw, it.second.second ^ 2 } });
            b._meta |= min(nw, 3) << (2 * (it.second.second ^ 3));
          }
        }
      }
    }
  }
}

void Mapgen::generateFragment(int fx, int fy) {
  for (int i = 0; i < COLUMN_PER_FRAGMENT; i++) {
    for (int j = 0; j < COLUMN_PER_FRAGMENT; j++) {
      ChunkCol* ncc = new ChunkCol(COLUMN_PER_FRAGMENT * fx + i, COLUMN_PER_FRAGMENT * fy + j, res);
      for (int z = 0; z < CHUNK_PER_COLUMN; z++) {
        Chunk* nc = new Chunk(COLUMN_PER_FRAGMENT * fx + i, COLUMN_PER_FRAGMENT * fy + j, z, ncc);
        ncc->setChunk(nc, z);
      }
      res->setChunkCol(i, j, ncc);
    }
  }

  CompositePerlin heightMap;
  heightMap._parts = noise_weights;

  heightMap.generate(
  {fx * COLUMN_PER_FRAGMENT * BLOCK_PER_CHUNK, fy * COLUMN_PER_FRAGMENT * BLOCK_PER_CHUNK},
  {1,1},
  { COLUMN_PER_FRAGMENT * BLOCK_PER_CHUNK, COLUMN_PER_FRAGMENT * BLOCK_PER_CHUNK },
  0,
  dim,
  0);

  vector<vector<float>>& heightmap = (heightMap._vals);

  for (int i = 0; i < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; i++) {
    for (int j = 0; j < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; j++) {
      for (int k = 0; k <= heightmap[i][j] - 4 && k < BLOCK_PER_CHUNK * CHUNK_PER_COLUMN; k++) {
        getBlock({i, j, k}) =  Block(1);
      }
      if(heightMap._vals[i][j] > 61) {
        for (int k = max(0.0f,heightmap[i][j] - 3); k <= heightmap[i][j] - 1 && k < BLOCK_PER_CHUNK * CHUNK_PER_COLUMN; k++) {
          getBlock({ i, j, k }) = Block(2);
        }
        for (int k = max(0.0f, heightmap[i][j]); k <= heightmap[i][j] && k < BLOCK_PER_CHUNK * CHUNK_PER_COLUMN; k++) {
          getBlock({ i, j, k }) = Block(3);
        }
      } else {
        for (int k = max(0.0f, heightmap[i][j] - 3); k <= heightmap[i][j] && k < BLOCK_PER_CHUNK * CHUNK_PER_COLUMN; k++) {
          getBlock({ i, j, k }) = Block(1);
        }
      }
      for (int k = max(0.0f, heightmap[i][j] + 1); k <= 60; k++) {
        getBlock({ i, j, k }) = Block(4);
      }
    }
  }

  for (int i = 0; i < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; i++) {
    for (int j = 0; j < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; j++) {
      int k = heightmap[i][j] + 1;

      //Flower
      if(getBlock({i, j, k-1})._ID == 3) {
        float val = locationRandom(0, 1, 0, i + fx * BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT, j + fy * BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT, 0);
        if (val > 0.98) {
          getBlock({ i, j, k }) = Block(5);
        }

        //Tree
        val = locationRandom(0, 1, 0, i + fx * BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT, j + fy * BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT, 1);
        if (val > 0.99) {
          //buildTree({i, j, k});
        }
      }
    }
  }
}
