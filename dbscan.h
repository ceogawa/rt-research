// https://github.com/james-yoo/DBSCAN
// LICENSED BY james-yoo

#ifndef DBSCAN_H
#define DBSCAN_H

#include <vector>
#include <cmath>
#include "vec3.h"
#include "mesh.h"

#define FAILURE 0
#define SUCCESS 1
#define UNCLASSIFIED 2
#define CORE_POINT 3
// #define BORDER_POINT 2
#define NOISE 4

class DBSCAN {
public:    
    DBSCAN(unsigned int minPts, float eps, vector<point3> face_points, vector<int> clusterids){  
        m_minPoints = minPts;
        m_epsilon = eps;
        m_points = face_points;
        cluster_ids = clusterids;
    }
    ~DBSCAN(){}

    
    int run()
    {
        int clusterID = 1;

        for(int i = 0; i < m_points.size(); i++){

            // std::clog << "\rNORMAL " << i << ' ' << std::flush;
            cout << i << endl;

            if (cluster_ids[i] == UNCLASSIFIED){
                if (expandCluster(i)){
                    cluster_ids[i]++;
                }
            }
        }

        return 0;
    }

    vector<int> calculateCluster(point3 point)
{
    vector<int> clusterIndex;

    for (int i = 0; i < m_points.size(); i++){
        if (calculateDistance(point, m_points[i]) <= m_epsilon){
            clusterIndex.push_back(i);
        }
    }

    return clusterIndex;
}

    int expandCluster(int i)
    {    
        point3 point = m_points[i];
        int clusterID = cluster_ids[i];

        vector<int> clusterSeeds = calculateCluster(point);

        if (clusterSeeds.size() < m_minPoints)
        {
            cluster_ids[i] = NOISE;
            return FAILURE;
        }
        else
        {
            int indexCorePoint = 0;

            for(int j = 0; j < clusterSeeds.size(); j++){
                cluster_ids[j] = clusterID;
                if(m_points[i] == point){
                    indexCorePoint = j;
                }
            }

            clusterSeeds.erase(clusterSeeds.begin()+indexCorePoint);

            for(int i = 0; i < clusterSeeds.size(); i++)
            {
                vector<int> clusterNeighors = calculateCluster(m_points[clusterSeeds[i]]);

                if (clusterNeighors.size() >= m_minPoints)
                {

                    for(int k = 0; k < clusterNeighors.size(); ++k){
                        if(cluster_ids[k] == NOISE){
                            cluster_ids[k] = clusterID;
                        }
                        if(cluster_ids[k] == UNCLASSIFIED){
                            clusterSeeds.push_back(k);
                            cluster_ids[k] = clusterID;
                        }
                    }
                }
            }

            return SUCCESS;
        }
    }

    inline double calculateDistance(point3 pointCore, point3 pointTarget )
    {
        vec3 dist = distance(pointCore, pointTarget);
        return dist.length();
    }

        
    
    void changeMaterial(){

        for(int i = 0; i < cluster_ids.size(); i++){
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
