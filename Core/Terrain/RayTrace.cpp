#include "WorldLoader.h"

list<iVec3> find_voxels(dVec3 from, dVec3 to) {
  
  dVec3 dir = to - from;

  priority_queue_smallest<pair<double, int>> jumps;

  iVec3 current(floor(from.x / VOXEL_SIZE),
    floor(from.y / VOXEL_SIZE),
    floor(from.z / VOXEL_SIZE));

  iVec3 last(floor(to.x / VOXEL_SIZE),
    floor(to.y / VOXEL_SIZE),
    floor(to.z / VOXEL_SIZE));

  for (int i = current.x; i < last.x; i++) {
    jumps.push({(1 + i - from.x) / dir.x, 2 });
  }
  for (int i = current.x; i > last.x; i--) {
    jumps.push({ (i - from.x) / dir.x,3 });
  }

  for (int i = current.y; i < last.y; i++) {
    jumps.push({ (1 + i - from.y) / dir.y,4});
  }
  for (int i = current.y; i > last.y; i--) {
    jumps.push({ (i - from.y) / dir.y,5 });
  }

  for (int i = current.z; i < last.z; i++) {
    jumps.push({ (1 + i - from.z) / dir.z,8 });
  }
  for (int i = current.z; i > last.z; i--) {
    jumps.push({ (i - from.z) / dir.z,9 });
  }

  list<iVec3> res; 

  res.push_back(current);
  while (jumps.size()) {
    int top = jumps.top().second;
    jumps.pop();
    current += iVec3((top & 2) ? 1 : 0, (top & 4) ? 1 : 0, (top & 8) ? 1 : 0) * ((top & 1) ? -1 : 1);
    res.push_back(current);
  }

  return res;
}