#include "include/DisplayCapture.hpp"

#include <stdexcept>

#include <opencv2/imgproc.hpp>

DisplayCapture::DisplayCapture(int windowNumber)
{
    windowNumber_ = windowNumber;
    display_ = XOpenDisplay(nullptr);
    root_ = RootWindow(display_, DefaultScreen(display_)); 

    getWindowList();
    
    if(windowNumber > lenlist_)
    {
        throw std::runtime_error("wrong index window");
    }

    window_ = listWindows_[windowNumber]; 
}

DisplayCapture::DisplayCapture(int windowNumber, char* address)
{
    windowNumber_ = windowNumber_;
    display_ = XOpenDisplay(address);
    root_ = RootWindow(display_, DefaultScreen(display_));

    getWindowList();

    if(windowNumber > lenlist_)
    {
        throw std::runtime_error("wrong index window");
    }

    window_ = listWindows_[windowNumber];
}

DisplayCapture::~DisplayCapture()
{
    XCloseDisplay(display_); 
    delete[](listWindows_);
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
        XWindowAttributes attributes;
        XGetWindowAttributes(display_, window_, &attributes);

        width_ = attributes.width;
        height_ = attributes.height;

        XImage* img = XGetImage(display_, window_, 0, 0 , width_, height_, AllPlanes, ZPixmap);
        BitsPerPixel_ = img -> bits_per_pixel;
        pixels_.resize(width_ * height_ * 4);

        memcpy(&pixels_[0], img -> data, pixels_.size());

        cv::Mat mat = cv::Mat(height_, width_, BitsPerPixel_ > 24 ? CV_8UC4 : CV_8UC3, &pixels_[0]);
        cv::cvtColor(mat, mat, cv::COLOR_BGRA2BGR);
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

void DisplayCapture::getWindowList()
{
    Atom prop = XInternAtom(display_, "_NET_CLIENT_LIST", false);
    Atom type;
    int form;
    unsigned long remain;
    unsigned char* list;

    XGetWindowProperty(display_, XDefaultRootWindow(display_), prop, 0, 1024, false, 33, &type, &form, &lenlist_, &remain, &list);

    if(list == nullptr)
    {
        throw std::runtime_error("window not found");
    }

    listWindows_ = (Window*) list;
}
