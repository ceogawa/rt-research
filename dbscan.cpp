// https://github.com/james-yoo/DBSCAN
// LICENSED BY james-yoo

// simplified and adapted by Claire Ogawa

#include "dbscan.h"
// #include "mesh.h"

int DBSCAN::run()
{
    int clusterID = 1;

    for(int i = 0; i < m->normals_origin.size(); i++){
        if (m->face_cluster_id[i] == UNCLASSIFIED){
            if (expandCluster(i)){
                m->face_cluster_id[i]++;
            }
        }
    }

    return 0;
}

int DBSCAN::expandCluster(int i)
{    
    point3 point = m->normals_origin[i];
    int clusterID = m->face_cluster_id[i];

    vector<int> clusterSeeds = calculateCluster(point);

    if (clusterSeeds.size() < m_minPoints)
    {
        m->face_cluster_id[i] = NOISE;
        return FAILURE;
    }
    else
    {
        int indexCorePoint = 0;

        for(int j = 0; j < clusterSeeds.size(); j++){
            m->face_cluster_id[j] = clusterID;
            if(m->normals_origin[i] == point){
                indexCorePoint = j;
            }
        }

        clusterSeeds.erase(clusterSeeds.begin()+indexCorePoint);

        for(int i = 0; i < clusterSeeds.size(); i++)
        {
            vector<int> clusterNeighors = calculateCluster(m->normals_origin[clusterSeeds[i]]);

            if (clusterNeighors.size() >= m_minPoints)
            {

                for(int k = 0; k < clusterNeighors.size(); ++k){
                    if(m->face_cluster_id[k] == NOISE){
                        m->face_cluster_id[k] = clusterID;
                    }

                    if(m->face_cluster_id[k] == UNCLASSIFIED){
                        clusterSeeds.push_back(k);
                        m->face_cluster_id[k] = clusterID;
                    }
                }
            }
        }

        return SUCCESS;
    }
}

vector<int> DBSCAN::calculateCluster(point3 point)
{
    vector<int> clusterIndex;

    for (int i = 0; i < m->normals_origin.size(); i++){
        if (calculateDistance(point, m->normals_origin[i]) <= m_epsilon){
            clusterIndex.push_back(i);
        }
    }

    return clusterIndex;
}

void DBSCAN::changeMaterial(){
    
}

inline double DBSCAN::calculateDistance(point3 pointCore, point3 pointTarget )
{
    vec3 dist = distance(pointCore, pointTarget);
    return dist.length();
}




