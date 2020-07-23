#include "include/tracker/TrackerMOSSEmod.hpp"

const double eps = 0.00001;
const double rate = 0.2;
const double psrthreshold = 5.7;
const double k = 1.3;

struct DummyModel : cv::TrackerModel
{
    virtual void modelUpdateImpl() CV_OVERRIDE {}
    virtual void modelEstimationImpl(const std::vector<cv::Mat>&) CV_OVERRIDE {}
};

bool TrackerMOSSEmod::initImpl(const cv::Mat& image, const cv::Rect2d& boundingBox)
{
    model = cv::makePtr<DummyModel>();

    cv::Rect2d bboxsmal = getSmalBigRect(boundingBox, 0);
    cv::Rect2d bboxbig = getSmalBigRect(boundingBox, 1);

    filters.resize(3);
    initFilter(image, bboxsmal, filters[0]);
    initFilter(image, boundingBox, filters[1]);
    initFilter(image, bboxbig, filters[2]);

    return true;
}

bool TrackerMOSSEmod::updateImpl( const cv::Mat& image, cv::Rect2d& boundingBox )
{
    cv::Rect2d bboxsmal = getSmalBigRect(boundingBox, 0);
    cv::Rect2d bboxbig = getSmalBigRect(boundingBox, 1);

    initFilter(image, bboxsmal, filters[0]);
    initFilter(image, bboxbig, filters[2]);

    updateFilter(image, bboxsmal, filters[0]);
    updateFilter(image, boundingBox, filters[1]);
    updateFilter(image, bboxbig, filters[2]);

    getFilterWithMaxPsr();

    if(mainFilter.psr < psrthreshold)
        return false;

    double x = mainFilter.center.x;
    double y = mainFilter.center.y;
    int w = mainFilter.size.width;
    int h = mainFilter.size.height;
    boundingBox = cv::Rect2d(cv::Point2d(x - 0.5 * w, y - 0.5 * h), cv::Point2d(x + 0.5 * w, y + 0.5 * h));

    filters[1] = mainFilter;
    return true;
}

void TrackerMOSSEmod::initFilter(const cv::Mat& image, const cv::Rect2d& bbox, Filter& filter)
{
    cv::Mat img;
    if (image.channels() == 1)
        img = image;
    else
        cv::cvtColor(image, img, cv::COLOR_BGR2GRAY);

    int w = cv::getOptimalDFTSize(int(bbox.width));
    int h = cv::getOptimalDFTSize(int(bbox.height));

    int x1 = int(std::floor((2 * bbox.x + bbox.width - w) / 2));
    int y1 = int(std::floor((2 * bbox.y + bbox.height - h) / 2));
    filter.center.x = x1 + w/2;
    filter.center.y = y1 + h/2;
    filter.size.width = w;
    filter.size.height = h;
    
    cv::Mat window;
    filter.hanWin = cv::Mat();
    cv::getRectSubPix(img, filter.size, filter.center, window);
    cv::createHanningWindow(filter.hanWin, filter.size, CV_32F);
    
    cv::Mat g = cv::Mat::zeros(filter.size, CV_32F);
    g.at<float>(h/2, w/2) = 1;
    cv::GaussianBlur(g, g, cv::Size(-1,-1), 2.0);

    double maxval;
    cv::minMaxLoc(g, 0, &maxval);
    g = g / maxval;
    cv::dft(g, filter.G, cv::DFT_COMPLEX_OUTPUT);

    filter.A = cv::Mat::zeros(filter.G.size(), filter.G.type());
    filter.B = cv::Mat::zeros(filter.G.size(), filter.G.type());

    for(int i = 0; i < 8; ++i)
    {
        cv::Mat temp = randWarp(window);
        preProcess(temp, filter.hanWin);

        cv::Mat WINDOW_WARP, Ai, Bi;
        cv::dft(temp, WINDOW_WARP, cv::DFT_COMPLEX_OUTPUT);
        cv::mulSpectrums(filter.G, WINDOW_WARP, Ai, 0, true);
        cv::mulSpectrums(WINDOW_WARP, WINDOW_WARP, Bi, 0, true);
        filter.A += Ai;
        filter.B += Bi;
    }
    filter.H = divDFTs(filter.A, filter.B);
}

void TrackerMOSSEmod::updateFilter(const cv::Mat& image, const cv::Rect2d& bbox, Filter& filter)
{
    cv::Mat imagesub;
    cv::getRectSubPix(image, filter.size, filter.center, imagesub);
    if (imagesub.channels() != 1)
        cv::cvtColor(imagesub, imagesub, cv::COLOR_BGR2GRAY);
    preProcess(imagesub, filter.hanWin);

    cv::Point delta;
    filter.psr = correlate(imagesub, filter.H, delta);

    filter.center.x += delta.x;
    filter.center.y += delta.y;

    cv::Mat img_sub_new;
    cv::getRectSubPix(image, filter.size, filter.center, img_sub_new);
    if (img_sub_new.channels() != 1)
        cvtColor(img_sub_new, img_sub_new, cv::COLOR_BGR2GRAY);
    preProcess(img_sub_new, filter.hanWin);

    cv::Mat F, A_new, B_new;
    cv::dft(img_sub_new, F, cv::DFT_COMPLEX_OUTPUT);
    cv::mulSpectrums(filter.G, F, A_new, 0, true );
    cv::mulSpectrums(F, F, B_new, 0, true );
    
    if(rate >= 1.0)
    {
        filter.A = A_new;
        filter.B = B_new;
    }
    else 
    {
        filter.A = filter.A * (1 - ::rate) + A_new * ::rate;
        filter.B = filter.B * (1 - ::rate) + B_new * ::rate;
    }
    filter.H = divDFTs(filter.A, filter.B);
}

cv::Rect2d TrackerMOSSEmod::getSmalBigRect(const cv::Rect2d& rect, const int param)
{
    int w = int(rect.width);
    int h = int(rect.height);
    int x = int(std::floor((2 * rect.x + rect.width - w) / 2));
    int y = int(std::floor((2 * rect.y + rect.height - h) / 2));
    cv::Point center(x + w/2, y + h/2);

    if(param == 0)
        return cv::Rect2d(cv::Point2d(center.x - (w/k * 0.5), center.y - (h/k * 0.5)), cv::Point2d(center.x + (w/k*0.5), center.y + (h/k * 0.5)));
    else
        return cv::Rect2d(cv::Point2d(center.x - (w/k), center.y - h/k), cv::Point2d(center.x + w/k, center.y + h/k));
        
}
//this need change
void TrackerMOSSEmod::getFilterWithMaxPsr()
{
    int index = 0;
    double psr = 0.0;
    for(size_t i = 0; i != filters.size(); ++i)
    {
        if(filters[i].psr > psr)
        {
            psr = filters[i].psr;
            index = i;
        }
    }

    mainFilter = filters[index];
}

cv::Mat TrackerMOSSEmod::divDFTs(const cv::Mat& src1, const cv::Mat& src2)
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

void TrackerMOSSEmod::preProcess(cv::Mat& window, cv::Mat& hanningWin)
{
    window.convertTo(window, CV_32F);
    cv::log(window + 1.0f, window);

    cv::Scalar mean;
    cv::Scalar stddev;
    cv::meanStdDev(window, mean, stddev);
    window = (window - mean[0]) / (stddev[0] + ::eps);

    window = window.mul(hanningWin);
}

double TrackerMOSSEmod::correlate(const cv::Mat& image, cv::Mat& H, cv::Point& delta)
{
    cv::Mat imagesub;
    cv::Mat RESPONSE;
    cv::Mat response;

    cv::dft(image, imagesub, cv::DFT_COMPLEX_OUTPUT);
    cv::mulSpectrums(imagesub, H, RESPONSE, 0, true);
    cv::idft(RESPONSE, response, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);

    double maxval;
    cv::Point maxloc;
    cv::minMaxLoc(response, 0, &maxval, 0, &maxloc);
    delta.x = maxloc.x - int(response.size().width/2);
    delta.y = maxloc.y - int(response.size().height/2);

    cv::Scalar mean, std;
    cv::meanStdDev(response, mean, std);
    return (maxval - mean[0]) / (std[0] + ::eps);
}

cv::Mat TrackerMOSSEmod::randWarp(const cv::Mat& a) 
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
