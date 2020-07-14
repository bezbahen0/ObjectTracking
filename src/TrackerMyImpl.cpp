#include "include/TrackerMyImpl.hpp"

const double eps = 0.00001;
const double rate = 0.2;
const double psrthreshold = 5.7;

struct DummyModel : cv::TrackerModel
{
    virtual void modelUpdateImpl() CV_OVERRIDE {}
    virtual void modelEstimationImpl(const std::vector<cv::Mat>&) CV_OVERRIDE {}
};

bool TrackerMyImpl::initImpl(const cv::Mat& image, const cv::Rect2d& boundingBox)
{
    model = cv::makePtr<DummyModel>();

    cv::Mat img;
    if (image.channels() == 1)
        img = image;
    else
        cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);

    int w = cv::getOptimalDFTSize(int(boundingBox.width));
    int h = cv::getOptimalDFTSize(int(boundingBox.height));

    int x1 = int(std::floor((2 * boundingBox.x + boundingBox.width - w) / 2));
    int y1 = int(std::floor((2 * boundingBox.y + boundingBox.height - h) / 2));
    center_.x = x1 + w/2;
    center_.y = y1 + h/2;
    size_.width = w;
    size_.height = h;

    cv::Mat window;
    cv::getRectSubPix(img, size_, center_, window);
    cv::createHanningWindow(hanningWin_, size_, CV_32F);

    cv::Mat g = cv::Mat::zeros(size_,CV_32F);
    g.at<float>(h/2, w/2) = 1;
    cv::GaussianBlur(g, g, cv::Size(-1,-1), 2.0);

    double maxval;
    cv::minMaxLoc(g, 0, &maxval);
    g = g / maxval;
    cv::dft(g, G_, cv::DFT_COMPLEX_OUTPUT);

    A_ = cv::Mat::zeros(G_.size(), G_.type());
    B_ = cv::Mat::zeros(G_.size(), G_.type());

    for(int i = 0; i < 8; ++i)
    {
        cv::Mat temp = randWarp(window);
        preProcess(temp);

        cv::Mat WINDOW_WARP, Ai, Bi;
        cv::dft(temp, WINDOW_WARP, cv::DFT_COMPLEX_OUTPUT);
        cv::mulSpectrums(G_, WINDOW_WARP, Ai, 0, true);
        cv::mulSpectrums(WINDOW_WARP, WINDOW_WARP, Bi, 0, true);
        A_ += Ai;
        B_ += Bi;
    }

    H_ = divDFTs(A_, B_);
    return true;
}

bool TrackerMyImpl::updateImpl( const cv::Mat& image, cv::Rect2d& boundingBox )
{
    if (H_.empty()) 
        return false;

    cv::Mat imagesub;
    cv::getRectSubPix(image, size_, center_, imagesub);

    if (imagesub.channels() != 1)
        cv::cvtColor(imagesub, imagesub, cv::COLOR_BGR2GRAY);
    preProcess(imagesub);

    cv::Point delta;
    double psr = correlate(imagesub, delta);
    if (psr < psrthreshold)
        return false;

    center_.x += delta.x;
    center_.y += delta.y;

    cv::Mat img_sub_new;
    cv::getRectSubPix(image, size_, center_, img_sub_new);
    if (img_sub_new.channels() != 1)
        cvtColor(img_sub_new, img_sub_new, cv::COLOR_BGR2GRAY);
    preProcess(img_sub_new);

    cv::Mat F, A_new, B_new;
    cv::dft(img_sub_new, F, cv::DFT_COMPLEX_OUTPUT);
    cv::mulSpectrums(G_, F, A_new, 0, true );
    cv::mulSpectrums(F, F, B_new, 0, true );

    A_ = A_ * (1 - rate) + A_new * rate;
    B_ = B_ * (1 - rate) + B_new * rate;
    H_ = divDFTs(A_, B_);

    double x = center_.x;
    double y = center_.y;
    int w = size_.width;
    int h = size_.height;
    boundingBox = cv::Rect2d(cv::Point2d(x - 0.5 * w, y - 0.5 * h), cv::Point2d(x + 0.5 * w, y + 0.5 * h));
    return true;
}

cv::Mat TrackerMyImpl::divDFTs(const cv::Mat& src1, const cv::Mat& src2)
{
    cv::Mat c1[2], c2[2], a1, a2, s1, s2, denom, re, im;
    
    cv::split(src1, c1);
    cv::split(src2, c2);

    cv::multiply(c2[0], c2[0], s1);
    cv::multiply(c2[1], c2[1], s2);
    cv::add(s1, s2, denom);

    cv::multiply(c1[0], c2[0], a1);
    cv::multiply(c1[1], c2[1], a2);
    cv::divide(a1 + a2, denom, re, 1.0);

    cv::multiply(c1[1], c2[0], a1);
    cv::multiply(c1[0], c2[1], a2);
    cv::divide(a1 + a2, denom, im, -1.0);


    cv::Mat dst;
    cv::Mat chn[] = {re, im};
    cv::merge(chn, 2, dst);
    return dst;
}

void TrackerMyImpl::preProcess(cv::Mat& window)
{
    window.convertTo(window, CV_32F);
    cv::log(window + 1.0f, window);

    cv::Scalar mean;
    cv::Scalar stddev;
    cv::meanStdDev(window, mean, stddev);
    window = (window - mean[0]) / (stddev[0] + eps);

    window = window.mul(hanningWin_);
}

double TrackerMyImpl::correlate(const cv::Mat& image, cv::Point& delta)
{
    cv::Mat imagesub;
    cv::Mat RESPONSE;
    cv::Mat response;

    cv::dft(image, imagesub, cv::DFT_COMPLEX_OUTPUT);
    cv::mulSpectrums(imagesub, H_, RESPONSE, 0, true);
    cv::idft(RESPONSE, response, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);

    double maxval;
    cv::Point maxloc;
    cv::minMaxLoc(response, 0, &maxval, 0, &maxloc);
    delta.x = maxloc.x - int(response.size().width/2);
    delta.y = maxloc.y - int(response.size().height/2);

    cv::Scalar mean, std;
    cv::meanStdDev(response, mean, std);
    return (maxval - mean[0]) / (std[0] + eps);
}

cv::Mat TrackerMyImpl::randWarp(const cv::Mat& a) 
{
    cv::RNG rng(8031965);

    const double C = 0.1;
    double ang = rng.uniform(-C, C);
    double c = std::cos(ang);
    double s = std::sin(ang);

    cv::Mat_<float> W(2, 3);
    W << c + rng.uniform(-C, C), -s + rng.uniform(-C, C), 0,
         s + rng.uniform(-C, C), c + rng.uniform(-C, C), 0; 

    cv::Mat_<float> centerwarp(2, 1);
    centerwarp << a.cols/2, a.rows/2;
    W.col(2) = centerwarp - W.colRange(0, 2) * centerwarp;

    cv::Mat warped;
    cv::warpAffine(a, warped, W, a.size(), cv::BORDER_REFLECT);
    return warped;

}
