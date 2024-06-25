#pragma once

#include "triangle.h"
#include "tri.h"
#include "tiny_obj_loader.h"

// https://github.com/anandhotwani/obj_raytracer/blob/master/src/trianglemesh.cpp

class mesh : public hittable {
  public:
    std::vector<std::shared_ptr<triangle>> triangles;
    std::shared_ptr<material> mat;
    // std::vector<std::shared_ptr<tri>> triangles;

    mesh(const char* path, shared_ptr<material> m, vec3 translate, float scale, bool add)
    {
        addLight = add;
        mat = m;

        std::string inputfile = path;
        unsigned long pos = inputfile.find_last_of("/");
        std::string mtlbasepath = inputfile.substr(0, pos + 1);  

        tinyobj::attrib_t attributes;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warnings;
        std::string errors;

        bool ret = tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, inputfile.c_str(), mtlbasepath.c_str());
        if (!warnings.empty()) { std::cout << "Warning: " << warnings << std::endl; }
        if (!errors.empty()) { std::cerr << "Error: " << errors << std::endl; }
        if (!ret) { exit(1); }

        // std::vector<Vertex> vertices;
        std::vector<point3> pts;

        // Min and max coordinates for bbox
        point3 min_point(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
        point3 max_point(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

        // Loop over shapes
        for (size_t s=0; s<shapes.size(); s++) {
            // Loop over faces(triangles)
            size_t index_offset = 0;
            for (size_t f=0; f<shapes[s].mesh.num_face_vertices.size(); f++) {
                uint8_t fv = shapes[s].mesh.num_face_vertices[f];

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx   = attributes.vertices[3*idx.vertex_index+0];
                    tinyobj::real_t vy   = attributes.vertices[3*idx.vertex_index+1];
                    tinyobj::real_t vz   = attributes.vertices[3*idx.vertex_index+2];

                    point3 pt = point3(vx*scale, vy*scale, vz*scale);
                    pts.push_back(pt);

                    // Update min and max coordinates
                    min_point[0] = fmin(min_point[0], pt[0]);
                    min_point[1] = fmin(min_point[1], pt[1]);
                    min_point[2] = fmin(min_point[2], pt[2]);

                    max_point[0] = fmax(max_point[0], pt[0]);
                    max_point[1] = fmax(max_point[1], pt[1]);
                    max_point[2] = fmax(max_point[2], pt[2]);
                }
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
        
        bbox = aabb(min_point, max_point);

        // Loops points
        for (size_t i=0; i<pts.size()/3; ++i) {
            triangles.push_back(std::make_shared<triangle>(pts[i*3], pts[i*3+1], pts[i*3+2], m));
            // triangles.push_back(std::make_shared<tri>(pts[i*3], pts[i*3+1], pts[i*3+2], m));
        }        
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
        bool addLight;

};

