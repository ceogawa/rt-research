#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"

class aabb {
  public:
    interval x, y, z;

    aabb() {} // The default AABB is empty, since intervals are empty by default.

    aabb(const interval& ix, const interval& iy, const interval& iz)
      : x(ix), y(iy), z(iz) {
        min = point3(x.min, y.min, z.min);
        max = point3(x.max, y.max, z.max);
        pad_to_minimums();
        find_center();
      }

    aabb(const point3& a, const point3& b) {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order.
        x = interval(fmin(a[0],b[0]), fmax(a[0],b[0]));
        y = interval(fmin(a[1],b[1]), fmax(a[1],b[1]));
        z = interval(fmin(a[2],b[2]), fmax(a[2],b[2]));
        min = point3(x.min, y.min, z.min);
        max = point3(x.max, y.max, z.max);
        pad_to_minimums();
        find_center();
    }

    aabb(const point3& a, const point3& b, const point3& c) {
        x = interval(fmin(fmin (a[0], b[0]), c[0]), fmax(fmax(a[0], b[0]), c[0]));
        y = interval(fmin(fmin (a[1], b[1]), c[1]), fmax(fmax(a[1], b[1]), c[1]));
        z = interval(fmin(fmin (a[2], b[2]), c[2]), fmax(fmax(a[2], b[2]), c[2]));
        min = point3(x.min, y.min, z.min);
        max = point3(x.max, y.max, z.max);
        pad_to_minimums();
        find_center();
    }


    aabb(const aabb& box0, const aabb& box1) {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
        min = point3(x.min, y.min, z.min);
        max = point3(x.max, y.max, z.max);
        find_center();
    }

    aabb pad() {
        // Return an AABB that has no side narrower than some delta, padding if necessary.
        double delta = 0.0001;
        interval new_x = (x.size() >= delta) ? x : x.expand(delta);
        interval new_y = (y.size() >= delta) ? y : y.expand(delta);
        interval new_z = (z.size() >= delta) ? z : z.expand(delta);

        return aabb(new_x, new_y, new_z);
    }

    const interval& axis(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const ray& r, interval ray_t) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1 / r.direction()[a];
            auto orig = r.origin()[a];

            auto t0 = (axis(a).min - orig) * invD;
            auto t1 = (axis(a).max - orig) * invD;

            if (invD < 0)
                std::swap(t0, t1);

            if (t0 > ray_t.min) ray_t.min = t0;
            if (t1 < ray_t.max) ray_t.max = t1;

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }

    int longest_axis() const {
        // Returns the index of the longest axis of the bounding box.

        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }

    void find_center() {
        center[0] = x.min + (x.max-x.min)/2;
        center[1] = y.min + (y.max-y.min)/2;
        center[2] = z.min + (z.max-z.min)/2;
    }
    
    point3 get_center() { return center; }
    point3 get_max() { return max; }
    point3 get_min() { return min; }

    static const aabb empty, universe;

  private:
    point3 center;
    point3 max;
    point3 min;

    void pad_to_minimums() {
        // Adjust the AABB so that no side is narrower than some delta, padding if necessary.

        double delta = 0.0001;
        if (x.size() < delta) x = x.expand(delta);
        if (y.size() < delta) y = y.expand(delta);
        if (z.size() < delta) z = z.expand(delta);
    }
};

const aabb aabb::empty    = aabb(interval::empty,    interval::empty,    interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

aabb operator+(const aabb& bbox, const vec3& offset) {
    return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

aabb operator+(const vec3& offset, const aabb& bbox) {
    return bbox + offset;
}


#endif