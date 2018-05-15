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
    }
  }

  float heightmap[BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT][BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT];

  for (int i = 0; i < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; i++) {
    for (int j = 0; j < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; j++) {
      heightmap[i][j] = 20;
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
          i*1.0 / it.first,
          j*1.0 / it.first)
          * it.second;
      }
    }
  }

  for (int i = 0; i < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; i++) {
    for (int j = 0; j < BLOCK_PER_CHUNK * COLUMN_PER_FRAGMENT; j++) {
      for (int k = 0; k < heightmap[i][j]; k++) {
        res->getChunkCol(i / BLOCK_PER_CHUNK, j / BLOCK_PER_CHUNK)->getChunk(k / BLOCK_PER_CHUNK)->_blocks[i % BLOCK_PER_CHUNK][j % BLOCK_PER_CHUNK][k % BLOCK_PER_CHUNK]._ID = 1;
      }
    }
  }
}
