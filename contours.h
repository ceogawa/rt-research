#pragma once

#include <utility>
#include <map>
#include <vector>
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

void check_contour(edge e, shared_ptr<map<edge, pair<vec3,vec3>>> adjacencies, shared_ptr<map<edge, pair<vec3,vec3>>> contours, vec3 camera){
    double dot1 = dot((*adjacencies)[e].first, camera);
    double dot2 = dot((*adjacencies)[e].second, camera);
    if (((dot1 < 0) && (dot2 > 0)) || ((dot1 > 0) && (dot2 < 0))){
        contours->insert({e, (*adjacencies)[e]});
    } 
}   

void check_edge(edge e, shared_ptr<vector<edge>> edges, vec3 face_normal, shared_ptr<map<edge, pair<vec3,vec3>>> adjacencies, shared_ptr<map<edge, pair<vec3,vec3>>> contours
, vec3 camera){
    // base case
    if(edges->size() == 0){
        edges->push_back(e);
        pair<vec3, vec3> p(face_normal, vec3(0));
        adjacencies->insert({e, p});
    }

    bool found = false;

    for(size_t i = 1; i < edges->size(); i++){
        cout << "edges index: " << i << "adjacencies size: " << adjacencies->size() << endl;
        // checking if edge is already "seen" and in edges, it would also already be in adjacencies
        if((edge_equals((*edges)[i], edge(e.first, e.second))) || edge_equals(((*edges)[i]), edge(e.second, e.first))){
            // edge already exists, therefore we need to update the adjacencies value to include the contour's second face
            (*adjacencies)[e].second = face_normal;

            // can check if the contour at this point is a silhouette
            check_contour(e, adjacencies, contours, camera);     
            found = true;
            break;             
        }
    }
    if(!found){
        // temporarily assign the second face to an arbitrary zero vector
        edges->push_back(e);
        pair<vec3, vec3> p(face_normal, vec3(0));
        // insert new contour with associated edge and one face pair
        adjacencies->insert({e, p});
    }
            
    
}

shared_ptr<map<edge, pair<vec3,vec3>>> create_edges(vector<vec3> pts, vector<vec3> normals, vec3 camera){
    shared_ptr<map<edge, pair<vec3,vec3>>> adjacencies = make_shared<map<edge, pair<vec3,vec3>>>();
    shared_ptr<vector<edge>> edges = make_shared<vector<edge>>();
    shared_ptr<map<edge, pair<vec3,vec3>>> contours = make_shared<map<edge, pair<vec3,vec3>>>();

    // create edges and update adjacencies
    for(size_t i = 0; i < normals.size(); i++){
        edge e1 = edge(pts[i], pts[i+1]);
        edge e2 = edge(pts[i+1], pts[i+2]);
        edge e3 = edge(pts[i+2], pts[i]);

        check_edge(e1, edges, normals[i], adjacencies, contours, camera);
        check_edge(e2, edges, normals[i], adjacencies, contours, camera);
        check_edge(e3, edges, normals[i], adjacencies, contours, camera);
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
            cout << "pushing back light" << endl;
            lights.push_back(point_along_line(u, it->first.first, (double)(t/divisions)));
        }
    }

    return lights;
    // create light at the contours, add tons of small point lights along the length of the contour...
}


