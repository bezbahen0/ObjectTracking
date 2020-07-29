#ifndef COMMON_HPP
#define COMMON_HPP

#include <vector>
#include <limits>

#include <opencv2/core/core.hpp>
namespace utils
{

float median(std::vector<float>& A);

cv::Point2f rotate(const cv::Point2f v, const float angle);

void preferFirst(const std::vector<cv::Point2f> & points_first, const std::vector<int> & classes_first,
    const std::vector<cv::Point2f> & points_second, const std::vector<int> & classes_second,
    std::vector<cv::Point2f> & points_fused, std::vector<int> & classes_fused);
    
template<typename T>
int sgn(T x)
{
    if(x >= 0)
        return 1;
    else
        return -1;
}

};

#endif /* COMMON_HPP */
