#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include "vec3.h"
#include "color.h"

class point_light {
public:
    point_light(const point3& pos, const float& intensity)
        : position(pos), intensity(intensity) {}

    point3 getPos() { return position; }
    float getIntensity() { return intensity; }
private:
    point3 position;
    float intensity;
};

#endif
