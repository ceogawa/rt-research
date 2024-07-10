// #ifndef CLUSTER_H
// #define CLUSTER_H

// #include <memory>
// #include <vector>
// #include <iostream>

// #include "hittable.h"
// #include "triangle.h"
// #include "sphere.h"

// class cluster {
// public:
//     cluster(float c) { 
//         center = c; 
//         prev = -infinity;
//     }

//     void clear() {
//         spheres.clear();
//         spheres.shrink_to_fit();
//     }

//     void add(std::shared_ptr<sphere> sphere) {
//         spheres.push_back(sphere);
//     }
    
//     float recenter(int axis) {
//         float tot = 0;
//         for (std::shared_ptr<sphere> sphere : spheres) {
//             tot += sphere->get_center()[axis];
//         }
//         prev = center;
//         center = tot/spheres.size();
//         return center;
//     }

//     vec3 com(int axis) {
//         vec3 tot = vec3(0,0,0);
//         for (std::shared_ptr<triangle> tri : triangles) {
//             vec3 tcenter = tri->get_center();
//             tot[0] += tcenter[0];
//             tot[1] += tcenter[1];
//             tot[2] += tcenter[2];
//         }
//         cmass[0] = tot[0]/triangles.size();
//         cmass[1] = tot[1]/triangles.size();
//         cmass[2] = tot[2]/triangles.size();
//         return cmass;
//     }

//     vec3 com_sphere(){
//         vec3 total = vec3(0, 0, 0);
//         for (std::shared_ptr<sphere> sphere : spheres){
//             point3 center = sphere->get_center();
//             // x, y, z coord of total sphere mass
//             total[0] += center[0];
//             total[1] += center[1];
//             total[2] += center[2];
//         }
//         cmass[0] = total[0]/spheres.size();
//         cmass[1] = total[1]/spheres.size();
//         cmass[2] = total[2]/spheres.size();

//         return cmass;
//     }
    
// public:
//     vec3 cmass;
//     float center;
//     float prev;
//     std::vector<std::shared_ptr<sphere>> spheres;
// };

// #endif