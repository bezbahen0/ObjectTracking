#include "include/tracker/Consensus.hpp"
#include "lib/fastcluster/fastcluster.h"

#include <cmath>

void Consensus::initialize(const std::vector<cv::Point2f>& points_normalized)
{
    this->points_normalized = points_normalized;

    size_t num_points = points_normalized.size();

    distances_pairwise = cv::Mat(num_points, num_points, CV_32FC1);
    angles_pairwise = cv::Mat(num_points, num_points, CV_32FC1);

    for(size_t i = 0; i < num_points; i++)
    {
        for(size_t j = 0; j < num_points; j++)
        {
            cv::Point2f v = points_normalized[i] - points_normalized[j];

            float distance = cv::norm(v);
            float angle = std::atan2(v.y,v.x);

            distances_pairwise.at<float>(i,j) = distance;
            angles_pairwise.at<float>(i,j) = angle;
        }
    }
}


void Consensus::estimateScaleRotation(const std::vector<cv::Point2f>& points, const std::vector<int>& classes,
        float& scale, float& rotation)
{

    std::vector<float> changes_scale;
    if(estimate_scale) changes_scale.reserve(points.size()*points.size());
    std::vector<float> changes_angles;
    if(estimate_rotation) changes_angles.reserve(points.size()*points.size());

    for(size_t i = 0; i < points.size(); i++)
    {
        for(size_t j = 0; j < points.size(); j++)
        {
            if(classes[i] != classes[j])
            {
                cv::Point2f v = points[i] - points[j];

                if(estimate_scale)
                {
                    float distance = cv::norm(v);
                    float distance_original = distances_pairwise.at<float>(classes[i],classes[j]);
                    float change_scale = distance / distance_original;
                    changes_scale.push_back(change_scale);
                }

                if(estimate_rotation)
                {
                    float angle = std::atan2(v.y,v.x);
                    float angle_original = angles_pairwise.at<float>(classes[i],classes[j]);
                    float change_angle = angle - angle_original;

                    if(std::fabs(change_angle) > M_PI) {
                        change_angle = utils::sgn(change_angle) * 2 * M_PI + change_angle;
                    }

                    changes_angles.push_back(change_angle);
                }
            }

        }

    }

    if(changes_scale.size() < 2) scale = 1;
    else scale = utils::median(changes_scale);

    if(changes_angles.size() < 2) rotation = 0;
    else rotation = utils::median(changes_angles);
}

void Consensus::findConsensus(const std::vector<cv::Point2f>& points, const std::vector<int>& classes,
        const float scale, const float rotation,
        cv::Point2f& center, std::vector<cv::Point2f>& points_inlier, std::vector<int>& classes_inlier)
{
    if(points.size() == 0)
    {
        center.x = std::numeric_limits<float>::quiet_NaN();
        center.y = std::numeric_limits<float>::quiet_NaN();

        return;
    }

    std::vector<cv::Point2f> votes(points.size());
    for(size_t i = 0; i < points.size(); i++)
    {
        votes[i] = points[i] - scale * utils::rotate(points_normalized[classes[i]], rotation);
    }

    t_index N = points.size();

    float * D = new float[N*(N-1)/2]; 
    cluster_result Z(N-1);

    int index = 0;
    for(size_t i = 0; i < points.size(); i++)
    {
        for(size_t j = i+1; j < points.size(); j++)
        {
            D[index] = norm(votes[i] - votes[j]);
            index++;
        }
    }

    MST_linkage_core(N,D,Z);

    union_find nodes(N);

    std::stable_sort(Z[0], Z[N-1]);

    int * S = new int[2*N-1];
    for(int i = 0; i < 2*N-1; i++)
    {
        S[i] = 1;
    }

    t_index parent = 0; 
    for(node const * NN=Z[0]; NN!=Z[N-1]; ++NN)
    {
        t_index node1 = nodes.Find(NN->node1);
        t_index node2 = nodes.Find(NN->node2);

        if(NN->dist < thr_cutoff)
        {
            parent = nodes.Union(node1, node2);
            S[parent] = S[node1] + S[node2];
        }
    }

    int * T = new int[N];
    for(t_index i = 0; i < N; i++)
    {
        T[i] = nodes.Find(i);
    }

    int S_max = std::distance(S, std::max_element(S, S + 2*N-1));

    points_inlier.reserve(S[S_max]);
    classes_inlier.reserve(S[S_max]);
    center.x = center.y = 0;

    for(size_t i = 0; i < points.size(); i++)
    {
        if(T[i] == S_max)
        {
            points_inlier.push_back(points[i]);
            classes_inlier.push_back(classes[i]);
            center.x += votes[i].x;
            center.y += votes[i].y;
        }

    }

    center.x /= points_inlier.size();
    center.y /= points_inlier.size();

    delete[] D;
	delete[] S;
	delete[] T;
}


