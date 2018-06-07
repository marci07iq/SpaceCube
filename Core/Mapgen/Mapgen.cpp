#include "Mapgen.h"

list<pair<int, double>> Mapgen::noise_weights;

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

  float heightmap[BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT][BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT];

  for (int i = 0; i < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; i++) {
    for (int j = 0; j < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; j++) {
      heightmap[i][j] = 70;
    }
  }

  for (auto&& it : noise_weights) {
    Perlin2D perlin(
      ceilDiv(COLUMN_PER_FRAGMENT * BLOCK_PER_CHUNK, it.first),
      ceilDiv(COLUMN_PER_FRAGMENT * BLOCK_PER_CHUNK, it.first),
      floorDiv(fx * COLUMN_PER_FRAGMENT * BLOCK_PER_CHUNK, it.first),
      floorDiv(fy * COLUMN_PER_FRAGMENT * BLOCK_PER_CHUNK, it.first));
    perlin.setSeed(it.first, 0);
    for (int i = 0; i < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; i++) {
      for (int j = 0; j < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; j++) {
        heightmap[i][j] += perlin.getAt(
          (i + fx * COLUMN_PER_FRAGMENT * BLOCK_PER_CHUNK - perlin._xo * it.first)*1.0 / it.first,
          (j + fy * COLUMN_PER_FRAGMENT * BLOCK_PER_CHUNK - perlin._yo * it.first)*1.0 / it.first)
          * it.second;
      }
    }
  }

  for (int i = 0; i < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; i++) {
    for (int j = 0; j < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; j++) {
      for (int k = 0; k <= heightmap[i][j] - 4 && k < BLOCK_PER_CHUNK * CHUNK_PER_COLUMN; k++) {
        res->getChunkCol(i / BLOCK_PER_CHUNK, j / BLOCK_PER_CHUNK)->getChunk(k / BLOCK_PER_CHUNK)->_blocks[i % BLOCK_PER_CHUNK][j % BLOCK_PER_CHUNK][k % BLOCK_PER_CHUNK]._ID = 1;
      }
      if(heightmap[i][j] > 61) {
        for (int k = max(0.0f,heightmap[i][j] - 3); k <= heightmap[i][j] - 1 && k < BLOCK_PER_CHUNK * CHUNK_PER_COLUMN; k++) {
          res->getChunkCol(i / BLOCK_PER_CHUNK, j / BLOCK_PER_CHUNK)->getChunk(k / BLOCK_PER_CHUNK)->_blocks[i % BLOCK_PER_CHUNK][j % BLOCK_PER_CHUNK][k % BLOCK_PER_CHUNK]._ID = 2;
        }
        for (int k = max(0.0f, heightmap[i][j]); k <= heightmap[i][j] && k < BLOCK_PER_CHUNK * CHUNK_PER_COLUMN; k++) {
          res->getChunkCol(i / BLOCK_PER_CHUNK, j / BLOCK_PER_CHUNK)->getChunk(k / BLOCK_PER_CHUNK)->_blocks[i % BLOCK_PER_CHUNK][j % BLOCK_PER_CHUNK][k % BLOCK_PER_CHUNK]._ID = 3;
        }
        if(fmodf(heightmap[i][j],0.01035165) > 0.01) {
          int k = heightmap[i][j] + 1;
          res->getChunkCol(i / BLOCK_PER_CHUNK, j / BLOCK_PER_CHUNK)->getChunk(k / BLOCK_PER_CHUNK)->_blocks[i % BLOCK_PER_CHUNK][j % BLOCK_PER_CHUNK][k % BLOCK_PER_CHUNK]._ID = 5;
        }
      } else {
        for (int k = max(0.0f, heightmap[i][j] - 3); k <= heightmap[i][j] && k < BLOCK_PER_CHUNK * CHUNK_PER_COLUMN; k++) {
          res->getChunkCol(i / BLOCK_PER_CHUNK, j / BLOCK_PER_CHUNK)->getChunk(k / BLOCK_PER_CHUNK)->_blocks[i % BLOCK_PER_CHUNK][j % BLOCK_PER_CHUNK][k % BLOCK_PER_CHUNK]._ID = 1;
        }
      }
      for (int k = max(0.0f, heightmap[i][j] + 1); k <= 60; k++) {
        res->getChunkCol(i / BLOCK_PER_CHUNK, j / BLOCK_PER_CHUNK)->getChunk(k / BLOCK_PER_CHUNK)->_blocks[i % BLOCK_PER_CHUNK][j % BLOCK_PER_CHUNK][k % BLOCK_PER_CHUNK]._ID = 4;
      }
    }
  }
}
