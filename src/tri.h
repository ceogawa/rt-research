#ifndef TRI_H
#define TRI_H

#include "hittable.h"
#include "vec3.h"
#include "material.h"
#include "color.h"

class tri : public hittable {
    public:
        tri() {}
        tri(vec3 inV1, vec3 inV2, vec3 inV3, shared_ptr<material> m) : v1(inV1), v2(inV2), v3(inV3), matPtr(m) {
            set_bounding_box();
        }

    virtual void set_bounding_box() { bbox = aabb(v1, v2, v3); }

    virtual bool hit(
        const ray& r, interval ray_t, hit_record& rec) const;

    aabb bounding_box() const override { return bbox; }

    void reSet(vec3 v1In, vec3 v2In, vec3 v3In) {
        v1 = v1In;
        v2 = v2In;
        v3 = v3In; 
    }

    public:
        vec3 v1, v2, v3;
        shared_ptr<material> matPtr;
    private:
        aabb bbox;
};

//shirley pg 208
/*
bool tri::hit(const ray& r, double tMin, double tMax, hitRec& rec) const {
      auto col1 = v1-v2;
      auto col2 = v1-v3;
      auto col3 = r.direct();
      auto ans = v1-r.orig;
      auto ei_hf = col2.y()*col3.z() - col3.y()*col2.z();
      auto gf_di = col3.x()*col2.z() - col2.x()*col3.z();
      auto dh_eg = col2.x()*col3.y() - col3.x()*col2.y();
      auto ak_jb = col1.x()*ans.y() - ans.x()*col1.y();
      auto jc_al = ans.x()*col1.z() - col1.x()*ans.z();
      auto bl_kc = col1.y()*ans.z() - ans.y()*col1.z();

      auto M = (col1.x()*ei_hf + col1.y()*gf_di + col1.z()*dh_eg);
      
      auto tempT = (col2.z()*ak_jb + col2.y()*jc_al + col2.x()*bl_kc)/-1*M;

      if (tempT < tMin || tempT > tMax)
            return false;

      auto gamma = (col3.z()*ak_jb + col3.y()*jc_al + col3.x()*bl_kc)/M;

      if (gamma < 0 || gamma > 1) 
            return false;

      auto betaM = (ans.x()*ei_hf + ans.y()*gf_di + ans.z()*dh_eg)/M;
      
      if (betaM < 0 || betaM > 1-gamma) 
            return false;
                  
      rec.t = tempT;
      rec.p = r.rayHere(tempT);
      rec.matPtr = matPtr;
      rec.norm =  normMe(cross(col1, col2));
      return true;

}
*/

bool tri::hit(const ray& r, interval ray_t, hit_record& rec) const {
//from RTR page 750
    auto e1 = v2-v1;
    auto e2 = v3-v1;
    auto q = cross(r.direction(), e2);
    auto a = dot(e1, q);

    constexpr double kEpsilon = std::numeric_limits<double>::epsilon();

    //check 
    if (fabs(a) > kEpsilon/100) {
        auto f = 1.0/a;
        auto s = r.origin()-v1;
        auto alpha = f*dot(s, q);
        if (alpha < 0 )
            return false;
        auto rTemp = cross(s, e1);
        auto beta = f*dot(r.direction(), rTemp);
        if (beta < 0 || alpha+beta > 1.0) {
            return false;
        }
        auto temp =f*dot(e2, rTemp);

        if (temp < ray_t.max && temp > ray_t.min) {
            rec.t = temp;
            rec.p = r.at(temp);
            rec.mat = matPtr;
            rec.set_face_normal(r, unit_vector(cross(e1, e2)));
            return true;
        }
    }
    return false;
}

#endif