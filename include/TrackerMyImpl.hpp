#ifndef TRACKERMYIMPL_HPP
#define TRACKERMYIMPL_HPP

 #include<opencv2/tracking.hpp>

class TrackerMyImpl : public cv::Tracker
{
public:
    virtual ~TrackerMyImpl() {}
    TrackerMyImpl() {isInit = 0;}

    virtual void read(const cv::FileNode&) CV_OVERRIDE {}
    virtual void write(cv::FileStorage&) const CV_OVERRIDE {}

    static cv::Ptr<TrackerMyImpl> create()
    {
        return cv::makePtr<TrackerMyImpl>();
    }

protected:
    virtual bool initImpl(const cv::Mat& image, const cv::Rect2d& boundingBox) CV_OVERRIDE;
    virtual bool updateImpl(const cv::Mat& image, cv::Rect2d& boundingBox) CV_OVERRIDE;

private:
    cv::Mat divDFTs(const cv::Mat& src1, const cv::Mat& src2);
    void preProcess(cv::Mat& window);
    double correlate(const cv::Mat& image, cv::Point& delta);
    cv::Mat randWarp(const cv::Mat& a);

    cv::Mat G_;
    cv::Mat H_;
    cv::Mat A_;
    cv::Mat B_;
    cv::Mat hanningWin_;
    cv::Point2d center_;
    cv::Size size_;
};

#endif /* TRACKERMYIMPL_HPP */
