#include "include/ObjectTracking.hpp"

static void help(char* argv[])
{
    std::cout << "help me please" << std::endl; 
}

int main(int argc, char* argv[])
{
    cv::CommandLineParser parser(argc, argv,
        "{help h    |         | help                                }"
        "{video     |         | tracking objects on video           }"
        "{camera    |    0    | tracking objects on camera or webcam}"
        "{display   |         | tracking objects on display         }"
        "{tracker t |   kcf   | tracker type                        }"
    );
    
    cv::String tracker = parser.get<cv::String>("tracker"); 
    ObjectTracking cap(tracker);
    
    if(parser.has("help"))
    {
        help(argv);
        return EXIT_SUCCESS;
    }
    if(parser.has("video"))
    {
        cap.run(parser.get<cv::String>("video"));
        return EXIT_SUCCESS;
    }
    if(parser.has("camera"))
    {
        cap.run(parser.get<int>("camera"));
        return EXIT_SUCCESS;
    }
    if(parser.has("display"))
    {
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
