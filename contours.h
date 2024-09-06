#pragma once

#include <utility>
#include <map>
#include <vector>
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
//     std:: << "Index: " << idx++ << ", Key: " << it->first << ", Value: " << it->second << std::endl;
// }

// point3 max_pt(point3 p1, point3 p2){
//     // condition ? expression1 : expression2;
//     // expression1 if true, expression2 if false
//     bool check = (p1[0] > p2[0]) ? true : false;
//     if(check){
//         return p1;
//     }
//     else{
//         return p2;
//     }
// }


void adjacency_list(shared_ptr<vector<pair<edge, pair<vec3,vec3>>>> contours){
    shared_ptr<map<vec3, vector<edge>>> adjs = make_shared<map<vec3, vector<edge>>>();
    // init adjacency list for vertices and corresponding edges
    for(auto it = contours->begin(); it != contours->end(); it++){
        vec3 v1 = it->first.first;
        vec3 v2 = it->first.second;
        vector<edge> es;

        if(adjs->find(v1) != adjs->end()){
            // push back edge to corresponding vertices adjacency list
            (*adjs)[v1].push_back(it->first);
            cout << "v1 num edges: " << (*adjs)[v1].size() << endl;
        }
        else{
            es.clear();
            es.push_back(it->first);
            adjs->insert({v1, es});
        }

        if(adjs->find(v2) != adjs->end()){ (*adjs)[v2].push_back(it->first); 
        cout << "v2 num edges: " << (*adjs)[v2].size() << endl;
        }
        else{
            es.clear();
            es.push_back(it->first);
            adjs->insert({v2, es});
        }

    }

    vector<edge> chained_contours;
    for(auto it = contours->begin(); it != contours->end(); it++){
        point3 v1 = it->first.first;
        point3 v2 = it->first.second;

        
    }

     //contours <map<edge, pair<vec3, vec3>>>
    // iter through contours:
        
        // if (e.first IN adjs)
        //     adj[e.first].push_back(e);
        // else 
            // adj insert({e.first, e})


        // if (e.second IN adjs)
        //  adj[e.second].push_back(e);
        // else 
            // adj insert({e.second, e})


    // cur edge e;
    // cur vertex v;
    // double min_angle = std::max(double);
    // int indx = 0;
    // for each edge, in adjs (adjs[i]):
    //     vector ei
    //     if angle between ei and e < min angle:
    //         min angle = angle bewtween ei and e;
    //         indx = i;
    // replace e and adjs[indx] with edge = chained edge;

    // must keep track of:
    //  - edgei with minimum angular change from cur edge e
    //  - the "direction" of the edge e to new edge i... i guess it would be from
        // - e.v != v -> edgei.v != v

    // want to take the existing contours, and chain them.. only want adjacency list for contours??
}

struct edge_hash{
    // Used ChatGPT to derive hash function for two vectors
    std::size_t operator()(const edge e) const {
        std::size_t h1 = std::hash<float>()(e.first[0]) ^ (std::hash<float>()(e.first[1]) << 1) ^ (std::hash<float>()(e.first[2]) << 2);
        std::size_t h2 = std::hash<float>()(e.second[0]) ^ (std::hash<float>()(e.second[1]) << 1) ^ (std::hash<float>()(e.second[2]) << 2);

        return h1 ^ (h2 << 1);
    }
};

point3 point_along_line(vec3 u, point3 v, double t){
    double x = v[0] + t*u[0];
    double y = v[1] + t*u[1];
    double z = v[2] + t*u[2];
 
    return point3(x, y, z);
}

void check_if_contour(edge e, shared_ptr<unordered_map<edge, pair<vec3, vec3>, edge_hash>> edges, shared_ptr<vector<pair<edge, pair<vec3,vec3>>>> contours, vec3 camera){
    double dot1 = dot((*edges)[e].first, camera);
    double dot2 = dot((*edges)[e].second, camera);
    // cout << "len normals: 1: " << (*edges)[e].first.length() << " 2: " << (*edges)[e].second.length() << " cam: " << camera.length() << endl;
    // cout << "normal: " << (*edges)[e].first << " normal: " << (*edges)[e].second << endl;
    // cout << "v1: " << e.first << " v2: " << e.second;
    // cout << "contour check: " << dot1 << " and " << dot2 << endl;
    double diff = acos((double)(dot(unit_vector((*edges)[e].first), unit_vector((*edges)[e].second))));///(double)((*edges)[e].first.length() + (*edges)[e].second.length()));
    // TODO CLEANUP ?? <= >=???
    if((((dot1 < -0.01 && dot2 > 0.01 ) || (dot2 < -0.01 && dot1 > 0.01 )))){// && (fabs(diff) > 1.5)){ //} && (abs(dot1 + dot2) >= 0.1)){
        // cout << "true contour" << endl;//<< dot1 << " and " << dot2 << endl;

        contours->push_back({e, (*edges)[e]});
        // cout << "contours: " << e.first << "   "<< e.second << endl;
    }
    // if (((dot1 <= 0.0) && (dot2 > 0.0)) || ((dot1 >= 0.0) && (dot2 < 0.0)) || ((dot1 > 0.0) && (dot2 <= 0.0)) || ((dot1 < 0.0) && (dot2 >= 0.0))){
    //     contours->insert({e, (*edges)[e]});
    // } 
}   

bool pt_order(point3 p1, point3 p2){
    // condition ? expression1 : expression2;
    // expression1 if true, expression2 if false
    bool check = (p1[0] <= p2[0]) ? true : false;
    return check;
}

void check_edge(edge e,  shared_ptr<map<vec3, edge>> adjacencies, shared_ptr<unordered_map<edge, pair<vec3, vec3>, edge_hash>> edges, vec3 face_normal,  shared_ptr<vector<pair<edge, pair<vec3,vec3>>>> contours, vec3 camera){
    // base case
    point3 minpt = (pt_order(e.first, e.second)) ? e.first : e.second;
    point3 maxpt = (pt_order(e.first, e.second)) ? e.second : e.first;
    edge new_edge = edge(minpt, maxpt);

    if(edges->size() == 0){
        pair<vec3, vec3> p(face_normal, vec3(0));
        edges->insert({new_edge, p});
        // cout << "edge: " << "("<< new_edge.first << ") and (" << new_edge.second << ") "<< endl;
        return;
    }

    if(edges->find({new_edge.first, new_edge.second}) != edges->end()){// || edges->find({new_edge.second, new_edge.first}) != edges->end()){
        // edge already exists, therefore we need to update value to include the contour's second face
        // cout << "edge found and what is its first facenormal: " << (*edges)[new_edge].first << endl;
        (*edges)[new_edge].second = face_normal;
        // can check if the edge e is a silhouette now that we have a complete adjacency from edge to faces
        // cout << "repeat edge..." << endl;
        check_if_contour(new_edge, edges, contours, camera);      
    }
    else{
        // temporarily assign the second face to an arbitrary zero vector
        pair<vec3, vec3> p(face_normal, vec3(0));
        // insert new contour with associated edge and one face pair
        // cout << "edge: " << "("<< new_edge.first << ") and (" << new_edge.second << ") "<< endl;
        edges->insert({new_edge, p});
    }
}

vector<vec3> contour_lights(vector<vec3> pts, vector<vec3> normals, vec3 camera){
    shared_ptr<unordered_map<edge, pair<vec3, vec3>, edge_hash>> es = make_shared<unordered_map<edge, pair<vec3, vec3>, edge_hash>>();
    shared_ptr<vector<pair<edge, pair<vec3,vec3>>>> contours = make_shared<vector<pair<edge, pair<vec3,vec3>>>>();
    shared_ptr<map<vec3, edge>> adjs = make_shared<map<vec3, edge>>();

    // create edges and update their adjacent facies
    for(size_t i = 0; i < normals.size(); i++){
        edge e1 = edge(pts[i*3], pts[i*3+1]);
        edge e2 = edge(pts[i*3+1], pts[i*3+2]);
        edge e3 = edge(pts[i*3+2], pts[i*3]);

        // cout << "normals passed in: " << normals[i] << endl;
        check_edge(e1, adjs, es, normals[i], contours, camera);
        check_edge(e2, adjs, es, normals[i], contours, camera);
        check_edge(e3, adjs, es, normals[i], contours, camera);
    }

    vector<vec3> lights;
    lights.clear();
    int divisions = 4;

    cout << "edges size: " << es->size() << endl;
    cout << "contours size: " << contours->size() << endl;

    // cout << "contours length: " << contours->size() << endl;
    // adjacency_list(contours); 

    for (auto it = contours->begin(); it != contours->end(); it++) {
        //cout << "in contours" << endl;
        vec3 u = it->first.second - it->first.first;
        divisions = (u.length()/0.08);
        // lights.push_back(point_along_line(u, it->first.first, 0.5));
        for(int t = 0; t < divisions; t++){
            // lights.push_back(it->first.first);
            // lights.push_back(it->first.second);
            lights.push_back(point_along_line(u, it->first.first, (double)t/(double)(divisions - 1)));
        }
    }

    return lights;
    // create light at the contours, add incremental small point lights along the length of the contour...
    // eventually CHAIN EDGES, divisions will depend on edge length
}


