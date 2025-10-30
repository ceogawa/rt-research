#include <memory>
#include <vector>
#include <algorithm>

// #include "dbscan3.h"

#include <cstddef>
#include "dbscan3_nanoflann.h"

#include <type_traits>

#include <cassert>
#include <cstddef>
#include <span>
#include <vector>
#include <cstdlib>

// DBSCAN FROM GITHUB

struct point2d
{
    float x, y;
};

struct point3d
{
    float x, y, z;
};



inline auto get_pt(const point2d& p, std::size_t dim)
{
    if(dim == 0) return p.x;
    return p.y;
}

inline auto get_pt(const point3d& p, std::size_t dim)
{
    if(dim == 0) return p.x;
    if(dim == 1) return p.y;
    return p.z;
}

template<typename Point>
struct adaptor
{
    std::shared_ptr<const std::vector<Point>> points;

    adaptor(std::shared_ptr<const std::vector<Point>> points) : points(points) { }

    inline std::size_t kdtree_get_point_count() const { return points->size(); }

    inline float kdtree_get_pt(std::size_t idx, std::size_t dim) const
    {
        return get_pt((*points)[idx], dim);
    }

    template <class BBOX>
    bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }

    const float* elem_ptr(std::size_t idx) const
    {
        const auto& pt = (*points)[idx];
        static float coords[3]; // Assuming max 3 dimensions
        coords[0] = get_pt(pt, 0);
        coords[1] = get_pt(pt, 1);
        coords[2] = get_pt(pt, 2);
        return coords;
    }
};

auto sort_clusters(std::vector<std::vector<size_t>>& clusters)
{
    for(auto& cluster: clusters)
    {
        std::sort(cluster.begin(), cluster.end());
    }
}

template<int n_cols, typename Adaptor>
auto dbscan(const Adaptor& adapt, int min_pts)
{
    float eps;
    using namespace nanoflann;
    using  my_kd_tree_t = KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<float, decltype(adapt)>, decltype(adapt), n_cols>;

    auto index = my_kd_tree_t(n_cols, adapt, KDTreeSingleIndexAdaptorParams(10));
    index.buildIndex();

    const auto n_points = adapt.kdtree_get_point_count();
    auto visited  = std::vector<bool>(n_points);
    auto clusters = std::vector<std::vector<size_t>>();
    auto matches  = std::vector<std::pair<size_t, float>>();
    auto sub_matches = std::vector<std::pair<size_t, float>>();

    // new code for optimal epsilon selection using k nearest neighbor
    std::vector<float> knns;

    int k = (min_pts < 4) ? 4 : min_pts;

    std::vector<float> k_distances(n_points);

        
    for(size_t i = 0; i < n_points; i++){
          // Prepare the query point
        std::vector<float> query_pt(n_cols);
        const float* pt = adapt.elem_ptr(i);
        std::copy(pt, pt + n_cols, query_pt.begin());

        // Prepare arrays for indices and distances
        std::vector<size_t> indices(k + 1);
        std::vector<float> distances(k + 1);
            // Create the result set object
        nanoflann::KNNResultSet<float, size_t> resultSet(k + 1);
        resultSet.init(indices.data(), distances.data());

        // matches.clear();
        index.knnSearch(adapt.elem_ptr(i), k + 1, indices.data(), distances.data(), 10);

        k_distances[i] = distances[k];
    }

     // Find an optimal epsilon value
    std::sort(k_distances.begin(), k_distances.end());
    float percentile = 0.4;
    size_t idx = static_cast<size_t>(percentile * k_distances.size());
    eps = std::sqrt(k_distances[idx]);
    eps = (eps > .9) ? 0.9 : eps;

    std::cout << "Optimal epsilon: " << eps << std::endl;

    for(size_t i = 0; i < n_points; i++)
    {
        if (visited[i]) continue;

        index.radiusSearch(adapt.elem_ptr(i), eps, matches, SearchParams(32, 0.f, false));
        if (matches.size() < static_cast<size_t>(min_pts)) continue;
        visited[i] = true;

        auto cluster = std::vector<size_t>({i});

        while (!matches.empty())
        {
            auto nb_idx = matches.back().first;
            matches.pop_back();
            if (visited[nb_idx]) continue;
            visited[nb_idx] = true;

            index.radiusSearch(adapt.elem_ptr(nb_idx), eps, sub_matches, SearchParams(32, 0.f, false));

            if (sub_matches.size() >= static_cast<size_t>(min_pts))
            {
                std::copy(sub_matches.begin(), sub_matches.end(), std::back_inserter(matches));
            }
            cluster.push_back(nb_idx);
        }
        clusters.emplace_back(std::move(cluster));
    }
    sort_clusters(clusters);
    return clusters;
}

auto dbscan(const std::shared_ptr<const std::vector<point2d>>& data, int min_pts) -> std::vector<std::vector<size_t>>
{
    const auto adapt = adaptor<point2d>(data);
    return dbscan<2>(adapt, min_pts);
}

auto dbscan(const std::shared_ptr<const std::vector<point3d>>& data, int min_pts) -> std::vector<std::vector<size_t>>
{
    const auto adapt = adaptor<point3d>(data);
    return dbscan<3>(adapt, min_pts);
}