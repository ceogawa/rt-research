#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <cmath>

#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"

class triangle : public hittable {
  public:
    triangle(const point3& _v0, const point3& _v1, const point3& _v2, shared_ptr<material> m)
      : v0(_v0), v1(_v1), v2(_v2), mat(m)
    {
        normal = unit_vector(cross(v1-v0, v2-v0));

        D = dot(normal, v0);

        set_bounding_box();

        center = point3((bbox.x.max-bbox.x.min/2)+bbox.x.min, 
                        (bbox.y.max-bbox.y.min/2)+bbox.y.min, 
                        (bbox.z.max-bbox.z.min/2)+bbox.z.min);
    }

    vec3 getv0() { return v0; }
    vec3 getv1() { return v1; }
    vec3 getv2() { return v2; }

    virtual void set_bounding_box() { bbox = aabb(v0, v1, v2).pad(); }
    aabb bounding_box() const override { return bbox; }
    point3 get_center() { return center; }

    void reSet(const point3& _v0, const point3& _v1, const point3& _v2) {
        normal = unit_vector(cross(v1 - v0, v2 - v0));
        D = dot(normal, v0);

        set_bounding_box();
    }

    virtual void set_mat(shared_ptr<material> m) { mat = m; }
    virtual void set_closest(float c) { closest = c; }
    int get_closest() { return closest; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const {

      //  https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection.html 

        vec3 v0v1 = v1 - v0;
        vec3 v0v2 = v2 - v0;
        vec3 pvec = cross(r.direction(), v0v2); 
        float det = dot(v0v1, pvec); 

        constexpr double kEpsilon = std::numeric_limits<double>::epsilon();

        // if the determinant is negative, the triangle is 'back facing'
        // if the determinant is close to 0, the ray misses the triangle
        if (det < (kEpsilon)) { return false; }
        // if (fabs(det) < kEpsilon) { return false; }

        double invDet = 1 / det;
 
        vec3 tvec = r.origin() - v0;
        double u = dot(tvec, pvec) * invDet; 
        if (u < 0 || u > 1) { return false; }

        vec3 qvec = cross(tvec, v0v1);     
        double v = dot(r.direction(), qvec) * invDet; 
        if (v < 0 || u + v > 1) { return false; }
    
        double t = dot(v0v2, qvec) * invDet;

        // For multiple objects, we take the closest one
        if (t < ray_t.min || ray_t.max < t) { 
          return false; 
        }

        // Hit Record
        rec.u = u;
        rec.v = v;
        rec.t = t;	
        // auto normal = cross(v0v1, v0v2);
        rec.set_face_normal(r, normal);
        rec.mat = mat;
        rec.p = r.at(t);

    return true;
    }

    // bool hit(const ray& r, interval ray_t, hit_record& rec) const {
    //     float area2 = normal.length();
    //     float ndotr = dot(normal, r.direction());
    //     constexpr double kEpsilon = std::numeric_limits<double>::epsilon();

    //     if (fabs(ndotr) < kEpsilon) // Almost 0
    //         return false; // They are parallel, so they don't intersect!
        
    //     float d = dot(-1*normal, v0);
    //     float t = -1*(dot(normal, r.origin())+d)/ndotr;

    //     if (t < 0) return false; // The triangle is behind

    //     vec3 p = r.origin() + t * r.direction();

    //     vec3 c;

    //     vec3 edge0 = v1 - v0; 
    //     vec3 vp0 = p - v0;
    //     c = cross(edge0, vp0);
    //     if (dot(c, normal) < 0) return false; // P is on the right side

    //     vec3 edge1 = v2 - v1; 
    //     vec3 vp1 = p - v1;
    //     c = cross(edge1, vp1);
    //     if (dot(c, normal) < 0) return false; // P is on the right side
        
    //     vec3 edge2 = v0 - v2; 
    //     vec3 vp2 = p - v2;
    //     c = cross(edge2, vp2);
    //     if (dot(c, normal) < 0) return false; // P is on the right side

    //     rec.set_face_normal(r, normal);
    //     rec.mat = mat;
    //     rec.p = r.at(t);
    //     return true;
    // }

    virtual bool is_interior(double a, double b, double c, hit_record& rec) const {
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true.

        if ((a < 0) || (1 < a) || (b < 0) || (1 < b) || (c < 0) || (1 < c))
            return false;

        rec.t = a;
        rec.u = b;
        rec.v = c;
        return true;
    }

  private:
    point3 v0, v1, v2;
    point3 center;
    shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
    vec3 w;
    int closest;
};

#endif