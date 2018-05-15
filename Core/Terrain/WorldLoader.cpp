#include "WorldLoader.h"

shared_mutex _fragmentsLock;
map<world_col_t, Fragment*> _fragments;

Fragment * findFragment(int xf, int yf, int dim) {
  _fragmentsLock.lock_shared();
  Fragment* f = _fragments[make_pair(dim, make_pair(xf, yf))];
  _fragmentsLock.unlock_shared();
  return f;
}

void addFragment(Fragment * frag) {
  _fragmentsLock.lock();
  _fragments[make_pair(frag->dim(), make_pair(frag->fx(), frag->fy()))] = frag;
  _fragmentsLock.unlock();
}

ChunkCol * findChunkCol(int xc, int yc, int dim) {
  Fragment* f = findFragment(
    floorDiv(xc, COLUMN_PER_FRAGMENT),
    floorDiv(yc, COLUMN_PER_FRAGMENT),
    dim);
  if (f == NULL) {
    return NULL;
  }
  return f->getChunkCol(
  modulo(xc, COLUMN_PER_FRAGMENT),
  modulo(yc, COLUMN_PER_FRAGMENT));
}

Chunk * findChunk(int xc, int yc, int zc, int dim) {
  ChunkCol* cc = findChunkCol(xc, yc, dim);
  if (cc == NULL) {
    return NULL;
  }
  return cc->getChunk(zc);
}

Block & getBlock(int xb, int yb, int zb, int dim) {
  return findChunk(
    floorDiv(xb, BLOCK_PER_CHUNK),
    floorDiv(yb, BLOCK_PER_CHUNK),
    floorDiv(zb, BLOCK_PER_CHUNK),
    dim)->_blocks
    [modulo(xb, BLOCK_PER_CHUNK)]
  [modulo(yb, BLOCK_PER_CHUNK)]
  [modulo(zb, BLOCK_PER_CHUNK)];
}

#ifdef M_CLIENT
void setChunk(int cx, int cy, int cz, int dim, DataElement * data) {
  int fx = floorDiv(cx, COLUMN_PER_FRAGMENT);
  int fy = floorDiv(cy, COLUMN_PER_FRAGMENT);
  int lccx = modulo(cx, COLUMN_PER_FRAGMENT);
  int lccy = modulo(cy, COLUMN_PER_FRAGMENT);


  bool frag_new = false;
  bool chunk_col_new = false;

  Fragment* f = findFragment(fx, fy, dim);
  if (f == NULL) {
    f = new Fragment(fx, fy, dim);
    addFragment(f);
    frag_new = true;
  }
  ChunkCol* cc = f->getChunkCol(lccx, lccy);
  if(cc == NULL) {
    cc = new ChunkCol(cx, cy, f);
    f->setChunkCol(lccx, lccy, cc);
    chunk_col_new = true;
  }
  Chunk* c = cc->getChunk(cz);
  if(c != NULL) {
    throw 1;
  }
  c = new Chunk(cx, cy, cz, cc);
  c->set(data);
  cc->setChunk(c, cz);

  if (frag_new) {
    f->link();
    return;
  }
  if (chunk_col_new) {
    cc->link();
    return;
  }
  c->link();
}
#endif

#ifdef M_SERVER
ChunkCol* findLoadColumn(int xc, int yc, int dim) {
  int xf = floorDiv(xc, COLUMN_PER_FRAGMENT);
  int yf = floorDiv(yc, COLUMN_PER_FRAGMENT);
  Fragment* f = findFragment(xf, yf, dim);
  if (f == NULL) {
    f = new Fragment(xf, yf, dim);
    addFragment(f);
    f->load();
  }
  return f->getChunkCol(
    modulo(xc, COLUMN_PER_FRAGMENT),
    modulo(yc, COLUMN_PER_FRAGMENT));
}

void initMapgen() {
  Mapgen::noise_weights = {
    { 4, 2 },
    { 16, 8 },
    { 64, 32}
  };
}
#endif