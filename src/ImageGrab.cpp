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
}

/*bool ImageGrab::open()
{
icap set;
}*/
