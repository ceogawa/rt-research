#ifndef POINT_H
#define POINT_H

#include "rtweekend.h"
#include "hittable.h"

class point : public hittable {
  public:

    point(point3 _center, double _radius, shared_ptr<material> _material, point3 _camPos)
    : center1(_center), radius(_radius), mat(_material), cam(_camPos) 
    { 
      auto rvec = vec3(radius, radius, radius);
      bbox = aabb(center1 - rvec, center1 + rvec);
    }
    
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
      point3 center = center1;
      vec3 oc = r.origin() - center;
      auto a = r.direction().length_squared();
      auto half_b = dot(oc, r.direction());
      auto c = oc.length_squared() - radius*radius;

      auto discriminant = half_b*half_b - a*c;
      if (discriminant < 0) return false;
      auto sqrtd = sqrt(discriminant);

      // Find the nearest root that lies in the acceptable range.
      auto root = (-half_b - sqrtd) / a;
      if (!ray_t.surrounds(root)) {
        root = (-half_b + sqrtd) / a;
        if (!ray_t.surrounds(root))
          return false;
      }

      if ((r.origin()[0] == cam[0]) &&
          (r.origin()[1] == cam[1]) &&
          (r.origin()[2] == cam[2])) { return false; }

      rec.t = root;
      rec.p = r.at(rec.t);
      vec3 outward_normal = (rec.p - center) / radius;
      rec.set_face_normal(r, outward_normal);
      rec.mat = mat;
      return true;
    }

  aabb bounding_box() const override { return bbox; }

  private:
    point3 center1;
    double radius;
    shared_ptr<material> mat;
    point3 cam;
    aabb bbox;
    vec3 center_vec;

};

#endif