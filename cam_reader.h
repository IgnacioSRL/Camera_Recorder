#ifndef CAM_READER_H
#define CAM_READER_H

#include "opencv2/opencv.hpp"
#include <thread>
#include <mutex>
#include <QTime>

using namespace std;

//Parameters structure for camera
struct Camera_params{
    string address;
    int fps;
    cv::Size image_size;
    Camera_params(){
        address="";
        fps=25;
        image_size=cv::Size();
    }
};

class Cam_Reader
{
public:
    Cam_Reader(Camera_params params=Camera_params());
    ~Cam_Reader();
    bool initialization(Camera_params params=Camera_params());
    bool is_initialized();
    void stop_running();
    bool get_frame_rate(float &fps);
    cv::Mat get_image();
    bool get_image(cv::Mat &image);


private:
    void frame_reader();
    //Variables
    cv::VideoCapture cap;
    QTime t;
    int miliseconds_cycle;
    cv::Size image_size;

    //Performance
    float fps_performance;
    bool underflow;

    //Thread control
    bool running;
    std::mutex mtx;

    //Initialized
    bool initialized;
};

#endif // CAM_READER_H
