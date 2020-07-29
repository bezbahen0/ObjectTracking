#include "include/tracker/Tracker.hpp"

void _Tracker::track(const cv::Mat imagePrev, const cv::Mat img, const std::vector<cv::Point2f>& pointsPrev,
        std::vector<cv::Point2f>& pointsTracked, std::vector<unsigned char>& status)
{
    if (pointsPrev.size() > 0)
    {
        std::vector<float> err; 

        cv::calcOpticalFlowPyrLK(imagePrev, img, pointsPrev, pointsTracked, status, err);

        std::vector<cv::Point2f> points_back;
        std::vector<unsigned char> status_back;
        std::vector<float> err_back;

        cv::calcOpticalFlowPyrLK(img, imagePrev, pointsTracked, points_back, status_back, err_back);

        for (int i = pointsPrev.size() - 1; i >= 0; i--)
        {
            float l2norm = cv::norm(points_back[i] - pointsPrev[i]);

            bool fb_err_is_large = l2norm > thr_fb;

            if (fb_err_is_large || !status[i] || !status_back[i])
            {
                pointsTracked.erase(pointsTracked.begin() + i);

                status[i] = 0;
            }
        }

    }
}

