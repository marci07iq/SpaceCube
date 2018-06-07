#pragma once

#include "../Definitions.h"

const double VOXEL_SIZE = 1;

typedef vec3<double> dVec3;

list<iVec3> find_voxels(dVec3 from, dVec3 to);
