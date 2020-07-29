#include "include/tracker/common.hpp"

namespace utils
{

float median(std::vector<float>& A)
{
    if (A.size() == 0)
    {
        return std::numeric_limits<float>::quiet_NaN();
    }

    std::nth_element(A.begin(), A.begin() + A.size()/2, A.end());

    return A[A.size()/2];
}

cv::Point2f rotate(const cv::Point2f v, const float angle)
{
    cv::Point2f r;
    r.x = std::cos(angle) * v.x - std::sin(angle) * v.y;
    r.y = std::sin(angle) * v.x + std::cos(angle) * v.y;

    return r;
}

void preferFirst(const std::vector<cv::Point2f> & points_first, const std::vector<int> & classes_first,
    const std::vector<cv::Point2f> & points_second, const std::vector<int> & classes_second,
    std::vector<cv::Point2f> & points_fused, std::vector<int> & classes_fused)
{
    points_fused = points_first;
    classes_fused = classes_first;

    for (size_t i = 0; i < points_second.size(); i++)
    {
        int class_second = classes_second[i];

        bool found = false;
        for (size_t j = 0; j < points_first.size(); j++)
        {
            int class_first = classes_first[j];
            if (class_first == class_second) found = true;
        }

        if (!found)
        {
            points_fused.push_back(points_second[i]);
            classes_fused.push_back(class_second);
        }
    }
}

};
