#ifndef TRACKERCMT_HPP
#define TRACKERCMT_HPP

#include<opencv2/tracking.hpp>

class TrackerCMT : public cv::Tracker 
{
public:
    virtual ~TrackerCMT(); 
    TrackerCMT(): detectorName_("FAST"), descriptorName_("BRISK")
    {isInit = 0;}

    virtual void read(const cv::FileNode&) CV_OVERRIDE {}
    virtual void write(cv::FileStorage&) const CV_OVERRIDE {}

    static cv::Ptr<TrackerCMT> create()
    {
        cv::makePtr<TrackerCMT>();
    }
protected:
    virtual bool initImpl(const cv::Mat& image, const cv::Rect2d& boundingBox) CV_OVERRIDE;
    virtual bool updateImpl(const cv::Mat& image, const cv::Rect2d& boundingBox) CV_OVERRIDE;
private:
    std::string detectorName_;
    std::string descriptorName_;

    cv::Mat imagePrev_;

};

#endif /* TRACKERCMT_HPP */
