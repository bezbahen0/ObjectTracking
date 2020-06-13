#ifndef OBJECTTRACKING_HPP
#define OBJECTTRACKING_HPP

#include <opencv2/tracking/tracker.hpp>

class ObjectTracking 
{
public:
    ObjectTracking(cv::Ptr<cv::Tracker> tracker, int cameraNum = 0);

    void run();

private:
    int cameraNum_;
    cv::Ptr<cv::Tracker> tracker_;
};

#endif /*OBJECTTRACKING_HPP*/
