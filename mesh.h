// #pragma once
#ifndef MESH_H
#define MESH_H

#include "triangle.h"
#include "tri.h"
#include "tiny_obj_loader.h"
#include "dbscan3.h"
#include <algorithm>

// https://github.com/anandhotwani/obj_raytracer/blob/master/src/trianglemesh.cpp
using namespace std;


class mesh : public hittable {
  public:
    std::vector<std::shared_ptr<triangle>> triangles;
    std::shared_ptr<material> mat;
    // std::vector<std::shared_ptr<tri>> triangles;

    mesh(const char* path, shared_ptr<material> m, vec3 translate, float scale, bool add)
    {
        addLight = add;
        mat = m;
        // models//skeleton.obj
        std::string inputfile = path;
        unsigned long pos = inputfile.find_last_of("\\");
        std::string mtlbasepath = inputfile.substr(0, pos);  
        std::string objname = inputfile.substr(pos+1, inputfile.length());

        normals.clear();
        normals_origin->clear();

        tinyobj::attrib_t attributes;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warnings;
        std::string errors;

        // cout << "inputFile: " << inputfile << endl;
        // cout << "mtlbasepath: " << mtlbasepath << endl;
        // cout << "objname: " << objname << endl;

        // after changing "file path" it loaded
        bool ret = tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, inputfile.c_str(), "");
        // cout << "after load obj" << endl;
        cout << inputfile.c_str() << endl;
        if (!warnings.empty()) { std::cout << "Warning: " << warnings << std::endl; }
        if (!errors.empty()) { std::cerr << "Error: " << errors << std::endl; }
        if (!ret) { 
            cout << "!ret" << endl;
            exit(1); 
        }

        // std::vector<Vertex> vertices;
        std::vector<point3> pts;
        vector<vec3> ns;

        // Min and max coordinates for bbox
        point3 min_point(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        point3 max_point(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

        // Loop over shapes
        face_cluster_id = make_shared<vector<int>>();

        cout << "shapes.size(): " << shapes.size() << endl;

        for (size_t s=0; s<shapes.size(); s++) {
            // Loop over faces(triangles)
            size_t index_offset = 0;
            for (size_t f=0; f<shapes[s].mesh.num_face_vertices.size(); f++) {
                uint8_t fv = shapes[s].mesh.num_face_vertices[f];
                //TODO calculate normals
                ns.clear();
                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx   = attributes.vertices[3*idx.vertex_index+0];
                    tinyobj::real_t vy   = attributes.vertices[3*idx.vertex_index+1];
                    tinyobj::real_t vz   = attributes.vertices[3*idx.vertex_index+2];

                    point3 pt = point3(vx*scale, vy*scale, vz*scale);
                    pts.push_back(pt);
                    ns.push_back(pt);

                    // Update min and max coordinates
                    min_point[0] = fmin(min_point[0], pt[0]);
                    min_point[1] = fmin(min_point[1], pt[1]);
                    min_point[2] = fmin(min_point[2], pt[2]);

                    max_point[0] = fmax(max_point[0], pt[0]);
                    max_point[1] = fmax(max_point[1], pt[1]);
                    max_point[2] = fmax(max_point[2], pt[2]);
                }

                
                vec3 center_ns = (min_point + max_point) / 2;
                for(int i = 0; i < 3; i++){
                    ns[i][0] = ns[i][0] - center_ns[0] + translate[0];
                    ns[i][1] = ns[i][1] - center_ns[1] + translate[1];
                    ns[i][2] = ns[i][2] - center_ns[2] + translate[2];
                }
               
                vec3 v1 = ns[1] - ns[0];
                vec3 v2 = ns[2] - ns[0];
                vec3 n = unit_vector(cross(v1, v2));


                // if(!(find(normals.begin(), normals.end(), n) != normals.end()) && (n[1] < 0.001 && n[1] > -0.001)){
                //     normals.push_back(n);
                //     normals_origin.push_back(ns[0]);
                // }
                vec3 n_center;
                normals.push_back(n);

                n_center[0] = (ns[0][0] + ns[1][0] + ns[2][0])/3.0;// + translate[0];
                n_center[1] = (ns[0][1] + ns[1][1] + ns[2][1])/3.0;// + translate[1];
                n_center[2] = (ns[0][2] + ns[1][2] + ns[2][2])/3.0;// + translate[2];
                
                normals_origin->push_back(n_center);
                // initialize all face points to default cluster id 

                // unclassified
                // cout << "before pushback" << endl;
                face_cluster_id->push_back(-1);
                // cout << "after pushback" << endl;

                // cout << "normal: <" << n_center[0] << ", " << n_center[1] << ", " << n_center[2] << ">" << endl;
                index_offset += fv;

            }
        }

        vec3 center = (min_point + max_point) / 2;
        min_point = point3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        max_point = point3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

        for (size_t p = 0; p < pts.size(); p++) {
            pts[p][0] = pts[p][0] - center[0] + translate[0];
            pts[p][1] = pts[p][1] - center[1] + translate[1];
            pts[p][2] = pts[p][2] - center[2] + translate[2];

            min_point[0] = fmin(min_point[0], pts[p][0]);
            min_point[1] = fmin(min_point[1], pts[p][1]);
            min_point[2] = fmin(min_point[2], pts[p][2]);

            max_point[0] = fmax(max_point[0], pts[p][0]);
            max_point[1] = fmax(max_point[1], pts[p][1]);
            max_point[2] = fmax(max_point[2], pts[p][2]);
        }
        
        cout << "num_verts: " << pts.size() << endl;
        cout << endl;
        bbox = aabb(min_point, max_point);

        // table
        // DBSCAN ds = DBSCAN(20, 1.6, normals_origin, face_cluster_id);

        //lamp 
        auto clusters = dbscan(normals_origin, 1.95, 20);

        auto flat_clusters = std::vector<size_t>(normals_origin->size());

        for(size_t i = 0; i < clusters.size(); i++)
        {
            for(auto p: clusters[i])
            {
                flat_clusters[p] = i + 1;
            }
        }

        for(size_t i = 0; i < normals_origin->size(); i++)
        {
            std::cout << normals_origin->at(i)[0] << ',' << normals_origin->at(i)[1] << ',' << normals_origin->at(i)[2] << ',' << flat_clusters[i] << '\n';
        }
        // DBSCAN ds = DBSCAN(20, 1.95, normals_origin, face_cluster_id);

        // ds.run();

        // Loops points

        auto blue       = make_shared<lambertian>(color(0.2, 0.2, 0.7));

        for(size_t j = 0; j < pts.size()/3; j++){
            // int id = (*face_cluster_id)[j];
            int id = -1;

            float r = static_cast<float>((id * 100) % 256) / 255.0f; 
            float g = static_cast<float>((id * 50) % 256) / 255.0f; 
            float b = static_cast<float>((id * 180) % 256) / 255.0f;

            auto col = make_shared<lambertian>(color(r,g,b));
            if(id < 0){
                triangles.push_back(std::make_shared<triangle>(pts[j*3], pts[j*3+1], pts[j*3+2], blue));
            }
            else{
                triangles.push_back(std::make_shared<triangle>(pts[j*3], pts[j*3+1], pts[j*3+2], col));
            }
            // switch((*face_cluster_id)[j]){
            //     case(0):
            //         triangles.push_back(std::make_shared<triangle>(pts[j*3], pts[j*3+1], pts[j*3+2], green));
            //         break;
            //     case(1):
            //         triangles.push_back(std::make_shared<triangle>(pts[j*3], pts[j*3+1], pts[j*3+2], brown));
            //         break;
            //     case(2):
            //         triangles.push_back(std::make_shared<triangle>(pts[j*3], pts[j*3+1], pts[j*3+2], orange));
            //         break;
            //     case(3):
            //         triangles.push_back(std::make_shared<triangle>(pts[j*3], pts[j*3+1], pts[j*3+2], purple));
            //         break;
            //     case(4):
            //         triangles.push_back(std::make_shared<triangle>(pts[j*3], pts[j*3+1], pts[j*3+2], yellow));
            //         break;
            //     case(5):
            //         triangles.push_back(std::make_shared<triangle>(pts[j*3], pts[j*3+1], pts[j*3+2], white));
            //         break;
            //     default:
            //         triangles.push_back(std::make_shared<triangle>(pts[j*3], pts[j*3+1], pts[j*3+2], blue));
            //         break;
            // }
   
        }
            
        // for (size_t i=0; i<pts.size()/3; ++i) {
            
        //     triangles.push_back(std::make_shared<triangle>(pts[i*3], pts[i*3+1], pts[i*3+2], blue));
        //     // triangles.push_back(std::make_shared<tri>(pts[i*3], pts[i*3+1], pts[i*3+2], m));
        // }        
        shapes.clear();
        materials.clear();
    }

    aabb bounding_box() const override { return bbox; }


    // void mesh::reScale() {
    //     vec3 extent = bbmax-bbmin;
    //     vec3 center = bbmin + 0.5*extent;
    //     vec3 invE = 2.1*(1/extent);

    //     for (auto& aTri : tris) {
    //         vec3 nv1 = (aTri.getv0() - center)*invE;
    //         vec3 nv2 = (aTri.getv1() - center)*invE;
    //         vec3 nv3 = (aTri.getv2() - center)*invE;
    //         aTri.reSet(nv1, nv2, nv3);
    //     }
    //     bbmin = (bbmin - center)*invE;
    //     bbmax = (bbmax - center)*invE;
    // }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const {
        bool hit = false;
        // long curr;
        // float t = ray_t.max;
        long num_triangles = triangles.size();
        for (long i=0; i<num_triangles; ++i) {
            if (triangles[i]->hit(r, ray_t, rec)) {
                hit = true;
                // curr = i;
                // ray_t.max = temp.t;
                // rec gets updated in intersect?
            }
        }
        return hit;
    }

    public:
        aabb bbox;
        vector<vec3> normals;
        shared_ptr<vector<point3>> normals_origin;
        shared_ptr<vector<int>> face_cluster_id;
        bool addLight;

};

#endif