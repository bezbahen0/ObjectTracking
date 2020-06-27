#ifndef OBJECTTRACKING_HPP
#define OBJECTTRACKING_HPP

#include <opencv2/tracking/tracker.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>

#include "include/ImageGrab.hpp"

enum TypeCapture
{
    VIDEO_CAP = 1,
    CAMERA_CAP = 2,
    DISPLAY_CAP = 3
};

class ObjectTracking 
{
public:
    ObjectTracking(cv::String& trackerName, int index, int typeCap);
    ObjectTracking(cv::String& trackerName, int index, char* addressDisplay);
    ObjectTracking(cv::String& trackerName, cv::String filename);
    
    void run();
private:
    cv::String trackerName_;
    cv::Ptr<cv::Tracker> tracker_;
    cv::Mat mask_;
    ImageGrab cap_;
};


#endif /*OBJECTTRACKING_HPP*/
