/*
*
* Copyright 2016 Ignacio San Roman Lana
*
* This file is part of Camera_Recorder
*
* Camera_Recorder is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* Camera_Recorder is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Camera_Recorder. If not, see http://www.gnu.org/licenses/.
*
* For those usages not covered by this license please contact with
* isanromanlana@gmail.com
*/

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
