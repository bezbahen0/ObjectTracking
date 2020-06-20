#ifndef IMAGEGRAB_HPP
#define IMAGEGRAB_HPP

#include <opencv2/videoio.hpp>

enum TypesCapture
{
    CAP_CAMERA = 0,
    CAP_FILE = 1,
    CAP_DISPLAY = 2
};

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

//    bool open();
protected:

};

#endif /* IMAGEGRAB_HPP */
