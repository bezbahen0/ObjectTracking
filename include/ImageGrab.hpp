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
    cv::Ptr<cv::IVideoCapture> createDisplayCap(int index);
};

#endif /* IMAGEGRAB_HPP */
