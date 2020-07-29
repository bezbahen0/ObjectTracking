#include "include/tracker/Matcher.hpp"

void Matcher::initialize(const std::vector<cv::Point2f>& pts_fg_norm, const cv::Mat desc_fg, const std::vector<int>& classes_fg,
        const cv::Mat desc_bg, const cv::Point2f center)
{
    this -> pts_fg_norm = pts_fg_norm;
    this -> num_bg_points = desc_bg.rows;

    if (desc_bg.rows > 0 && desc_fg.rows > 0)
        vconcat(desc_bg, desc_fg, database);
    else if (desc_bg.rows > 0)
        database = desc_bg;
    else
        database = desc_fg;

    desc_length = database.cols*8;

    std::vector<int> classes_bg = std::vector<int>(desc_bg.rows,-1);

    classes = classes_bg;
    classes.insert(classes.end(), classes_fg.begin(), classes_fg.end());

    bfmatcher = cv::DescriptorMatcher::create("BruteForce-Hamming");
}

void Matcher::matchGlobal(const std::vector<cv::KeyPoint> & keypoints, const cv::Mat descriptors,
        std::vector<cv::Point2f> & points_matched, std::vector<int> & classes_matched)
{
    if(keypoints.size() == 0)
        return;

    std::vector<std::vector<cv::DMatch>> matches;
    bfmatcher -> knnMatch(descriptors, database, matches, 2);

    for(size_t i = 0; i < matches.size(); i++)
    {
        std::vector<cv::DMatch> m = matches[i];

        float distance1 = m[0].distance / desc_length;
        float distance2 = m[1].distance / desc_length;
        int matched_class = classes[m[0].trainIdx];

        if(matched_class == -1) continue;
        if(distance1 > thr_dist) continue;
        if(distance1/distance2 > thr_ratio) continue;

        points_matched.push_back(keypoints[i].pt);
        classes_matched.push_back(matched_class);
    }
}

void Matcher::matchLocal(const std::vector<cv::KeyPoint>& keypoints, const cv::Mat descriptors,
        const cv::Point2f center, const float scale, const float rotation,
        std::vector<cv::Point2f>& points_matched, std::vector<int>& classes_matched)
{
    if(keypoints.size() == 0) 
        return;

    std::vector<cv::Point2f> pts_fg_trans;
    pts_fg_trans.reserve(pts_fg_norm.size());
    for(size_t i = 0; i < pts_fg_norm.size(); ++i)
    {
        pts_fg_trans.push_back(scale * utils::rotate(pts_fg_norm[i], -rotation));
    }

    for(size_t i = 0; i < keypoints.size(); ++i)
    {
        cv::Point2f location_rel = keypoints[i].pt - center;

        std::vector<int> indices_potential;
        for(size_t j = 0; j < pts_fg_trans.size(); ++j)
        {
            float l2norm = cv::norm(pts_fg_trans[j] - location_rel);

            if(l2norm < thr_cutoff) 
            {
                indices_potential.push_back(num_bg_points + j);
            }

        }

        if(indices_potential.size() == 0) continue;

        cv::Mat database_potential = cv::Mat(indices_potential.size(), database.cols, database.type());
        for(size_t j = 0; j < indices_potential.size(); j++) {
            database.row(indices_potential[j]).copyTo(database_potential.row(j));
        }

        std::vector<std::vector<cv::DMatch> > matches;
        bfmatcher -> knnMatch(descriptors.row(i), database_potential, matches, 2);

        std::vector<cv::DMatch> m = matches[0];

        float distance1 = m[0].distance / desc_length;
        float distance2 = m.size() > 1 ? m[1].distance / desc_length : 1;

        if(distance1 > thr_dist) continue;
        if(distance1/distance2 > thr_ratio) continue;

        int matched_class = classes[indices_potential[m[0].trainIdx]];

        points_matched.push_back(keypoints[i].pt);
        classes_matched.push_back(matched_class);
    }
}


