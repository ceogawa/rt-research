// https://github.com/james-yoo/DBSCAN
// LICENSED BY james-yoo

#ifndef DBSCAN_H
#define DBSCAN_H

#include <vector>
#include <cmath>
#include "vec3.h"

#define UNCLASSIFIED -1
// #define CORE_POINT 1
// #define BORDER_POINT 2
// #define NOISE -2
#define SUCCESS 0
#define FAILURE -3

// #define FAILURE 0
// #define SUCCESS 1
// #define UNCLASSIFIED 2
// #define CORE_POINT 3
// // #define BORDER_POINT 2
// #define NOISE 4

class DBSCAN {
public:    
    DBSCAN(unsigned int minPts, float eps, vector<point3> face_points, vector<int> clusterids){  
        m_minPoints = minPts;
        m_epsilon = eps;
        m_points = face_points;
        cluster_ids = clusterids;
    }
    ~DBSCAN(){}

    
    vector<int> run()
    {
        int clusterID = 1;

        for(std::vector<vec3>::size_type i = 0; i < m_points.size(); i++){

            // std::clog << "\rNORMAL " << i << ' ' << std::flush;
            // cout << i << endl;

            if (cluster_ids[i] == UNCLASSIFIED){
                if (expandCluster(i, clusterID)){
                    // cluster_ids[i]++;
                    clusterID++;
                }
            }
            // cout << "num cluster id: " << clusterID << endl;
        }

        return cluster_ids;
    }

    vector<int> calculateCluster(point3 point){
        vector<int> clusterIndex;

        int index = 0;

        for (std::vector<vec3>::size_type i = 0; i < m_points.size(); i++){
            if ((calculateDistance(point, m_points[i]) <= m_epsilon) && (!(point == m_points[i]))){
                clusterIndex.push_back(index);
            }
            index++;
        } 
        return clusterIndex;
}   

    bool expandCluster(int i, int clusterID)
    {    
        point3 point = m_points[i];

        vector<int> clusterSeeds = calculateCluster(point);

        if (clusterSeeds.size() < m_minPoints){
            // cluster_ids[i] = NOISE;
            return false;
            // cout << "FALSE" << endl;
        }
        else
        {
            // cout << "TRUE" << endl;
            for(std::vector<vec3>::size_type i = 0; i < clusterSeeds.size(); i++)
            {
                cluster_ids[i] = clusterID;
                vector<int> clusterNeighbors = calculateCluster(m_points[clusterSeeds[i]]);

                if (clusterNeighbors.size() >= m_minPoints)
                {
                    for(std::vector<vec3>::size_type k = 0; k < clusterNeighbors.size(); k++){
                        if(cluster_ids[k] == UNCLASSIFIED){
                            clusterSeeds.push_back(k);
                        }
                        cluster_ids[k] = clusterID;
                    }
                }
            }

            return true;
        }
    }

    inline double calculateDistance(point3 pointCore, point3 pointTarget )
    {
        vec3 dist = distance(pointCore, pointTarget);
        return dist.length();
    }

        
    
    void changeMaterial(){

        for(std::vector<vec3>::size_type i = 0; i < cluster_ids.size(); i++){
            cout << cluster_ids[i] << endl;
        }
        
    }

    // int getTotalPointSize() {return m_pointSize;}
    int getMinimumClusterSize() {return m_minPoints;}
    int getEpsilonSize() {return m_epsilon;}
    
public:
    // shared_ptr<mesh> m;
    vector<point3> m_points;
    vector<int> cluster_ids;
    
    
private:    
    // unsigned int m_pointSize;
    unsigned int m_minPoints;
    float m_epsilon;
};

#endif // DBSCAN_H