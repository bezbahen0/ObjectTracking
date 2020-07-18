#include "include/ObjectTracking.hpp"
#include "include/TrackerMOSSEmod.hpp"

inline cv::Ptr<cv::Tracker> cvtrackers(cv::String& name)
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
    //this tracker use caffemodel, for mor information on gouturn tracker https://www.learnopencv.com/goturn-deep-learning-based-object-tracking/
    else if (name == "goturn")
        tracker = cv::TrackerGOTURN::create();
    else if (name == "mossemod")
        tracker = TrackerMOSSEmod::create();
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

    cap_.set(cv::CAP_PROP_FRAME_WIDTH, 960);
    cap_.set(cv::CAP_PROP_FRAME_HEIGHT, 540);
    cap_.set(cv::CAP_PROP_POS_FRAMES, 0);
    cap_.set(cv::CAP_PROP_POS_MSEC, 0);
}

ObjectTracking::ObjectTracking(cv::String& trackerName, cv::String filename)
{
    trackerName_ = trackerName;
    tracker_ = ::cvtrackers(trackerName);

    cap_.open(filename);

    cap_.set(cv::CAP_PROP_FRAME_WIDTH, 960);
    cap_.set(cv::CAP_PROP_FRAME_HEIGHT, 540);
    cap_.set(cv::CAP_PROP_POS_FRAMES, 0);
    cap_.set(cv::CAP_PROP_POS_MSEC, 0);
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
            break;

        if(init)
        {
            bool messag;
            if(tracker_ -> update(frame, roi))
            {
                cv::rectangle(frame, roi, cv::Scalar(0, 255, 0), 2, 1);
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
            break;
    }
}
