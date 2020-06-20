#include "include/DisplayCapture.hpp"


DisplayCapture::DisplayCapture()
{
    screenNumber_ = 0;
    display_ = XOpenDisplay(nullptr);
    root_ = RootWindow(display_, screenNumber_);
}

DisplayCapture::~DisplayCapture()
{
   XCloseDisplay(display_); 
   pixels_.shrink_to_fit();
}

bool DisplayCapture::grabFrame()
{
    if(display_ == NULL)
    {
        return false;
    }
    return true;
}

bool DisplayCapture::retrieveFrame(int, cv::OutputArray dst)
{
    if(display_ != NULL)
    {
        XWindowAttributes attributes = {0};
        XGetWindowAttributes(display_, root_, &attributes);

        width_ = attributes.width;
        height_ = attributes.height;

        XImage* img = XGetImage(display_, root_, 0, 0 , width_, height_, AllPlanes, ZPixmap);
        BitsPerPixel_ = img -> bits_per_pixel;
        pixels_.resize(width_ * height_ * 4);

        memcpy(&pixels_[0], img -> data, pixels_.size());

        cv::Mat mat = cv::Mat(height_, width_, BitsPerPixel_ > 24 ? CV_8UC4 : CV_8UC3, &pixels_[0]);
        mat.copyTo(dst);

        XDestroyImage(img);

        return true;
    }
    return false;
}

bool DisplayCapture::isOpened() const
{
    return display_ != NULL ? true : false;
}

