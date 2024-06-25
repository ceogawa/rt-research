#include <memory>
#include <vector>
#include <iostream>
#include "hittable.h"

class scene {
public:
    scene() {}
    scene(std::shared_ptr<hittable> object) { add(object); }
    
    void clear() {
        objects.clear();
        objects.shrink_to_fit();
    }

    void add(std::shared_ptr<hittable> object) {
        objects.push_back(object);
    }
    
    // virtual bool intersect(const ray& r, SurfaceInteraction &interaction) const;
    
public:
    std::vector<std::shared_ptr<hittable>> objects;
};
