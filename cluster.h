#ifndef CLUSTER_H
#define CLUSTER_H

#include <memory>
#include <vector>
#include <iostream>

#include "hittable.h"
#include "triangle.h"

class cluster {
public:
    cluster(float c) { 
        center = c; 
        prev = -infinity;
    }

    void clear() {
        triangles.clear();
        triangles.shrink_to_fit();
    }

    void add(std::shared_ptr<triangle> triangle) {
        triangles.push_back(triangle);
    }
    
    float recenter(int axis) {
        float tot = 0;
        for (std::shared_ptr<triangle> tri : triangles) {
            tot += tri->get_center()[axis];
        }
        prev = center;
        center = tot/triangles.size();
        return center;
    }

    vec3 com(int axis) {
        vec3 tot = vec3(0,0,0);
        for (std::shared_ptr<triangle> tri : triangles) {
            vec3 tcenter = tri->get_center();
            tot[0] += tcenter[0];
            tot[1] += tcenter[1];
            tot[2] += tcenter[2];
        }
        cmass[0] = tot[0]/triangles.size();
        cmass[1] = tot[1]/triangles.size();
        cmass[2] = tot[2]/triangles.size();
        return cmass;
    }
    
public:
    vec3 cmass;
    float center;
    float prev;
    std::vector<std::shared_ptr<triangle>> triangles;
};

#endif