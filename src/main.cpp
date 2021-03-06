#include "include/ObjectTracking.hpp"

cv::String keys =
        "{ help h    || help }"
        "{ video   v || tracking objects on video }"
        "{ camera  c || tracking objects on camera or webcam }"
        "{ window  w || tracking objects on display window}"
        "{ tracker t |kcf| tracker type }";

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv, keys);
    
    cv::String tracker = parser.get<cv::String>("tracker"); 
    
    if(parser.has("help") || argc == 1)
    {
        parser.printMessage();
        return EXIT_SUCCESS;
    }
    if(parser.has("video"))
    {
        ObjectTracking cap(tracker, parser.get<cv::String>("video"));
        cap.run();
        return EXIT_SUCCESS;
    }
    if(parser.has("window"))
    {
        ObjectTracking cap(tracker, parser.get<int>("window"), TypeCapture::DISPLAY_CAP);
        cap.run();
        return EXIT_SUCCESS;
    }
    if(parser.has("camera"))
    {
        ObjectTracking cap(tracker, parser.get<int>("camera"), TypeCapture::CAMERA_CAP);
        cap.run();
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
