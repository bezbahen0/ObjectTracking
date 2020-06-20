#ifndef DISPLAYCAPTURE_HPP
#define DISPLAYCAPTURE_HPP

#include <opencv2/videoio.hpp>

class DisplayCapture : public cv::IVideoCapture
{
public:

    //virtual double getProperty(int propId) const CV_OVERRIDE;//{ return 0; }
    //virtual bool setProperty(int propId, double value) CV_OVERRIDE;//{ return false; }
    virtual bool grabFrame() CV_OVERRIDE;
    virtual bool retrieveFrame(int, cv::OutputArray dst) CV_OVERRIDE;
    virtual bool isOpened() const CV_OVERRIDE;
    //virtual int getCaptureDomain() 
private:

};

#endif /* DISPLAYCAPTURE_HPP */
