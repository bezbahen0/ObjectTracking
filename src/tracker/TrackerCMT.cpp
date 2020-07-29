#include "include/tracker/TrackerCMT.hpp"

#include <opencv2/imgproc/imgproc.hpp>
struct DummyModel : cv::TrackerModel
{
    virtual void modelUpdateImpl() CV_OVERRIDE {}
    virtual void modelEstimationImpl(const std::vector<cv::Mat>&) CV_OVERRIDE {}
};

bool TrackerCMT::initImpl(const cv::Mat& image, const cv::Rect2d& boundingBox)
{
    model = cv::makePtr<DummyModel>();

    cv::Mat img;
    if(img.channels() == 1)
        img = image;
    else
        cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);
    
    initSize_ = boundingBox.size();
    imagePrev_ = img.clone();
    cv::Point2f center = cv::Point2f(boundingBox.x + boundingBox.width/2.0, boundingBox.y + boundingBox.height/2.0);
    bb_ = cv::RotatedRect(center, initSize_, 0.0);

#if CV_MAJOR_VERSION > 2
    detector_ = cv::FastFeatureDetector::create();
    descriptor_ = cv::BRISK::create();
#else
    detector_ = cv::FeatureDetector::create(detectorName_);
    descriptor_ = cv::DescriptorExtractor::create(descriptorName_);
#endif

    std::vector<cv::KeyPoint> keypoints;
    detector_ -> detect(img, keypoints);

    std::vector<cv::KeyPoint> keypoints_fg;
    std::vector<cv::KeyPoint> keypoints_bg;
    for(size_t i = 0; i < keypoints.size(); ++i)
    {
        cv::KeyPoint k = keypoints[i];
        cv::Point2f pt = k.pt;

        if(pt.x > boundingBox.x && pt.y > boundingBox.y && pt.x < boundingBox.br().x && pt.y < boundingBox.br().y)
        {
            keypoints_fg.push_back(k);
        }
        else
        {
            keypoints_bg.push_back(k);
        }
    }

    std::vector<int> classes_fg;
    classes_fg.reserve(keypoints_fg.size());
    for(size_t i = 0; i < keypoints_fg.size(); ++i)
    {
        classes_fg.push_back(i);
    }

    cv::Mat descs_fg;
    cv::Mat descs_bg;
    descriptor_ -> compute(img, keypoints_fg, descs_fg);
    descriptor_ -> compute(img, keypoints_bg, descs_bg);

    std::vector<cv::Point2f> points_fg;
    std::vector<cv::Point2f> points_bg;
    
    for(size_t i = 0; i < keypoints_fg.size(); ++i)
    {
        points_fg.push_back(keypoints_fg[i].pt);
    }
    for(size_t i = 0; i < keypoints_bg.size(); ++i)
    {
        points_bg.push_back(keypoints_bg[i].pt);
    }

    std::vector<cv::Point2f> pointsNormalized;
    for(size_t i = 0; i < points_fg.size(); ++i)
    {
        pointsNormalized.push_back(points_fg[i] - center);
    }
    
    matcher_.initialize(pointsNormalized, descs_fg, classes_fg, descs_bg, center);

    consensus_.initialize(pointsNormalized);

    for(size_t i = 0; i < keypoints_fg.size(); ++i)
    {
        pointsActive_.push_back(keypoints_fg[i].pt);
        activeClasses_ = classes_fg;
    }

    return true;
}

bool TrackerCMT::updateImpl(const cv::Mat& image, cv::Rect2d& boundingBox)
{
    cv::Mat img;
    if(img.channels() == 1)
        img = image;
    else
        cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);
    

    std::vector<cv::Point2f> points_tracked;
    std::vector<unsigned char> status;
    tracker_.track(imagePrev_, img, pointsActive_, points_tracked, status);

    std::vector<int> classes_tracked;
    for (size_t i = 0; i < activeClasses_.size(); i++)
    {
        if (status[i])
        {
            classes_tracked.push_back(activeClasses_[i]);
        }

    }

    std::vector<cv::KeyPoint> keypoints;
    detector_ -> detect(img, keypoints);

    cv::Mat descriptors;
    descriptor_ -> compute(img, keypoints, descriptors);

    std::vector<cv::Point2f> points_matched_global;
    std::vector<int> classes_matched_global;
    matcher_.matchGlobal(keypoints, descriptors, points_matched_global, classes_matched_global);

    std::vector<cv::Point2f> points_fused;
    std::vector<int> classes_fused;
    utils::preferFirst(points_tracked, classes_tracked, points_matched_global, classes_matched_global, points_fused, classes_fused);

    float scale;
    float rotation;
    consensus_.estimateScaleRotation(points_fused, classes_fused, scale, rotation);

    cv::Point2f center;
    std::vector<cv::Point2f> points_inlier;
    std::vector<int> classes_inlier;
    consensus_.findConsensus(points_fused, classes_fused, scale, rotation, center, points_inlier, classes_inlier);

    std::vector<cv::Point2f> points_matched_local;
    std::vector<int> classes_matched_local;
    matcher_.matchLocal(keypoints, descriptors, center, scale, rotation, points_matched_local, classes_matched_local);

    pointsActive_.clear();
    activeClasses_.clear();

    utils::preferFirst(points_matched_local, classes_matched_local, points_inlier, classes_inlier, pointsActive_, activeClasses_);

    bb_ = cv::RotatedRect(center, initSize_ * scale, rotation/CV_PI * 180);
    boundingBox = bb_.boundingRect();

    for(size_t i = 0; i < this->pointsActive_.size(); i++)
    {
        cv::circle(image, this -> pointsActive_[i], 2, cv::Scalar(255,0,0));
    }

    imagePrev_ = img.clone();

    return true;
}

