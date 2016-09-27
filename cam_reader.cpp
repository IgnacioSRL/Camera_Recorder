#include "cam_reader.h"

Cam_Reader::Cam_Reader(Camera_params params){
    this->initialized=false;
    initialization(params);
}

Cam_Reader::~Cam_Reader(){
    stop_running();
}

void Cam_Reader::stop_running(){
    mtx.lock();
    this->running=false;
    this->initialized=false;
    this->fps_performance=0;
    this->image_size=cv::Size();
    mtx.unlock();
}

bool Cam_Reader::initialization(Camera_params params){
    if(!params.address.empty() && params.address!="0.0.0.0" && !is_initialized()){
        std::stringstream ss;
        ss<<params.address;
        int num = 0;
        ss >> num;
        if(ss.good() || (num==0 && ss.str()!="0"))
            this->cap.open(params.address);
        else
            this->cap.open(num);
        if(this->cap.isOpened()){
            this->cap.grab();
            if(params.fps>0)
                this->miliseconds_cycle=1000./params.fps;
            else
                this->miliseconds_cycle=0;
            this->image_size=image_size;
            this->running=true;
            std::thread thrd(&Cam_Reader::frame_reader,this);
            thrd.detach();
            this->t.start();
        }
        this->initialized=this->cap.isOpened();
    }
    return this->initialized;
}

bool Cam_Reader::is_initialized(){
    bool init=this->initialized;
    return init;
}

void Cam_Reader::frame_reader(){
    while(this->running){
        if(is_initialized()){
            this->mtx.lock();
            this->cap.grab();
            this->mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    if(this->cap.isOpened())
        this->cap.release();
}

cv::Mat Cam_Reader::get_image(){
    cv::Mat image;
    if(is_initialized()){
        this->mtx.lock();
        this->cap.retrieve(image);
        this->mtx.unlock();
        if(this->image_size!=cv::Size() && !image.empty())
            cv::resize(image,image,this->image_size);
    }
    this->underflow=true;
    while(this->miliseconds_cycle-this->t.elapsed()>0)
        this->underflow=false;
    this->fps_performance=1000./this->t.elapsed();
    this->t.restart();
    return image;
}

bool Cam_Reader::get_image(cv::Mat &image){
    if(is_initialized()){
        this->mtx.lock();
        this->cap.retrieve(image);
        this->mtx.unlock();
        if(this->image_size!=cv::Size() && !image.empty())
            cv::resize(image,image,this->image_size);
    }
    this->underflow=true;
    while(this->miliseconds_cycle-this->t.elapsed()>0)
        this->underflow=false;
    this->fps_performance=1000./this->t.elapsed();
    this->t.restart();
    if(image.empty())
        return false;
    return true;
}

bool Cam_Reader::get_frame_rate(float &fps){
    fps=this->fps_performance;
    bool ret=this->underflow && (this->miliseconds_cycle>0);
    return ret;
}
