#include "WorldLoader.h"

#ifdef M_CLIENT
#include "../../SpaceCube/UI/Render.h"
#endif

shared_mutex _fragmentsLock;
map<world_col_t, Fragment*> _fragments;

void addFragment(Fragment * frag) {
  _fragmentsLock.lock();
  _fragments[make_pair(frag->dim(), make_pair(frag->fx(), frag->fy()))] = frag;
  _fragmentsLock.unlock();
}

Fragment* findFragment(int fx, int fy, int dim) {
  Fragment* f = NULL;
  _fragmentsLock.lock_shared();
  auto it = _fragments.find(make_pair(dim, make_pair(fx, fy)));
  if(it != _fragments.end()) {
    f = it->second;
  }
  _fragmentsLock.unlock_shared();
  return f;
}

Fragment* findLoadFragment(int fx, int fy, int dim) {
  Fragment* f = findFragment(fx, fy, dim);
  if(f == NULL) {
    f = new Fragment(fx, fy, dim);
#ifdef M_SERVER
    f->load();
#endif
    addFragment(f);
    f->link();
  }
  return f;
}

ChunkCol* findChunkCol(int xc, int yc, int dim) {
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

ChunkCol* findLoadChunkCol(int cx, int cy, int dim) {
  int fx = floorDiv(cx, COLUMN_PER_FRAGMENT);
  int fy = floorDiv(cy, COLUMN_PER_FRAGMENT);
  Fragment* f = findLoadFragment(fx, fy, dim);
  ChunkCol* cc = f->getChunkCol(
    modulo(cx, COLUMN_PER_FRAGMENT),
    modulo(cy, COLUMN_PER_FRAGMENT));
#ifdef M_CLIENT
  if(cc == NULL) {
    cc = new ChunkCol(cx, cy, f);
    f->setChunkCol(modulo(cx, COLUMN_PER_FRAGMENT), modulo(cy, COLUMN_PER_FRAGMENT), cc);
    cc->link();
    DataElement* res = new DataElement();
    DataElement* nd;
    nd = new DataElement();
    vGFunc(cx, nd);
    res->addChild(nd);
    nd = new DataElement();
    vGFunc(cy, nd);
    res->addChild(nd);
    Connection->SendData(res, PacketChunk);
  }
#endif
  return cc;
}

Chunk * findChunk(int xc, int yc, int zc, int dim) {
  ChunkCol* cc = findChunkCol(xc, yc, dim);
  if (cc == NULL) {
    return NULL;
  }
  return cc->getChunk(zc);
}

BlockPos getBlock(int xb, int yb, int zb, int dim, bool& success) {
  Chunk* c = findChunk(
    floorDiv(xb, BLOCK_PER_CHUNK),
    floorDiv(yb, BLOCK_PER_CHUNK),
    floorDiv(zb, BLOCK_PER_CHUNK),
    dim);
  if(c != NULL) {
    success = true;
    return BlockPos{
      c,
      modulo(xb, BLOCK_PER_CHUNK),
      modulo(yb, BLOCK_PER_CHUNK),
      modulo(zb, BLOCK_PER_CHUNK),
      &(c->_blocks
      [modulo(xb, BLOCK_PER_CHUNK)]
      [modulo(yb, BLOCK_PER_CHUNK)]
      [modulo(zb, BLOCK_PER_CHUNK)])
    };
  }
  success = false;
}

void setBlock(iVec3 location, int dim, Block to) {
  int lbx = modulo(location.x, BLOCK_PER_CHUNK);
  int lby = modulo(location.y, BLOCK_PER_CHUNK);
  int lbz = modulo(location.z, BLOCK_PER_CHUNK);

  int lcx = floorDiv(location.x, BLOCK_PER_CHUNK);
  int lcy = floorDiv(location.y, BLOCK_PER_CHUNK);
  int lcz = floorDiv(location.z, BLOCK_PER_CHUNK);

  Chunk* c = findChunk(lcx, lcy, lcz, dim);

  if (c != NULL) {
    c->setBlock(lbx, lby, lbz, to);
  }
}

bool blockNeighbour(BlockPos & block, Directions dir, BlockPos & out) {
  out.b = NULL;
  switch (dir) {
    case Dir_MX:
      if (0 < block.lbx) {
        out.lbx = block.lbx - 1;
        out.lby = block.lby;
        out.lbz = block.lbz;
        out.c = block.c;
        out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
        return true;
      } else {
        Chunk* c = block.c->getNeigh(dir);
        if (c != NULL) {
          out.lbx = BLOCK_PER_CHUNK - 1;
          out.lby = block.lby;
          out.lbz = block.lbz;
          out.c = c;
          out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
          return true;
        }
      }
      return false;
      break;
    case Dir_PX:
      if (block.lbx < BLOCK_PER_CHUNK - 1) {
        out.lbx = block.lbx + 1;
        out.lby = block.lby;
        out.lbz = block.lbz;
        out.c = block.c;
        out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
        return true;
      } else {
        Chunk* c = block.c->getNeigh(dir);
        if (c != NULL) {
          out.lbx = 0;
          out.lby = block.lby;
          out.lbz = block.lbz;
          out.c = c;
          out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
          return true;
        }
      }
      return false;
      break;

    case Dir_MY:
      if (0 < block.lby) {
        out.lbx = block.lbx;
        out.lby = block.lby - 1;
        out.lbz = block.lbz;
        out.c = block.c;
        out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
        return true;
      } else {
        Chunk* c = block.c->getNeigh(dir);
        if (c != NULL) {
          out.lbx = block.lbx;
          out.lby = BLOCK_PER_CHUNK - 1;
          out.lbz = block.lbz;
          out.c = c;
          out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
          return true;
        }
      }
      return false;
      break;
    case Dir_PY:
      if (block.lby < BLOCK_PER_CHUNK - 1) {
        out.lbx = block.lbx;
        out.lby = block.lby + 1;
        out.lbz = block.lbz;
        out.c = block.c;
        out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
        return true;
      } else {
        Chunk* c = block.c->getNeigh(dir);
        if (c != NULL) {
          out.lbx = block.lbx;
          out.lby = 0;
          out.lbz = block.lbz;
          out.c = c;
          out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
          return true;
        }
      }
      return false;
      break;

    case Dir_MZ:
      if (0 < block.lbz) {
        out.lbx = block.lbx;
        out.lby = block.lby;
        out.lbz = block.lbz - 1;
        out.c = block.c;
        out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
        return true;
      } else {
        Chunk* c = block.c->getNeigh(dir);
        if (c != NULL) {
          out.lbx = block.lbx;
          out.lby = block.lby;
          out.lbz = BLOCK_PER_CHUNK - 1;
          out.c = c;
          out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
          return true;
        }
      }
      return false;
      break;
    case Dir_PZ:
      if (block.lbz < BLOCK_PER_CHUNK - 1) {
        out.lbx = block.lbx;
        out.lby = block.lby;
        out.lbz = block.lbz + 1;
        out.c = block.c;
        out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
        return true;
      } else {
        Chunk* c = block.c->getNeigh(dir);
        if (c != NULL) {
          out.lbx = block.lbx;
          out.lby = block.lby;
          out.lbz = 0;
          out.c = c;
          out.b = &(out.c->_blocks[out.lbx][out.lby][out.lbz]);
          return true;
        }
      }
      return false;
      break;
  }
  return false;
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

void initMapgen() {
  Mapgen::noise_weights = {
    { 4, 0.8 },
    { 16, 3 },
    { 64, 20},
    { 256, 70 },
    { 1024, 160 }
  };
}

/*struct chunkRequest {
  guid_t _from;
  int _ccx, _ccy;
  int _dim;
  enum RequestType {
    Send,     //Load if needed, then send.
    TryDelete //Save and delete if unused.
  };
  RequestType _type;
};
list<chunkRequest> _requests;
mutex _requestsLock;
void registerRequest(int ccx, int ccy, int dim, guid_t from, chunkRequest::RequestType type) {
  _requestsLock.lock();
  _requests.push_back({from, ccx, ccy, dim, type});
  _requestsLock.unlock();
}
void requestLoop() {
  for(auto&& it : req)
}*/
#endif