#ifndef DISPLAYCAPTURE_HPP
#define DISPLAYCAPTURE_HPP

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <opencv2/videoio.hpp>

#include <vector>

namespace cv
{
class IVideoCapture
{
public:
    virtual ~IVideoCapture() {}
    virtual double getProperty(int) const { return 0; }
    virtual bool setProperty(int, double) { return false; }
    virtual bool grabFrame() = 0;
    virtual bool retrieveFrame(int, OutputArray) = 0;
    virtual bool isOpened() const = 0;
    virtual int getCaptureDomain() { return CAP_ANY; } // Return the type of the capture object: CAP_DSHOW, etc...
};
}

class DisplayCapture : public cv::IVideoCapture
{
public:
    
    DisplayCapture(int windowNumber);
    DisplayCapture(int windowNumber, char* address);
    virtual ~DisplayCapture();
    
    virtual bool grabFrame() CV_OVERRIDE;
    virtual bool retrieveFrame(int, cv::OutputArray dst) CV_OVERRIDE;
    virtual bool isOpened() const CV_OVERRIDE;

private:
    void getWindowList();

    Display* display_;
    Window root_;
    Window window_;
    int windowNumber_;
    Window* listWindows_;
    unsigned long lenlist_;

    int width_; 
    int height_;
    int BitsPerPixel_;
    std::vector<uint8_t> pixels_;

};

#endif /* DISPLAYCAPTURE_HPP */
