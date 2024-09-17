#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "rtweekend.h"
#include "hittable.h"
#include "aabb.h"
#include "mesh.h"
#include "triangle.h"
#include "cluster.h"
#include "contours.h"
#include "point.h"

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
    std::vector<point> extraLights;

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

    int determine_num_lights(size_t clusters_len){
        // may complicate number of lights to select based on density/size/span of cluster
        // for now, returns a certain percentage of the number of the number of pts per cluster, min 2 lights per cluster
        double weight = 0.01;
        int minlights = 2;
        return (clusters_len*weight < minlights) ? minlights : clusters_len*weight;
    }
    /* Pseudocode:
    1. Find bounds of scene (furthest point from camera in camera direction)
    2. Assign k centers to the planes of the scene perpendicular to the camera 
        at furthest/k intervals 
    3. Update k centers based on vertices added to each layer
    4. Find center of mass for each layer 
    5. Place lights at each, ensuring that there is an object in front of the light */

    void layer(point3 lookAt, point3 lookFrom, int k, int a, double intensity) {
         cout << "in layer intensity : " << intensity << endl;

        std::vector<std::shared_ptr<mesh>> meshObjects; // Meshes
        std::vector<std::shared_ptr<triangle>> tris; // Triangles
        std::vector<vector<vec3>> lights; // Centers of Mass
        // std::vector<std::shared_ptr<point>> extraLights;

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
            cout << "preintensity: " << intensity << endl;
            float currDist = 0;
            aabb currBbox = meshObjects[o]->bbox;
            point3 currCenter = currBbox.get_center();
            point3 currMax = currBbox.get_max();
            point3 currMin = currBbox.get_min();
            vec3 cam = unit_vector(lookAt - lookFrom);
            lights.clear();

            cout << "num normals for mesh: " << meshObjects[o]->normals.size() << endl;
            // cout << "num normal points of origin for mesh: " << meshObjects[o]->normals_origin->size() << endl;

            // create edges
            vector<vec3> contour_l = contour_lights(meshObjects[o]->vertices, meshObjects[o]->normals, cam);
            lights.push_back(contour_l);
            // for(size_t j = 0; j < c_lights.size(); j++){
            //     lights.push_back(c_lights[j]);
            // }

            cout << "after contours" << endl;

            vector<vec3> cluster_l;
            vector<vector<point3>> clusters(meshObjects[o]->cls.size());

            cout << "pre cluster lights" << endl;
            cout << "cls size: " << meshObjects[o]->cls.size() << endl;
            for(size_t i = 0; i < meshObjects[o]->cls.size(); i++){
                for(size_t j = 0; j < meshObjects[o]->cls[i].size(); j++){
                    // clusters contains groups of normal origins separated by cluster
                    // cout << "pre clusters pushback" << endl;
                    clusters[i].push_back((*meshObjects[o]->normals_origin)[j]);
                }
                // make the number of lights behind each cluster size dependent;
                size_t num_lights = determine_num_lights(clusters[i].size());
                cout << "determined num of backlights: " << num_lights << endl;
                // sort each list in ascending order, based on depth (deepest points in object selected first)
                sort(clusters[i].begin(), clusters[i].end(), sortClusters);
                for(size_t k = 0; k < (int)num_lights; k++){
                    // want to select a point early on in the cluster list (farther back in the obj pts so backlights are not visible)
                    int maxindx = (clusters[i].size()/8 < 6) ? 6 : clusters[i].size()/8;
                    int indx = random_int(0, maxindx);
                    // if the point in the back half of the cluster AND is in the back fifth of the bounding box, add a backlight
                    // if(clusters[i][indx][2] >= currBbox.get_max()[2]-((float)currBbox.get_min()[2]/(float)6)){
                        cluster_l.push_back(clusters[i][indx]);
                    // }
                }
                
            }
            // for(size_t k = 0; k < meshObjects[o]->clusters->size(); k++){
            //     sort((*meshObjects[o]->clusters)[k].begin(),(*meshObjects[o]->clusters)[k].end(), sortClusters);
            //     size_t num_lights = ((*meshObjects[o]->clusters)[k].size())*0.01;
            //     for(size_t l = 0; l < (int)num_lights; l++){
            //         int indx = random_int(0, (*meshObjects[o]->clusters)[k].size()/4);
            //         cluster_l.push_back((*meshObjects[o]->clusters)[k][indx]);
            //     }
            // } 
            lights.push_back(cluster_l);
            cout << "lights.size()" << lights.size() << endl;

            // auto moonlight   = make_shared<diffuse_light>(color(4, 7, 15));
            auto backlight   = make_shared<diffuse_light>(color(4, 4, 4));
            auto rimlight    = make_shared<diffuse_light>(color(2, 3, 6));
            float backlight_intensity = 0.11;

            for(size_t i = 0; i < lights.size(); i++){
                for(size_t j = 0; j < lights[i].size(); j++){
                    float rimlight_intensity = intensity;
                    if(i == 0){
                        double min = std::numeric_limits<double>::max();
                        bool check = false;

                        // Array of all planes to check
                        std::vector<std::array<double, 4>> planes = {currBbox.ff, currBbox.lf, currBbox.rf, currBbox.backf, currBbox.tf, currBbox.bottomf};

                        for (const auto& plane : planes) {
                            double d = abs(currBbox.pt_distance_plane(plane, lights[i][j]));
                            if (d < min) { min = d; }
                        }

                        // update min to be the distance to the nearest plane
                        // cout << "min distance to plane: " << min << endl;
                        double reduce = 0.1;
                        double enhance = 1.1;
                        double eps = 0.2;
                        // if(min >= 0.1){
                            // if the distance to the nearest plane is larger, the light is more towards the center of the object 
                            // so the intensity decreases
                            // if(check){
                                // if the nearest plane is the ff or backf, reduce the intensity (top bottom left right have higher intensity)
                                // rimlight_intensity = ((double)rimlight_intensity/(double)(min + eps))*reduce;
                            // }
                            // else{
                                // if(check){
                                //     rimlight_intensity = ((double)rimlight_intensity/(double)(min + eps))*enhance;
                                // }
                                // else{
                                    rimlight_intensity = ((double)rimlight_intensity/(double)(min + eps));
                                // }
                            // }
                        // }
                        // else{
                        //     rimlight_intensity = 0.13;
                        // }
                        
                        // else{
                        //     rimlight_intensity *=  enhance;
                        // }

                        rimlight_intensity = (rimlight_intensity > 0.12) ? 0.12 : (rimlight_intensity < 0.07) ? 0.07 : rimlight_intensity;
                        
                        cout << "intensity: " << rimlight_intensity << endl;
                        extraLights.push_back(point(lights[i][j], rimlight_intensity, rimlight, lookFrom));  
                    }
                    else{
                        // extraLights.push_back(point(lights[i][j], backlight_intensity, backlight, lookFrom));  
                    }
                }
            }
    
            /* ADD LIGHTS BEHIND EACH OBJECT */

            // if (meshObjects[o]->addLight != false) {
            //     float yDist = currBbox.y.max-currBbox.y.min;
            //     point3 pt = point3(currBbox.x.min-(currBbox.x.max-currBbox.x.min)/8, 
            //                     currBbox.y.min+(yDist*scale),
            //                     currCenter[2]);
            //     lights.push_back(pt);

            //     pt = point3(currBbox.x.min-(currBbox.x.max-currBbox.x.min)/8, 
            //                     currBbox.y.min+(yDist*scale2),
            //                     currBbox.z.min+(currBbox.z.max-currBbox.z.min)/6);
            //     lights.push_back(pt);

            //     pt = point3(currBbox.x.min-(currBbox.x.max-currBbox.x.min)/8, 
            //                     currBbox.y.min+(yDist*scale2),
            //                     currBbox.z.max-(currBbox.z.max-currBbox.z.min)/6);
            //     lights.push_back(pt);

            //     std::cout << "size: " << currMax[1]-currMin[1] << std::endl;
            //     std::cout << "Max: " << "(" << currMax[0] << ", " << currMax[1] << ", " << currMax[2] << ")" << std::endl; // COMMENT
            //     std::cout << "Min: " << "(" << currMin[0] << ", " << currMin[1] << ", " << currMin[2] << ")" << std::endl; // COMMENT
            //     std::cout << "Cen: " << "(" << currCenter[0] << ", " << currCenter[1] << ", " << currCenter[2] << ")" << std::endl; // COMMENT
            //     std::cout << "Out: " << "(" << pt[0] << ", " << pt[1] << ", " << pt[2] << ")" << std::endl; // COMMENT
            //     std::cout << "-" << std::endl;
            // } 
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
    }
    


private:
    aabb bbox;  

};

#endif