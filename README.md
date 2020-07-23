# Objects Tracking Using OpenCV

## Working Example

## Project Description 

## Requirements
To compile this code you need to install opencv together with opencv_contrib.
Also install libx11-dev
```
sudo apt-get install libopencv-dev libx11-dev
```

## Installation
```
git clone https://github.com/bezbahen0/ObjectTracking
cd ObjectTracking
mkdir build 
cd build
cmake ..
make 
./ObjectTracking
```

## Execution
To run this application execute this
```
./ObjectTracking
```
Run with camera capture
```
./ObjectTracking -c=<camera index>
```
Run with video capture
```
./ObjectTracking -v=<path to videofile>
```
Run with capture image on window application
```
./ObjectTracking -w=<index window>
```
To find out the index of the window for capturing the image use the application in the apps directory

Default, application using the kcf tracker algorithm if you want to change algoritm use --tracker key. For example:
```
./ObjectTracking -c=<index camera> --tracker=<tracker name>
./ObjectTracking -c=0 --tracker=csrt
```
## Links
https://habr.com/ru/post/421285
https://github.com/gnebehay/CppMT
https://github.com/opencv/opencv_contrib/tree/master/modules/tracking

