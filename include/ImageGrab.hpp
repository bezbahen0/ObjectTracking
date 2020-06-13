#ifndef IMAGEGRAB_HPP
#define IMAGEGRAB_HPP

#include <opencv2/videoio.hpp>

class ImageGrab : public cv::VideoCapture
{
public:
    ImageGrab();
    ImageGrab(const cv::String& filename);
    ImageGrab(const cv::String& filename, int apiPreference);
    ImageGrab(int index);
    ImageGrab(int index, int apiPreference);

    /*constructors for grab image display*/
    virtual ~ImageGrab();

    bool open();
private:

};

#endif /* IMAGEGRAB_HPP */
