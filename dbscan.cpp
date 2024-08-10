// https://github.com/james-yoo/DBSCAN
// LICENSED BY james-yoo

// modified by Claire Ogawa to accomodate this repos point class

#include "dbscan.h"
#include "mesh.h"

int DBSCAN::run()
{
    int clusterID = 1;

    for(int i = 0; i < m->normals_origin.size(); i++){
        if (m->face_cluster_id[i] == UNCLASSIFIED){
            if (expandCluster(m->normals_origin[i], m->face_cluster_id[i], i) != FAILURE){
                m->face_cluster_id[i]++;
            }
        }
    }

    // vector<point3>::iterator iter;
    // for(iter = m_points.begin(); iter != m_points.end(); ++iter)
    // {
    //     if ( iter->clusterID == UNCLASSIFIED )
    //     {
    //         if ( expandCluster(*iter, clusterID) != FAILURE )
    //         {
    //             clusterID += 1;
    //         }
    //     }
    // }

    return 0;
}

int DBSCAN::expandCluster(point3 point, int clusterID, int i)
{    
    vector<int> clusterSeeds = calculateCluster(point);

    if ( clusterSeeds.size() < m_minPoints )
    {
        m->face_cluster_id[i] = NOISE;
        // point.clusterID = NOISE;
        return FAILURE;
    }
    else
    {
        int index = 0, indexCorePoint = 0;

        for(int j = 0; j < clusterSeeds.size(); j++){
            m->face_cluster_id[j] = clusterID;
            if (m->normals_origin[i][0] == point[0] && m->normals_origin[i][1] == point[1] && m->normals_origin[i][2] == point[2]){
                indexCorePoint = index;
            }
        }

        // vector<int>::iterator iterSeeds;
        // for( iterSeeds = clusterSeeds.begin(); iterSeeds != clusterSeeds.end(); ++iterSeeds)
        // {
        //     m_points.at(*iterSeeds).clusterID = clusterID;
        //     if (m_points.at(*iterSeeds).x == point.x && m_points.at(*iterSeeds).y == point.y && m_points.at(*iterSeeds).z == point.z )
        //     {
        //         indexCorePoint = index;
        //     }
        //     ++index;
        // }

        clusterSeeds.erase(clusterSeeds.begin()+indexCorePoint);

        for( vector<int>::size_type i = 0, n = clusterSeeds.size(); i < n; ++i )
        {
            vector<int> clusterNeighors = calculateCluster(m_points.at(clusterSeeds[i]));

            if ( clusterNeighors.size() >= m_minPoints )
            {
                vector<int>::iterator iterNeighors;
                for ( iterNeighors = clusterNeighors.begin(); iterNeighors != clusterNeighors.end(); ++iterNeighors )
                {
                    if ( m_points.at(*iterNeighors).clusterID == UNCLASSIFIED || m_points.at(*iterNeighors).clusterID == NOISE )
                    {
                        if ( m_points.at(*iterNeighors).clusterID == UNCLASSIFIED )
                        {
                            clusterSeeds.push_back(*iterNeighors);
                            n = clusterSeeds.size();
                        }
                        m_points.at(*iterNeighors).clusterID = clusterID;
                    }
                }
            }
        }

        return SUCCESS;
    }
}

vector<int> DBSCAN::calculateCluster(point3 point)
{
    int index = 0;
    vector<point3>::iterator iter;
    vector<int> clusterIndex;

    for (int i = 0; i < m->normals_origin.size(); i++){
        if (calculateDistance(&point, &(m->normals_origin[i])) <= m_epsilon){
            clusterIndex.push_back(index);
        }
        index++;
    }

    return clusterIndex;
    // for( iter = m_points.begin(); iter != m_points.end(); ++iter)
    // {
    //     if ( calculateDistance(point, *iter) <= m_epsilon )
    //     {
    //         clusterIndex.push_back(index);
    //     }
    //     index++;
    // }
    // return clusterIndex;
}

inline double DBSCAN::calculateDistance(point3* pointCore, point3* pointTarget )
{
    return distance(pointCore, pointTarget);
   // return pow(pointCore[0] - pointTarget[0],2)+pow(pointCore[1] - pointTarget[1],2)+pow(pointCore[2] - pointTarget[2],2);
}


