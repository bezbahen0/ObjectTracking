#include "include/ObjectTracking.hpp"
#include "include/TrackerMyImpl.hpp"

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
    else if (name == "myimpl")
        tracker = TrackerMyImpl::create();
    else
        CV_Error(cv::Error::StsBadArg, "invalid tracking algorithm name\n");

    return tracker;
}

ObjectTracking::ObjectTracking(cv::String& trackerName, int index, int typeCap)
{
    trackerName_ = trackerName;
    tracker_ = ::cvtrackers(trackerName);
    if(typeCap == TypeCapture::DISPLAY_CAP)
    {
        cap_.opendisplay(index);
    }
    else
    {
        cap_.open(index);
    }
}

ObjectTracking::ObjectTracking(cv::String& trackerName, int index, char* addressDisplay)
{
    trackerName_ = trackerName;
    tracker_ = ::cvtrackers(trackerName);
    cap_.opendisplay(index, addressDisplay);
}

ObjectTracking::ObjectTracking(cv::String& trackerName, cv::String filename)
{
    trackerName_ = trackerName;
    tracker_ = ::cvtrackers(trackerName);
    cap_.open(filename);

}

void ObjectTracking::run()
{
    cv::Mat frame;
    cv::Rect2d roi;

    std::cout << "[INFO] Start stream" << std::endl;
    
    bool init = false;
    while(true)
    {
        cap_ >> frame;

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
            cap_ >> frame;
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

            cap_ >> frame;

            tracker_ = cvtrackers(trackerName_);
        }
        if(c == 'q')
        {
            break;
        }
    }
}
