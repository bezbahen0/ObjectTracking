#ifndef IMAGEGRAB_HPP
#define IMAGEGRAB_HPP

#include <opencv2/videoio.hpp>

#include "include/DisplayCapture.hpp"

class ImageGrab : public cv::VideoCapture
{
public:
    ImageGrab();
    ImageGrab(const cv::String& filename);
    ImageGrab(const cv::String& filename, int apiPreference);
    ImageGrab(int index);
    ImageGrab(int index, int apiPreference);

    virtual ~ImageGrab();

    bool opendisplay(int index);
    bool opendisplay(int index, char* address);
    cv::Ptr<cv::IVideoCapture> createDisplayCap(int index);
    cv::Ptr<cv::IVideoCapture> createDisplayCap(int index, char* address);
};

#endif /* IMAGEGRAB_HPP */
