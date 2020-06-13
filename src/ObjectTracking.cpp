#include "include/ObjectTracking.hpp"
#include "include/ImageGrab.hpp"

#include <opencv2/videoio.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

ObjectTracking::ObjectTracking(cv::Ptr<cv::Tracker> tracker, int cameraNum)
{
    tracker_ = tracker;
    cameraNum_ = cameraNum;
}

void ObjectTracking::run()
{
    ImageGrab cap;
    cap.open(cameraNum_);

    cv::Mat frame;
    cv::Rect2d roi;

    std::cout << "[INFO] Start stream" << std::endl;
    
    //cv::Ptr<cv::Tracker> temp = cv::Ptr<cv::Tracker>(*tracker_);

    bool init = false;
    while(true)
    {
        cap >> frame;

        if(frame.empty())
        {
            break;
        }

        if(init)
        {
            bool messag;
            if(tracker_ -> update(frame, roi))
            {
                cv::rectangle(frame, roi, cv::Scalar( 255, 0, 0 ), 2, 1);
                messag = false;
            }
            else if(!messag)
            {
                std::cout << "[INFO] target cannot be detect" << std::endl;
                messag = true;
            }
        }

        cv::imshow("cammera", frame);

        char c = (char) cv::waitKey(1);
        if(c == 's')
        {
            cap >> frame;
            roi = cv::selectROI("cammera", frame);

            if(tracker_ -> init(frame, roi))
            {
                std::cout << "[-] tracker not be init" << std::endl;
            }
            init = true;
        }
        if((int) c == 27)
        {
            init=false;
            cap >> frame;
        }
        if(c == 'q')
        {
            break;
        }
    }
}
