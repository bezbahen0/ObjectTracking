#ifndef TRACKER_HPP
#define TRACKER_HPP

#include <opencv2/video/tracking.hpp>

class _Tracker
{
public:
    _Tracker() : thr_fb(30) {};
    void track(const cv::Mat imagePrev, const cv::Mat img, const std::vector<cv::Point2f>& pointsPrev,
            std::vector<cv::Point2f>& pointsTracked, std::vector<unsigned char>& status);

private:
    float thr_fb;
};

#endif /* TRACKER_HPP */
