#ifndef HITTABLE_LIST_SPHERE_H
#define HITTABLE_LIST_SPHERE_H

#include "rtweekend.h"
#include "hittable.h"
#include "aabb.h"
#include "mesh.h"
#include "triangle.h"
#include "sphere.h"
#include "cluster.h"
#include "cluster_sphere.h"

#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <random>

using namespace std;

class hittable_list_sphere : public hittable {
  public:
    std::vector<shared_ptr<hittable>> objects;

    void hittable_list() {}
    void hittable_list(shared_ptr<hittable> object) { add(object); }

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

    static bool compareSphere(const std::shared_ptr<sphere>& s1, const std::shared_ptr<sphere>& s2) 
    {   
        point3 c1 = s1->get_center();
        point3 c2 = s2->get_center();
        return (c1[0] < c2[0]);
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

    bool check_clusters(std::vector<std::shared_ptr<cluster_sphere>> clusters) {
        for (std::shared_ptr<cluster_sphere> c : clusters) {
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

    std::vector<vec3> layer(point3 lookAt, point3 lookFrom, int k, int a){
        //
        vector<shared_ptr<sphere>> sphs;
        vector<vec3> coms; // Centers of Mass

        float maxDist = -infinity; // only X value matters

        for (const auto& obj : objects) {
            // Check if the current object is a sphere
            std::shared_ptr<sphere> sph = std::dynamic_pointer_cast<sphere>(obj);
            if (sph) {
                sphs.push_back(sph);
            }
        }
        
        for(size_t i = 0; i < sphs.size(); i++){
            float currDist = 0;
            // lookFrom.x  = lookFrom[a] where a = 0
            point3 currC= sphs[i]->get_center();
            currDist = abs(currC[a])-lookFrom[a];

            if (currDist > maxDist) {
                maxDist = currDist;
            }
        }

        sort(sphs.begin(), sphs.end(), compareSphere);

        // Initialize centers for k-means
        float inc = maxDist/(k+1);
        cout << "maxDist: " << maxDist << endl;
        std::cout << "inc: " << inc << std::endl;
        std::vector<std::shared_ptr<cluster_sphere>> clusters;
        for (float i=inc; i<=k*inc; i+=inc) {
            std::cout << "init " << i << std::endl; // COMMENT
            clusters.push_back(std::make_shared<cluster_sphere>(abs(i-lookFrom[a])));
        }

        auto red   = make_shared<lambertian>(color(.65, .05, .05));
        auto purple   = make_shared<lambertian>(color(.65, .05, .65));
        auto blue = make_shared<lambertian>(color(.1, .1, .8));
        auto white = make_shared<lambertian>(color(.73, .73, .73));
        auto green = make_shared<lambertian>(color(.12, .45, .15));
        // for(std::shared_ptr<sphere> sph : sphs) {
        //         // Assign triangle to closest cluster
        //         float ctr = (sph->get_center()[a]-lookFrom[a]);
        //         float close = infinity;
        //         for (size_t c=0; c<clusters.size(); c++) {
        //             float dist = abs(clusters[c]->center - ctr);
        //             if(dist < close) {
        //                 close = dist;
        //                 sph->set_closest(c);
        //             }
        //         }
        //         clusters[sph->get_closest()]->add(sph);
        //         // tri->set_mat(pickMat(tri->get_closest()))
        // }

        int i=0;
        while(check_clusters(clusters) != 0) {
            if (i>200) {
                break;
            }
            for(std::shared_ptr<sphere> sph : sphs) {
                // Assign triangle to closest cluster
                float ctr = (sph->get_center()[a]-lookFrom[a]);
                float close = infinity;
                for (size_t c=0; c<clusters.size(); c++) {
                    // float dist = abs(clusters[c]->center - ctr);
                    float dist = abs(clusters[c]->center - sph->get_center()[a]);
                    if(dist < close) {
                        // cout << "dist < closest pt    NEW CLUSTER: " << c << endl;
                        close = dist;
                        sph->set_closest(c);
                        if(c == 0){
                            sph->set_material(red);
                            sph->set_radius(8);
                        }
                        else if(c == 1){
                            sph->set_material(purple);
                            sph->set_radius(12);
                        }
                        else if(c == 2){
                            sph->set_material(blue);
                            sph->set_radius(17);

                        }
                        else{
                            sph->set_material(green);
                            sph->set_radius(21);    
                        }
                       
                    }

                }
                clusters[sph->get_closest()]->add(sph);
            }

            for (std::shared_ptr<cluster_sphere> c : clusters) {
                c->recenter(a);
                c->com_sphere();
                // c->clear();
            }
            i++;
        }

        // for(auto sph : sphs){
        //     switch(sph->get_closest()){
        //         case 0:
        //             sph->set_material(red);
        //         case 1:
        //             sph->set_material(blue);
        //         // case 2:
        //         //     sph->set_material(green);
        //     }
        // }
        //  std::clog << "\rCLUSTER: " << c << ' ' << std::flush;

        for (std::shared_ptr<cluster_sphere> c : clusters) {
            std::cout << "final " << c->center << std::endl; // COMMENT
            vec3 com = c->cmass;
            coms.push_back(com);
            std::cout << "(" << com[0] << ", " << com[1] << ", " << com[2] << ")" << std::endl; // COMMENT
        }
        
        return coms;

    }

private:
    aabb bbox;  
};

#endif