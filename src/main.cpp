#include <opencv2/opencv.hpp>

#include "include/ObjectTracking.hpp"

inline cv::Ptr<cv::Tracker> cvTrackers(cv::String name)
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
        CV_Error(cv::Error::StsBadArg, "Invalid tracking algorithm name\n");

    return tracker;
}

static void help(char* argv[])
{
    std::cout << "help me please" << std::endl; 
}



int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv,
        "{help h    |         |}"
        "{video     |test.mp4 |}"
        "{camera    |    0    |}"
        "{display   |    0    |}"
        "{tracker t |   kcf   |}"
    );
    
    cv::Ptr<cv::Tracker> tracker = cvTrackers(parser.get<cv::String>("tracker"));
    
    if(parser.has("help"))
    {
        help(argv);
        return EXIT_SUCCESS;
    }
    if(parser.has("video"))
    {

    }
    if(parser.has("camera"))
    {
        ObjectTracking cap(tracker, parser.get<int>("camera"));
        cap.run();
    }
    if(parser.has("display"))
    {

    }

    return EXIT_SUCCESS;
}
