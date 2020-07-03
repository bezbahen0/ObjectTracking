#include "include/TrackerMyImpl.hpp"

bool TrackerMyImpl::initImpl(const cv::Mat& image, const cv::Rect2d& boundingBox)
{

}

bool TrackerMyImpl::updateImpl(const cv::Mat& image, cv::Rect2d& boundingBox)
{

}

cv::Ptr<TrackerMyImpl> TrackerMyImpl::create()
{
    return cv::makePtr<TrackerMyImpl>();
}
