#ifndef OBJECTTRACKING_HPP
#define OBJECTTRACKING_HPP

#include <opencv2/tracking/tracker.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>

#include "include/ImageGrab.hpp"

inline cv::Ptr<cv::Tracker> cvtrackers(cv::String name)
{
    cv::Ptr<cv::Tracker> tracker;

    if (name == "kcf")
        tracker = cv::TrackerKCF::create();
    else if (name == "tld")
        tracker = cv::TrackerTLD::create();
    else if (name == "boosting")
        tracker = cv::TrackerBoosting::create();
    else if (name == "medianflow")
        tracker = cv::TrackerMedianFlow::create();
    else if (name == "mil")
        tracker = cv::TrackerMIL::create();
    else if (name == "mosse")
        tracker = cv::TrackerMOSSE::create();
    else if (name == "csrt")
        tracker = cv::TrackerCSRT::create();
    else
        CV_Error(cv::Error::StsBadArg, "invalid tracking algorithm name\n");

    return tracker;
}

class ObjectTracking 
{
public:
    ObjectTracking(cv::String& trackerName)
    {
        trackerName_ = trackerName;
        tracker_ = ::cvtrackers(trackerName);
    }
    
    template<typename T>
    void run(T index)
    {
        ImageGrab cap;
        cap.open(index);

        cv::Mat frame;
        cv::Rect2d roi;

        std::cout << "[INFO] Start stream" << std::endl;
        
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

                if(!tracker_ -> init(frame, roi))
                {
                    std::cout << "[-] tracker not be init" << std::endl;
                    init = false;
                }
                else
                {
                    init = true;
                }

            }
            if((int) c == 27)
            {
                init = false;

                cap >> frame;

                tracker_ = cvtrackers(trackerName_);
            }
            if(c == 'q')
            {
                break;
            }
        }
    }
private:
    cv::String trackerName_;
    cv::Ptr<cv::Tracker> tracker_;
};


#endif /*OBJECTTRACKING_HPP*/
