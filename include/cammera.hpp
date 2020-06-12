#ifndef CAMMERA_HPP
#define CAMMERA_HPP

#include <opencv2/tracking/tracker.hpp>

#include <iostream>

class CaptureImageCamera
{
public:
    CaptureImageCamera(cv::Ptr<cv::Tracker> tracker, int cameraNum = 0);

    void run();

private:
    int cameraNum_;
    cv::Ptr<cv::Tracker> tracker_;
};

#endif /*CAMMERA_HPP*/
