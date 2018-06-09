#pragma once

#ifdef M_CLIENT
#define SCRIPT_GUI
#endif
#include "../NGin/NGin.h"

typedef pair<int, pair<int,int>> world_col_t;

typedef uint64_t guid_t;
const int GUID_L = 64;

const int BLOCK_PER_CHUNK = 16;
const int CHUNK_PER_COLUMN = 16;
const int COLUMN_PER_FRAGMENT = 4;
const int FRAGMENT_PER_REGION = 4;

const int BLOCK_BYTES = 8;
const int BLOCK_ID_BYTES = 2;
const int BLOCK_META_BYTES = BLOCK_BYTES - BLOCK_ID_BYTES;

const int BLOCK_BITS = 8 * BLOCK_BYTES;
const int BLOCK_ID_BITS = 8 * BLOCK_ID_BYTES;
const int BLOCK_META_BITS = 8 * BLOCK_META_BYTES;

extern string mapname;

const int MAP_X = 1 << 25; //2^24
const int MAP_Y = 1 << 24; //2^25

const int CHUNK_LOAD_RADIUS = 12;
const int CHUNK_UNLOAD_RADIUS = 16;

enum PacketIdsL {
  PacketChunk = 2,
  PacketMove = 3,
  PacketTurn = 4,
  PacketBlock = 5
};

guid_t randomE_GUID();

const int VersionA = 0;
const int VersionB = 1;
const int VersionC = 0;

enum Directions {
  Dir_MX = 0,
  Dir_PX = 1,
  Dir_MY = 2,
  Dir_PY = 3,
  Dir_MZ = 4,
  Dir_PZ = 5,
  Dir_All = 6
};

iVec3 jump(iVec3 in, int towards);