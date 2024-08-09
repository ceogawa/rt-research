// https://github.com/james-yoo/DBSCAN
// LICENSED BY james-yoo

#ifndef DBSCAN_H
#define DBSCAN_H

#include <vector>
#include <cmath>
#include "vec3.h"
#include "mesh.h"

#define UNCLASSIFIED -1
#define CORE_POINT 1
#define BORDER_POINT 2
#define NOISE -2
#define SUCCESS 0
#define FAILURE -3

using namespace std;

class DBSCAN {
public:    
    DBSCAN(unsigned int minPts, float eps, shared_ptr<mesh> msh){  
        m_minPoints = minPts;
        m_epsilon = eps;
        m = msh;
    }
    ~DBSCAN(){}

    int run();
    vector<int> calculateCluster(point3 point);
    int expandCluster(point3 point, int clusterID, int i);
    inline double calculateDistance(point3* pointCore, point3* pointTarget);

    // int getTotalPointSize() {return m_pointSize;}
    int getMinimumClusterSize() {return m_minPoints;}
    int getEpsilonSize() {return m_epsilon;}
    
public:
    shared_ptr<mesh> m;
    // vector<point3> m_points;
    
    
private:    
    // unsigned int m_pointSize;
    unsigned int m_minPoints;
    float m_epsilon;
};

#endif // DBSCAN_H
