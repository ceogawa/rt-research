#pragma once

#include <cassert>
#include "vec3.h"
#include <cstddef>
#include <span>
#include <vector>
#include <cstdlib>

std::vector<std::vector<size_t>> dbscan(shared_ptr<std::vector<point3>> data, float eps, int min_pts);
