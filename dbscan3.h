#pragma once


#include <cassert>
#include <cstddef>
#include <span>
#include <vector>
#include <cstdlib>

struct point2d
{
    float x, y;
};

struct point3d
{
    float x, y, z;
};

auto dbscan(point2d* data, float eps, int min_pts) -> std::vector<std::vector<size_t>>;
auto dbscan(point3d* data, float eps, int min_pts) -> std::vector<std::vector<size_t>>;

// template<size_t dim>
// auto dbscan(const std::span<float>& data, float eps, int min_pts)
// {
//     static_assert(dim == 2 or dim == 3, "This only supports either 2D or 3D points");
//     assert(data.size() % dim == 0);
    
//     if(dim == 2)
//     {
//         auto * const ptr = reinterpret_cast<float const*> (data.data());
//         auto points = std::span<const point2> 
//     }
// }
// #pragma once

// #include <cassert>
// #include "vec3.h"
// #include <cstddef>
// #include <span>
// #include <vector>
// #include <cstdlib>

// std::vector<std::vector<size_t>> dbscan(shared_ptr<std::vector<point3>> data, float eps, int min_pts);
