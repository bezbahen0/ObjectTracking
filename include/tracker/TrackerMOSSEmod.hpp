#ifndef TRACKERMOSSEMOD_HPP
#define TRACKERMOSSEMOD_HPP

#include<opencv2/tracking.hpp>

struct Filter
{
    double psr;
    cv::Mat H;
    cv::Mat G;
    cv::Mat A;
    cv::Mat B;
    cv::Mat hanWin;
    cv::Point center;
    cv::Size size;
};

class TrackerMOSSEmod : public cv::Tracker
{
public:
    virtual ~TrackerMOSSEmod() {}
    TrackerMOSSEmod() {isInit = 0;}

    virtual void read(const cv::FileNode&) CV_OVERRIDE {}
    virtual void write(cv::FileStorage&) const CV_OVERRIDE {}

    static cv::Ptr<TrackerMOSSEmod> create()
    {
        return cv::makePtr<TrackerMOSSEmod>();
    }

protected:
    virtual bool initImpl(const cv::Mat& image, const cv::Rect2d& boundingBox) CV_OVERRIDE;
    virtual bool updateImpl(const cv::Mat& image, cv::Rect2d& boundingBox) CV_OVERRIDE;

private:
    void initFilter(const cv::Mat& image, const cv::Rect2d& bbox, Filter& filter);
    void updateFilter(const cv::Mat& image, const cv::Rect2d& bbox, Filter& filter);

    cv::Rect2d getSmalBigRect(const cv::Rect2d& rect, const int param);
    void getFilterWithMaxPsr();

    cv::Mat divDFTs(const cv::Mat& src1, const cv::Mat& src2);
    void preProcess(cv::Mat& window, cv::Mat& hanningWin);
    double correlate(const cv::Mat& image, cv::Mat& H, cv::Point& delta);
    cv::Mat randWarp(const cv::Mat& a);

    std::vector<Filter> filters;
    Filter mainFilter;
};

#endif /* TRAMOSSEMOD_HPP */
