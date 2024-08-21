#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "rtweekend.h"
#include "hittable.h"
#include "aabb.h"
#include "mesh.h"
#include "triangle.h"
#include "cluster.h"

#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <random>


class hittable_list : public hittable {
  public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
        bbox = aabb(bbox, object->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        auto hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects) {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    aabb bounding_box() const override { return bbox; }


    /* Cluster Helper Functions */

    static bool compareTri(const std::shared_ptr<triangle>& t1, const std::shared_ptr<triangle>& t2) 
    { 
        return (t1->bounding_box().x.max < t2->bounding_box().x.max); 
    }

    std::shared_ptr<lambertian> pickMat(int i) {
        std::shared_ptr<lambertian> mat;
        switch(i) {
            case 1: // red
                mat = make_shared<lambertian>(color(1.0, 0.0, 0.0));
                break;
            case 2: // orange
                mat = make_shared<lambertian>(color(1.0, 0.5, 0.0));
                break;
            case 3: // yellow
                mat = make_shared<lambertian>(color(1.0, 1.0, 0));
                mat = make_shared<lambertian>(color(0.0, 1.0, 0));
                break;
            case 4: // green
                mat = make_shared<lambertian>(color(0.0, 1.0, 0.0));
                break;
            case 5: // blue
                mat = make_shared<lambertian>(color(0.0, 0.0, 1.0));
                break;
            case 6: // purple
                mat = make_shared<lambertian>(color(1.0, 0.0, 1.0));
                break;
            default:
                mat = make_shared<lambertian>(color(0.4, 0.4, 0.4));
                mat = make_shared<lambertian>(color(1.0, 0.0, 0));

        }
        return mat;
    }

    bool check_clusters(std::vector<std::shared_ptr<cluster>> clusters) {
        for (std::shared_ptr<cluster> c : clusters) {
            if (abs(c->center-c->prev) > 0.0001) {
                return 1;
            }
        }
        return 0;
    }

    /* Pseudocode:
    1. Find bounds of scene (furthest point from camera in camera direction)
    2. Assign k centers to the planes of the scene perpendicular to the camera 
        at furthest/k intervals 
    3. Update k centers based on vertices added to each layer
    4. Find center of mass for each layer 
    5. Place lights at each, ensuring that there is an object in front of the light */

    std::vector<vec3> layer(point3 lookAt, point3 lookFrom, int k, int a) {

        std::vector<std::shared_ptr<mesh>> meshObjects; // Meshes
        std::vector<std::shared_ptr<triangle>> tris; // Triangles
        std::vector<vec3> lights; // Centers of Mass

        // Convert objects to meshObjects
        for (const auto& obj : objects) {
            // Check if the current object is actually a mesh
            std::shared_ptr<mesh> meshObj = std::dynamic_pointer_cast<mesh>(obj);
            if (meshObj) {
                meshObjects.push_back(meshObj);
            }
        }

        // vec3 viewDir = unit_vector(lookFrom-lookAt);
        float maxDist = -infinity; // only X value matters
        float minY = 0;
        float maxY = 0;
        float minZ = 0;
        float maxZ = 0;
        float scale = 0.7;
        float scale2 = 0.5;
        // std::cout<<meshObjects.size()<<std::endl;

        // Find max distance of object and add all triangles in scene to one vector
        for(size_t o=0; o<meshObjects.size(); o++) {
            float currDist = 0;
            aabb currBbox = meshObjects[o]->bbox;
            point3 currCenter = currBbox.get_center();
            point3 currMax = currBbox.get_max();
            point3 currMin = currBbox.get_min();

            cout << "num normals for mesh: " << meshObjects[o]->normals.size() << endl;
            cout << "num normal points of origin for mesh: " << meshObjects[o]->normals_origin->size() << endl;
            for(size_t i=0; i < meshObjects[o]->normals.size(); i++){

                vec3 n = unit_vector(meshObjects[o]->normals[i]);
                vec3 cam = unit_vector(lookAt - lookFrom);

                double d = dot(n, cam);
                double theta = acos(d); 
                

                // cout << endl;
                point3 c = currBbox.get_center();
                // if(d == 0.0){    
                //     lights.push_back(meshObjects[o]->normals_origin[i]);
                // }
                if((d <= -.97 && d >= -1.0) && (meshObjects[o]->normals_origin->at(i)[2] >= c[2])){
                    lights.push_back(meshObjects[o]->normals_origin->at(i));
                }

                // if((d > .96 && d < 1) || d == .989949){
                //     lights.push_back(meshObjects[o]->normals_origin[i]);
                //     lights.push_back(meshObjects[o]->normals_origin[i]);
                //     lights.push_back(meshObjects[o]->normals_origin[i]);
                // }

                // if((theta <= 1.62 && theta >= 1.57) && (d < 0)) {
                //     lights.push_back(meshObjects[o]->normals_origin[i]);
                // }
                // point3 c = currBbox.get_center();
                //  if((theta <= 1.57 && theta >= 0.5) && (d >= 0) && (meshObjects[o]->normals_origin[i][2] >= c[2])) {
                //     lights.push_back(meshObjects[o]->normals_origin[i]);


                // }
                // double theta = acos(d/(n.length() * (lookAt-lookFrom).length()));
                // cout << "theta: " << theta << endl;
                // if(theta <= 6.28 && theta >= 0){
                //     // lights.push_back(meshObjects[o]->normals_origin[i]);
                // }
            }

            cout << "lights.size()" << lights.size() << endl;
            
            /* ADD LIGHTS BEHIND EACH OBJECT */

            if (meshObjects[o]->addLight != false) {
                float yDist = currBbox.y.max-currBbox.y.min;
                point3 pt = point3(currBbox.x.min-(currBbox.x.max-currBbox.x.min)/8, 
                                currBbox.y.min+(yDist*scale),
                                currCenter[2]);
                lights.push_back(pt);

                pt = point3(currBbox.x.min-(currBbox.x.max-currBbox.x.min)/8, 
                                currBbox.y.min+(yDist*scale2),
                                currBbox.z.min+(currBbox.z.max-currBbox.z.min)/6);
                lights.push_back(pt);

                pt = point3(currBbox.x.min-(currBbox.x.max-currBbox.x.min)/8, 
                                currBbox.y.min+(yDist*scale2),
                                currBbox.z.max-(currBbox.z.max-currBbox.z.min)/6);
                lights.push_back(pt);

                std::cout << "size: " << currMax[1]-currMin[1] << std::endl;
                std::cout << "Max: " << "(" << currMax[0] << ", " << currMax[1] << ", " << currMax[2] << ")" << std::endl; // COMMENT
                std::cout << "Min: " << "(" << currMin[0] << ", " << currMin[1] << ", " << currMin[2] << ")" << std::endl; // COMMENT
                std::cout << "Cen: " << "(" << currCenter[0] << ", " << currCenter[1] << ", " << currCenter[2] << ")" << std::endl; // COMMENT
                std::cout << "Out: " << "(" << pt[0] << ", " << pt[1] << ", " << pt[2] << ")" << std::endl; // COMMENT
                std::cout << "-" << std::endl;
            } 
            currDist = std::max(abs(currBbox.axis(a).min-lookFrom[a]), 
                                abs(currBbox.axis(a).max-lookFrom[a]));
            if (currDist > maxDist) {
                maxDist = currDist;
            }
            if (currBbox.axis(1).min < minY) {
                minY = currBbox.axis(1).min;
            }
            if (currBbox.axis(1).max > maxY) {
                maxY = currBbox.axis(1).max;
            }
            if (currBbox.axis(2).min < minZ) {
                minZ = currBbox.axis(2).min;
            }
            if (currBbox.axis(2).max > maxZ) {
                maxZ = currBbox.axis(2).max;
            }
            for(size_t t=0; t<meshObjects[o]->triangles.size(); t++) {
                tris.push_back(meshObjects[o]->triangles[t]);
            }
        }

        // RANDOMLY GENERATE LIGHTS - STRAW MAN
        // for(int i=0; i<19; i++) {
        //     lights.push_back(vec3(random_double(lookFrom[0], maxDist), random_double(minY, maxY), random_double(minZ, maxZ)));
        // }
        // return lights;

        // Scene bounds: lookFrom -> maxDist
        // Sort triangles according to distance
        std::sort(tris.begin(), tris.end(), compareTri);
        std::cout << "max: " << maxDist << std::endl;

        // Initialize centers for k-means
        float inc = maxDist/(k+1);
        std::cout << "inc: " << inc << std::endl;
        std::vector<std::shared_ptr<cluster>> clusters;
        for (float i=inc; i<=k*inc; i+=inc) {
            std::cout << "init " << i << std::endl; // COMMENT
            clusters.push_back(std::make_shared<cluster>(abs(i-lookFrom[a])));
        }
        int i=0;
        while(check_clusters(clusters) != 0) {
            if (i>200) {
                break;
            }
            for(std::shared_ptr<triangle> tri : tris) {
                // Assign triangle to closest cluster
                float ctr = abs(tri->get_center()[a]-lookFrom[a]);
                float close = infinity;
                for (size_t c=0; c<clusters.size(); c++) {
                    float dist = abs(clusters[c]->center - ctr);
                    if(dist < close) {
                        close = dist;
                        tri->set_closest(c);
                    }
                }
                clusters[tri->get_closest()]->add(tri);
                // tri->set_mat(pickMat(tri->get_closest()));
            }
            for (std::shared_ptr<cluster> c : clusters) {
                c->recenter(a);
                c->com(a);
                c->clear();
            }
            i++;
        }
        for (std::shared_ptr<cluster> c : clusters) {
            std::cout << "final " << c->center << std::endl; // COMMENT
            vec3 com = c->cmass;
            // lights.push_back(com);
            std::cout << "(" << com[0] << ", " << com[1] << ", " << com[2] << ")" << std::endl; // COMMENT
        }

        cout << "NUM LIGHTS: " << lights.size() << endl;
        return lights;
    }


private:
    aabb bbox;  
};

#endif