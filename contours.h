#pragma once

#include <utility>
#include <map>
#include <vector>
#include <hashtable.h>
#include <hash_map>
#include <unordered_map>
#include <functional>
#include "vec3.h"

using namespace std;

using edge = pair<point3, point3>;
//TODO is an edge list necessary or can i check if its in adjacencies already??????

// 1st create edges from vertices
// 2nd add the edge to edges if it is not already
// 3rd update adjacencies information according to if edge is in edges or not
// 4th check adjacencies to see if they are contours

// the reason i created an edge list separately was so that i could simply iterate through all edges without needing a key
// do i just make the adjacencies list a different data structure?? it need sto be iterable... maybe a pair of pairs??

// could just do pair of pairs???????
//  int idx = 0;
// for (auto it = vec.begin(); it != vec.end(); ++it) {
//     std::cout << "Index: " << idx++ << ", Key: " << it->first << ", Value: " << it->second << std::endl;
// }

bool edge_equals(edge e1, edge e2){
    return ((e1.first == e2.first) && (e1.second == e2.second));
}

void check_contour(edge e, shared_ptr<unordered_map<edge, pair<vec3, vec3>, edge_hash>> edges, shared_ptr<map<edge, pair<vec3,vec3>>> contours, vec3 camera){
    double dot1 = dot((*edges)[e].first, camera);
    double dot2 = dot((*edges)[e].second, camera);
    if (((dot1 < 0) && (dot2 > 0)) || ((dot1 > 0) && (dot2 < 0))){
        contours->insert({e, (*edges)[e]});
    } 
}   

// hashmap edges


void check_edge(edge e, shared_ptr<unordered_map<edge, pair<vec3, vec3>, edge_hash>> edges, vec3 face_normal, shared_ptr<map<edge, pair<vec3,vec3>>> adjacencies, shared_ptr<map<edge, 
                pair<vec3,vec3>>> contours, vec3 camera){
    // base case
    if(edges->size() == 0){

        pair<vec3, vec3> p(face_normal, vec3(0));

        edges->insert({e, p});
        adjacencies->insert({e, p});
    }

    bool found = false;

    for(size_t i = 1; i < edges->size(); i++){
        cout << "edges index: " << i << "adjacencies size: " << adjacencies->size() << endl;

        if(edges->find({e.first, e.second}) != edges->end() || edges->find({e.second, e.first}) != edges->end()){
            // checking if edge is already "seen" and in edges, it would also already be in adjacencies
             // edge already exists, therefore we need to update the adjacencies value to include the contour's second face
            (*adjacencies)[e].second = face_normal;
            (*edges)[e].second = face_normal;

            // can check if the contour at this point is a silhouette
            // check_contour(e, adjacencies, contours, camera);  
            check_contour(e, edges, contours, camera);     
            found = true;
            break;     
        }
        
    }
    if(!found){
        // temporarily assign the second face to an arbitrary zero vector

        pair<vec3, vec3> p(face_normal, vec3(0));
        // insert new contour with associated edge and one face pair
        edges->insert({e, p});
        adjacencies->insert({e, p});
    }
            
    
}

struct edge_hash{
    // Used ChatGPT to derive hash function for two vectors
    std::size_t operator()(const edge e) const {
        std::size_t h1 = std::hash<float>()(e.first[0]) ^ (std::hash<float>()(e.first[1]) << 1) ^ (std::hash<float>()(e.first[2]) << 2);
        std::size_t h2 = std::hash<float>()(e.second[0]) ^ (std::hash<float>()(e.second[1]) << 1) ^ (std::hash<float>()(e.second[2]) << 2);

        return h1 ^ (h2 << 1);
    }
};

shared_ptr<map<edge, pair<vec3,vec3>>> create_edges(vector<vec3> pts, vector<vec3> normals, vec3 camera){

    shared_ptr<unordered_map<edge, pair<vec3, vec3>, edge_hash>> es = make_shared<unordered_map<edge, pair<vec3, vec3>, edge_hash>>();

    shared_ptr<map<edge, pair<vec3,vec3>>> adjacencies = make_shared<map<edge, pair<vec3,vec3>>>();
    shared_ptr<map<edge, pair<vec3,vec3>>> contours = make_shared<map<edge, pair<vec3,vec3>>>();

    // create edges and update adjacencies
    for(size_t i = 0; i < normals.size(); i++){
        edge e1 = edge(pts[i], pts[i+1]);
        edge e2 = edge(pts[i+1], pts[i+2]);
        edge e3 = edge(pts[i+2], pts[i]);

        check_edge(e1, es, normals[i], adjacencies, contours, camera);
        check_edge(e2, es, normals[i], adjacencies, contours, camera);
        check_edge(e3, es, normals[i], adjacencies, contours, camera);
    }

    return contours;
}

point3 point_along_line(vec3 u, point3 v, double t){
    double x = v[0] + t*u[0];
    double y = v[1] + t*u[1];
    double z = v[2] + t*u[2];

    return point3(x, y, z);
}

vector<vec3> contour_lights(vector<vec3> pts, vector<vec3> normals, vec3 camera){
    shared_ptr<map<edge, pair<vec3,vec3>>> contours = create_edges(pts, normals, camera);
    vector<vec3> lights;
    int divisions = 10;

    for (auto it = contours->begin(); it != contours->end(); it++) {
        vec3 u = it->first.second - it->first.first;
        for(int t = 0; t < divisions; t++){
            lights.push_back(point_along_line(u, it->first.first, (double)(t/divisions)));
        }
    }

    return lights;
    // create light at the contours, add tons of small point lights along the length of the contour...
}


