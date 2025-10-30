#ifndef CLUSTER_SPHERE_H
#define CLUSTER__SPHERE_H

#include <memory>
#include <vector>
#include <iostream>

#include "hittable.h"
#include "triangle.h"
#include "sphere.h"

class cluster_sphere {
public:
    cluster_sphere(float c) { 
        center = c; 
        prev = -infinity;
    }

    void clear() {
        spheres.clear();
        spheres.shrink_to_fit();
    }

    void add(std::shared_ptr<sphere> sphere) {
        spheres.push_back(sphere);
    }
    
    float recenter(int axis) {
        float tot = 0;
        for (std::shared_ptr<sphere> sphere : spheres) {
            tot += sphere->get_center()[axis];
        }
        prev = center;
        center = tot/spheres.size();
        return center; 
    }

    vec3 com_sphere(){
        vec3 total = vec3(0, 0, 0);
        for (std::shared_ptr<sphere> sphere : spheres){
            vec3 c = sphere->get_center();
            // x, y, z coord of total sphere mass
            total[0] += c[0];
            total[1] += c[1];
            total[2] += c[2];
        }
        cmass[0] = total[0]/spheres.size();
        cmass[1] = total[1]/spheres.size();
        cmass[2] = total[2]/spheres.size();

        return cmass;
    }
    
public:
    vec3 cmass;
    float center;
    float prev;
    std::vector<std::shared_ptr<sphere>> spheres;
};

#endif