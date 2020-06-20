#ifndef DISPLAYCAPTURE_HPP
#define DISPLAYCAPTURE_HPP

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <opencv2/videoio.hpp>

#include <vector>

class DisplayCapture : public cv::IVideoCapture
{
public:
    
    DisplayCapture();
    virtual ~DisplayCapture();
    
    virtual bool grabFrame() CV_OVERRIDE;
    virtual bool retrieveFrame(int, cv::OutputArray dst) CV_OVERRIDE;
    virtual bool isOpened() const CV_OVERRIDE;

private:
    Display* display_;
    Window root_;
    int screenNumber_;

    int width_; 
    int height_;
    int BitsPerPixel_;
    std::vector<uint8_t> pixels_;
};

#endif /* DISPLAYCAPTURE_HPP */
