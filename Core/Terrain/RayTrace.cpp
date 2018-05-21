#include "WorldLoader.h"

/*list<iVec3> find_voxels(dVec3 from, dVec3 to) {
  list<iVec3> visited_voxels;

  iVec3 current_voxel(floor(from.x / VOXEL_SIZE),
    floor(from.y / VOXEL_SIZE),
    floor(from.z / VOXEL_SIZE));

  iVec3 last_voxel(floor(to.x / VOXEL_SIZE),
    floor(to.y / VOXEL_SIZE),
    floor(to.z / VOXEL_SIZE));

  dVec3 ray = to - from;

  dVec3 step = vecSwitch<double>(ray >= dVec3(0), dVec3(1), dVec3(-1));

  dVec3 next_voxel_boundary = (current_voxel + step)*VOXEL_SIZE;

  dVec3 tMax = vecSwitch<double>(ray != dVec3(0), (next_voxel_boundary - from) / ray, dVec3(DBL_MAX));

  dVec3 tDelta = vecSwitch<double>(ray != dVec3(0), step / ray * VOXEL_SIZE, dVec3(DBL_MAX));

  iVec3 diff(0, 0, 0);
  bool neg_ray = false;
  if (current_voxel.x != last_voxel.x && ray.x<0) { diff.x--; neg_ray = true; }
  if (current_voxel.y != last_voxel.y && ray.y<0) { diff.y--; neg_ray = true; }
  if (current_voxel.z != last_voxel.z && ray.z<0) { diff.z--; neg_ray = true; }
  visited_voxels.push_back(current_voxel);
  if (neg_ray) {
    current_voxel += diff;
    visited_voxels.push_back(current_voxel);
  }

  while (bor(last_voxel != current_voxel)) {
    if (tMax.x < tMax.y) {
      if (tMax.x < tMax.z) {
        current_voxel.x += step.x;
        tMax.x += tDelta.x;
      } else {
        current_voxel.z += step.z;
        tMax.z += tDelta.z;
      }
    } else {
      if (tMax.y < tMax.z) {
        current_voxel.y += step.y;
        tMax.y += tDelta.y;
      } else {
        current_voxel.z += step.z;
        tMax.z += tDelta.z;
      }
    }
    visited_voxels.push_back(current_voxel);
  }
  return visited_voxels;
}
*/

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