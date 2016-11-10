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
