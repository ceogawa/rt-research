#pragma once

#include <utility>
#include <map>
#include <vector>
#include "vec3.h"

using namespace std;

using edge = pair<point3, point3>;

void check_edge(edge e, vector<edge> edges, vec3 face_normal, shared_ptr<map<edge, pair<vec3,vec3>>> contours){
    for(int i = 0; i < edges.size(); i++){
        // checking if edge is already "seen" and in edges (maybe check if e is in contours instead??)
        if(edges[i] == edge(e.first, e.second) || edges[i] == edge(e.second, e.first)){
            // edge already exists, therefore we need to update the contours value to include the contour's second face
            (*contours)[e].second = face_normal;         
        }
        else{
            // temporarily assign the second face to an arbitrary zero vector
            pair<vec3, vec3> p(face_normal, vec3(0));
            // insert new contour with associated edge and one face pair
            contours->insert({e, p});
        }
    }
}
