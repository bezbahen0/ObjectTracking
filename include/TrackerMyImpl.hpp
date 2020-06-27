#ifndef TRACKERMYIMPL_HPP
#define TRACKERMYIMPL_HPP

 #include<opencv2/tracking.hpp>

class TrackerMyImpl : public cv::Tracker
{
public:
    virtual ~TrackeriMyImpl() {}

    virtual void read(const FileNode&) CV_OVERRIDE {}
    virtual void write(FileStorage&) const CV_OVERRIDE {}

    static cv::Ptr<TrackerMyImpl> create();

protected:
    virtual bool initImpl(const cv::Mat& image, const cv::Rect2d& boundingBox) CV_OVERRIDE;
    virtual bool updateImpl(const cv::Mat& image, cv::Rect2d& boundingBox) CV_OVERRIDE;

private:
    TrackerMyImpl() {isInit = 0;}
};

#endif /* TRACKERMYIMPL_HPP */
