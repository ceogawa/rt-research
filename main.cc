#include "rtweekend.h"
#include "color.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "point.h"
#include "quad.h"
#include "triangle.h"
#include "mesh.h"
#include "bvh.h"
#include "texture.h"
#include "tiny_obj_loader.h"

/* ORIGINAL SCENES FROM BOOK */

void finalscene(char* fname, float intensity) {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    // auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    // world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    //world.add(make_shared<sphere>(center, 0.2, sphere_material));
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 500;
    cam.samples_per_pixel = 500; // 500;
    cam.max_depth         = 50; // 50;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world, fname);
}


void quads(char* fname, float intensity) {
    hittable_list world;

    // Materials
    auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov     = 80;
    cam.lookfrom = point3(0,0,9);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, fname);
}

void simple_light(char* fname, float intensity) {
    hittable_list world;

    // auto pertext = make_shared<noise_texture>(4);
    // world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    // world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(color(0.1, 0.2, 0.5))));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(color(0.1, 0.2, 0.5))));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    world.add(make_shared<sphere>(point3(0,7,0), 2, difflight));
    world.add(make_shared<quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), difflight));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 20;
    cam.lookfrom = point3(26,3,6);
    cam.lookat   = point3(0,2,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, fname);
}

void simple_light2(char* fname, float intensity) {
    hittable_list world;

    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(color(0.1, 0.2, 0.5))));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(color(0.1, 0.2, 0.5))));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    world.add(make_shared<sphere>(point3(0,7,0), 2, difflight));
    world.add(make_shared<quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), difflight));

    auto bunny = make_shared<mesh>("models/bunny.obj", make_shared<lambertian>(color(0.1, 0.2, 0.5)), vec3(0,0,0), 1.0, false);
    world.add(bunny);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 100;
    cam.samples_per_pixel = 40;
    cam.max_depth         = 20;
    cam.background        = color(0,0,0);

    cam.vfov     = 20;
    cam.lookfrom = point3(26,3,6);
    cam.lookat   = point3(0,2,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, fname);
}

void cornell_box(char* fname, float intensity) {
    hittable_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    world.add(qbox(point3(130, 0, 65), point3(295, 165, 230), white));
    world.add(qbox(point3(265, 0, 295), point3(430, 330, 460), white));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 100; //600
    cam.samples_per_pixel = 200;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, fname);
}

void teapot(char* fname, float intensity) {
    hittable_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    // world.add(qbox(point3(130, 0, 65), point3(295, 165, 230), white));
    world.add(qbox(point3(265, 0, 295), point3(430, 330, 460), white));

    world.add(make_shared<sphere>(point3(100,70,150), 100, make_shared<lambertian>(color(0.1, 0.2, 0.5))));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 200; //600
    cam.samples_per_pixel = 200;
    cam.max_depth         = 30;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, fname);
}


/* REAL SCENES */

void camp(char* fname, float intensity) {
    hittable_list world;
    std::vector<vec3> cms;

    // colors
    auto difflight   = make_shared<diffuse_light>(color(4, 4, 4));
    auto firelight   = make_shared<diffuse_light>(color(15, 10, 8));
    auto grey        = make_shared<lambertian>(color(0.2, 0.2, 0.4));
    auto brown       = make_shared<lambertian>(color(0.4, 0.2, 0.2));
    auto green       = make_shared<lambertian>(color(0.5, 1.0, 0.5));
    auto orange      = make_shared<lambertian>(color(0.7, 0.4, 0.3));
    auto purple      = make_shared<lambertian>(color(0.5, 0.3, 0.6));
    auto yellow      = make_shared<lambertian>(color(0.8, 0.6, 0.6));
    auto white       = make_shared<lambertian>(color(0.8, 0.8, 0.8));

    // load meshes
    auto skeleton  = make_shared<mesh>("models/nskeleton.obj", white, vec3(4,1,-2), 0.5, true);
    auto campfire  = make_shared<mesh>("models/campfire.obj", firelight, vec3(4,0.6,0), 1.0, false);

    auto tree1     = make_shared<mesh>("models/btree1.obj", green, vec3(0,4*0.8,0), 0.8, true); //C
    auto tree2     = make_shared<mesh>("models/btree1.obj", green, vec3(-1,4,2), 1, true); //B
    auto tree3     = make_shared<mesh>("models/btree1.obj", green, vec3(-1,4*0.6,4), 0.6, true); //A
    auto tree4     = make_shared<mesh>("models/btree1.obj", green, vec3(1,4*0.6,-5), 0.6, true); 
    auto tree5     = make_shared<mesh>("models/btree1.obj", green, vec3(1,4*0.9,-4), 0.9, true); 
    auto tree6     = make_shared<mesh>("models/btree1.obj", green, vec3(3,4*0.5,-2), 0.5, true);
    auto tree7     = make_shared<mesh>("models/btree1.obj", green, vec3(5,4,-6), 1.0, true);
    auto tree8     = make_shared<mesh>("models/btree1.obj", green, vec3(6,4,5), 1.0, true);

    auto rock1     = make_shared<mesh>("models/rock.obj", grey, vec3(-4,0.5,-1), 0.5, true);
    auto rock2     = make_shared<mesh>("models/rock.obj", grey, vec3(3,0.5,4.5), 0.5, true);
    auto rock3     = make_shared<mesh>("models/rock.obj", grey, vec3(4,0.5,-4), 0.5, true);
    
    /* not added */
    auto hya1      = make_shared<mesh>("models/hyacinth.obj", purple, vec3(4,0.5,-1), 2.0, false);
    auto daff1     = make_shared<mesh>("models/daffodil.obj", yellow, vec3(5,0.5,1), 2.0, false);
    auto grass1    = make_shared<mesh>("models/grass.obj", green, vec3(6,0.25,-3), 1.0, false);
    auto grass2    = make_shared<mesh>("models/grass.obj", green, vec3(3,0.25,-2), 1.0, false);
    auto grass3    = make_shared<mesh>("models/grass.obj", green, vec3(5,0.25,2), 1.0, false);
    auto grass4    = make_shared<mesh>("models/grass.obj", green, vec3(7,0.25,4), 1.0, false);

    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, brown));
    world.add(campfire);
    world.add(skeleton);

    world.add(tree1);
    world.add(tree2);
    world.add(tree3);
    world.add(tree4);
    world.add(tree5);
    world.add(tree6);
    world.add(tree7);
    world.add(tree8);
    
    world.add(rock1);
    world.add(rock2);
    world.add(rock3);
    // world.add(hya1);
    // world.add(daff1);
    world.add(grass1);
    world.add(grass2);
    world.add(grass3);
    world.add(grass4);

    world.add(make_shared<sphere>(point3(-15,8,-3), 1.0, difflight));

    point3 lookFrom = point3(26,3,0);
    point3 lookAt   = point3(0,3,0);

    cms = world.layer(lookFrom, lookAt, 3, 0);

    std::cout << "len: " << cms.size() << std::endl;

    // world.clear();
    // world.add(make_shared<sphere>(point3(0,-1000,0), 1000, brown));

    // std::vector<std::shared_ptr<sphere>> extraLights;
    std::vector<std::shared_ptr<point>> extraLights;
    for(vec3 cm : cms) {
        // extraLights.push_back(make_shared<sphere>(cm, intensity, difflight));
        std::cout << "light (" << cm[0] << ", " << cm[1] << ", " << cm[2] << ")" << std::endl;
        extraLights.push_back(make_shared<point>(cm, intensity, difflight, lookFrom));
    }
    // for(std::shared_ptr<sphere> light : extraLights) {
    for(std::shared_ptr<point> light : extraLights) {
        world.add(light);
    }

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 600; // 400
    cam.samples_per_pixel = 100; // 100
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);
    // cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = lookFrom;
    cam.lookat   = lookAt;
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, fname);
}

void simple_camp(char* fname, float intensity) {
    hittable_list world;
    std::vector<vec3> cms;

    // colors
    auto difflight   = make_shared<diffuse_light>(color(4, 4, 4));
    // auto ptlight     = make_shared<point_light>(color(4, 4, 4));
    auto firelight   = make_shared<diffuse_light>(color(15, 10, 8));
    auto grey        = make_shared<lambertian>(color(0.2, 0.2, 0.4));
    auto brown       = make_shared<lambertian>(color(0.4, 0.2, 0.2));
    auto green       = make_shared<lambertian>(color(0.5, 1.0, 0.5));
    auto orange      = make_shared<lambertian>(color(0.7, 0.4, 0.3));
    auto purple      = make_shared<lambertian>(color(0.6, 0.3, 0.6));
    auto yellow      = make_shared<lambertian>(color(0.8, 0.7, 0.6));

    // load meshes
    auto campfire  = make_shared<mesh>("models/sphere2.obj", firelight, vec3(6,0.4,0), 0.6, false);

    auto tree1     = make_shared<mesh>("models/sphere2.obj", green, vec3(0,0.8,0), 0.8, true);
    auto tree2     = make_shared<mesh>("models/sphere2.obj", green, vec3(-1,1,2), 1, true); 
    auto tree3     = make_shared<mesh>("models/sphere2.obj", green, vec3(-1,0.6,4), 0.6, true); 
    auto tree4     = make_shared<mesh>("models/sphere2.obj", green, vec3(1,0.6,-5), 0.6, true); 
    auto tree5     = make_shared<mesh>("models/sphere2.obj", green, vec3(1,0.9,-3), 0.9, true); 
    auto tree6     = make_shared<mesh>("models/sphere2.obj", green, vec3(3,0.5,-1), 0.5, true);
    auto tree7     = make_shared<mesh>("models/sphere2.obj", green, vec3(5,1,-7), 1.0, true);
    auto tree8     = make_shared<mesh>("models/sphere2.obj", green, vec3(6,1,6), 1.0, true);
    auto rock1     = make_shared<mesh>("models/sphere2.obj", grey, vec3(1,0.5,-1), 0.5, true);
    auto rock2     = make_shared<mesh>("models/sphere2.obj", grey, vec3(4,0.5,4), 0.5, true);

    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, brown));
    world.add(campfire);
    world.add(tree1);
    world.add(tree2);
    world.add(tree3);
    world.add(tree4);
    world.add(tree5);
    world.add(tree6);
    world.add(rock1);
    world.add(rock2);
    world.add(make_shared<sphere>(point3(-15,8,-3), 1.0, difflight));

    point3 lookFrom = point3(26,3,0);
    point3 lookAt   = point3(0,3,0);

    cms = world.layer(lookFrom, lookAt, 4, 0);

    std::vector<std::shared_ptr<point>> extraLights;
    for(vec3 cm : cms) {
        extraLights.push_back(make_shared<point>(cm, intensity, difflight, lookFrom));
    }
    for(std::shared_ptr<point> light : extraLights) {
        world.add(light);
    }

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 600; // 400
    cam.samples_per_pixel = 100; // 100
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);
    // cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = lookFrom;
    cam.lookat   = lookAt;
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, fname);
}

void beach(char* fname, float intensity) {
    hittable_list world;
    std::vector<vec3> cms;

    // colors
    auto difflight   = make_shared<diffuse_light>(color(4, 4, 4));
    auto firelight   = make_shared<diffuse_light>(color(15, 10, 8));
    auto red         = make_shared<lambertian>(color(0.8, 0.2, 0.0));
    auto orange      = make_shared<lambertian>(color(0.7, 0.3, 0.0));
    auto yellow      = make_shared<lambertian>(color(1.0, 0.9, 0.4));
    auto green       = make_shared<lambertian>(color(0.5, 1.0, 0.5));
    auto purple      = make_shared<lambertian>(color(0.5, 0.3, 0.6));
    auto white       = make_shared<lambertian>(color(0.8, 0.8, 0.8));
    auto grey        = make_shared<lambertian>(color(0.2, 0.2, 0.4));
    auto brown       = make_shared<lambertian>(color(0.4, 0.2, 0.2));

    // load meshes
    auto diver      = make_shared<mesh>("models/beach/diver.obj", orange, vec3(6,1,-4), 0.5, true);
    auto head       = make_shared<sphere>(point3(6,1.5,-4), 0.38, orange);
    auto crab       = make_shared<mesh>("models/beach/crab.obj", red, vec3(10,1.2,2), 0.8, false);

    auto ptree1     = make_shared<mesh>("models/beach/palm.obj", green, vec3(3,3.5,6), 0.5, true);
    auto coco1      = make_shared<sphere>(point3(3.5,6.0,5.9), 0.25, brown);
    auto coco2      = make_shared<sphere>(point3(3.5,6.0,5.3), 0.25, brown);
    auto ptree2     = make_shared<mesh>("models/beach/palm.obj", green, vec3(0,3.5,2), 0.5, true);
    auto coco3      = make_shared<sphere>(point3(0.5,5.5,2.2), 0.25, brown);
    auto coco4      = make_shared<sphere>(point3(0.5,5.5,1.6), 0.25, brown);
    auto ptree3     = make_shared<mesh>("models/beach/palm.obj", green, vec3(-3,3.5,-2), 0.5, true);
    auto coco5      = make_shared<sphere>(point3(-2.5,5.8,-2.4), 0.25, brown);
    auto coco6      = make_shared<sphere>(point3(-2.5,5.8,-1.8), 0.25, brown);

    auto rock1     = make_shared<mesh>("models/rock.obj", grey, vec3(-4,0.1,-1), 0.1, false);
    auto rock2     = make_shared<mesh>("models/rock.obj", grey, vec3(3,0.1,4.5), 0.1, false);
    auto rock3     = make_shared<mesh>("models/rock.obj", grey, vec3(3.5,0.1,-5), 0.1, false);

    auto grass1    = make_shared<mesh>("models/grass.obj", green, vec3(6,0.25,-3), 1.0, false);
    auto grass2    = make_shared<mesh>("models/grass.obj", green, vec3(3,0.25,-2), 1.0, false);
    auto grass3    = make_shared<mesh>("models/grass.obj", green, vec3(5,0.25,2), 1.0, false);
    auto grass4    = make_shared<mesh>("models/grass.obj", green, vec3(7,0.25,4), 1.0, false);

    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, yellow));
    world.add(diver);
    world.add(head);

    world.add(ptree1);
    world.add(ptree2);
    world.add(ptree3);
    world.add(coco1);
    world.add(coco2);
    world.add(coco3);
    world.add(coco4);
    world.add(coco5);
    world.add(coco6);
    world.add(crab);
    
    world.add(rock1);
    world.add(rock2);
    world.add(rock3);

    world.add(grass1);
    world.add(grass2);
    world.add(grass3);
    world.add(grass4);

    world.add(make_shared<sphere>(point3(-15,7,-4), 1.0, difflight));

    point3 lookFrom = point3(26,3,0);
    point3 lookAt   = point3(0,3,0);

    cms = world.layer(lookFrom, lookAt, 3, 0);

    // world.clear();
    // world.add(make_shared<sphere>(point3(0,-1000,0), 1000, yellow));
    std::cout << "len: " << cms.size() << std::endl;

    // std::vector<std::shared_ptr<sphere>> extraLights;
    std::vector<std::shared_ptr<point>> extraLights;
    for(vec3 cm : cms) {
        // extraLights.push_back(make_shared<sphere>(cm, intensity, difflight));
        extraLights.push_back(make_shared<point>(cm, intensity, difflight, lookFrom));
    }
    for(std::shared_ptr<point> light : extraLights) {
    // for(std::shared_ptr<sphere> light : extraLights) {
        world.add(light);
    }

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 200; // 600
    cam.samples_per_pixel = 50; // 100
    cam.max_depth         = 20;
    cam.background        = color(0,0,0);
    // cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = lookFrom;
    cam.lookat   = lookAt;
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, fname);
}

int main(int argc, char** argv) {
    float intensity;
    if (argc !=3) {
        std::cerr << "Usage: ./raytracer filename.ppm intensity" << std::endl;
    }
    intensity = std::stof(argv[2]);
    if ((intensity < 0) || (intensity > 1)) {
        std::cerr << "Intensity must be a floating point value between 0 and 1." << std::endl;
    }
    switch (6) {
        case 1:  finalscene(argv[1], intensity); break;
        case 2:  simple_light2(argv[1], intensity); break;
        case 3:  quads(argv[1], intensity); break;
        case 4:  cornell_box(argv[1], intensity); break;
        // case 5:  bunny(argv[1], intensity); break;
        case 6:  camp(argv[1], intensity); break;
        case 7:  simple_camp(argv[1], intensity); break;
        case 8:  beach(argv[1], intensity); break;
        case 9: teapot(argv[1], intensity); break;
    }
}