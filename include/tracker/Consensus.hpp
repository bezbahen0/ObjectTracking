#ifndef CONSENSUS_HPP
#define CONSENSUS_HPP

#include "common.h"


class Consensus
{
public:
    Consensus() : estimate_scale(true), estimate_rotation(false), thr_cutoff(20) {};

    void initialize(const std::vector<cv::Point2f>& points_normalized);
    void estimateScaleRotation(const std::vector<cv::Point2f>& points, const std::vector<int>& classes,
            float& scale, float& rotation);
    void findConsensus(const std::vector<cv::Point2f>& points, const std::vector<int>& classes,
            const float scale, const float rotation,
            cv::Point2f& center, std::vector<cv::Point2f>& points_inlier, std::vector<int>& classes_inlier);

    bool estimate_scale;
    bool estimate_rotation;

private:
    float thr_cutoff;
    vector<Point2f> points_normalized;
    Mat distances_pairwise;
    Mat angles_pairwise;
};

#endif /* CONSENSUS_HPP */
