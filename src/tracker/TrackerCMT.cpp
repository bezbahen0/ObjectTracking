#include "include/tracker/TrackerCMT.hpp"

#include <opencv2/imgproc/imgproc.hpp>

bool TrackerCMT::initImpl(const cv::Mat& image, const cv::Rect2d& boundingBox)
{
    cv::Mat img;
    if(img.channels() == 1)
        img = image;
    else
        cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);
    
    initSize_ = boundingBox.size();
    imagePrev_ = img.clone();
    cv::Point2f center = cv::Point2f(boundingBox.x + boundingBox.weight/2.0, boundingBox.y + boundingBox.height/2.0);
    bb_ = cv::RotatedRect(center, initSize_, 0.0); 

    detector_ = cv::FeatureDetector::create(detectorName_);
    descriptor_ = cv::DescriptorExtractor::create(descriptorName_);

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

    consensus_.Initialize(pointsNormalized);

    for(size_t i = 0; i < keypoints_fg.size(); ++i)
    {
        pointsActive_.push_back(keypoints_fg[i].pt);
        activeClasses_ = classes_fg;
    }
}

bool TrackerCMT::updateImpl(const cv::Mat& image, const, cv::Rect2d& boundingBox)
{

}

