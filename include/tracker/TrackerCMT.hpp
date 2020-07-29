#ifndef TRACKERCMT_HPP
#define TRACKERCMT_HPP

#include <opencv2/tracking.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "include/tracker/common.hpp"
#include "include/tracker/Tracker.hpp"
#include "include/tracker/Consensus.hpp"
#include "include/tracker/Matcher.hpp"

class TrackerCMT : public cv::Tracker 
{
public:
    virtual ~TrackerCMT() {}; 
    TrackerCMT(): detectorName_("FAST"), descriptorName_("BRISK")
    {isInit = 0;}

    virtual void read(const cv::FileNode&) CV_OVERRIDE {}
    virtual void write(cv::FileStorage&) const CV_OVERRIDE {}

    static cv::Ptr<TrackerCMT> create()
    {
        return cv::makePtr<TrackerCMT>();
    }
    cv::Rect bb_;
protected:
    virtual bool initImpl(const cv::Mat& image, const cv::Rect2d& boundingBox) CV_OVERRIDE;
    virtual bool updateImpl(const cv::Mat& image, cv::Rect2d& boundingBox) CV_OVERRIDE;

private:
    Matcher matcher_;
    _Tracker tracker_;
    Consensus consensus_;

    std::string detectorName_;
    std::string descriptorName_;

    cv::Ptr<cv::FeatureDetector> detector_;
    cv::Ptr<cv::DescriptorExtractor> descriptor_;

    cv::Size2f  initSize_;
    std::vector<int> activeClasses_;
    std::vector<cv::Point2f> pointsActive_; 

    float theta_;

    cv::Mat imagePrev_;
};

#endif /* TRACKERCMT_HPP */
