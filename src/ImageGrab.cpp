#include "include/ImageGrab.hpp"

ImageGrab::ImageGrab()
{
}

ImageGrab::ImageGrab(const cv::String& filename) : cv::VideoCapture(filename)
{
}

ImageGrab::ImageGrab(const cv::String& filename, int apiPreference) : cv::VideoCapture(filename, apiPreference)
{
}

ImageGrab::ImageGrab(int index) : cv::VideoCapture(index)
{
}

ImageGrab::ImageGrab(int index, int apiPreference) : cv::VideoCapture(index, apiPreference)
{
}

ImageGrab::~ImageGrab()
{
    icap.release();
}

bool ImageGrab::opendisplay(int index)
{
    if(isOpened())
    {
        release();
    }
    
    icap = createDisplayCap();
    if(!icap.empty())
    {
        return true;
    }
    return false;
}

cv::Ptr<cv::IVideoCapture> ImageGrab::createDisplayCap()
{
    DisplayCapture* cap = new DisplayCapture();
    return cv::Ptr<cv::IVideoCapture>(cap);
}
