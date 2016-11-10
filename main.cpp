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
* along with Orion Context Broker. If not, see http://www.gnu.org/licenses/.
*
* For those usages not covered by this license please contact with
* isanromanlana@gmail.com
*/

#include <iostream>
#include <sstream>
#include "cam_reader.h"

int main(int argc, char* argv[])
{
    //Check number of arguments
    if(argc==8){

        //Start timer
        QTime t;
        t.start();

        //Cam_Reader class
        Camera_params params;
        params.address=argv[1];
        params.fps=std::stof(argv[2]);
        params.image_size=cv::Size(atoi(argv[3]),atoi(argv[3]));
        Cam_Reader input(params);

        cv::Mat image;
        input.get_image(image);
        input.stop_running();
        input.initialization(params);
        input.get_image(image);

        //If initialized
        if(input.is_initialized()){

            //Repeat n times
            for(int i=0; i<atoi(argv[6]); i++){

                //Get output address
                std::stringstream dir;
                dir<<argv[7];
                dir<<i+1;
                dir<<".avi";

                //Start writer
                cv::VideoWriter writer(dir.str(), CV_FOURCC('M','J','P','G') ,std::stof(argv[2]),
                        cv::Size(input.get_image().cols,input.get_image().rows));

                //Restart timer
                t.restart();

                //While not finished this video and not ESC
                while(t.elapsed()/1000.<atof(argv[5]) && writer.isOpened()){

                    //Get image, write and show
                    cv::Mat image=input.get_image();
                    writer.write(image);
                    std::stringstream text;
                    text<<"Number video="<<i+1<<"         Time="<<round(t.elapsed()/1000.)<<"s";
                    cv::putText(image,text.str(),cv::Point(10,10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0,0,255));
                }
                writer.release();
            }
        }
        else
            std::cout<<"Could not connect"<<std::endl;
    }
    else{
        std::cout<<"ARGUMENTS"<<std::endl
                <<"1.Input address"<<std::endl
                <<"2.Fps"<<std::endl
                <<"3.Image Width (default=0)"<<std::endl
                <<"4.Image Height (default=0)"<<std::endl
                <<"5.Video lenght (seconds)"<<std::endl
                <<"6.Repetition"<<std::endl
                <<"7.Output address"<<std::endl;
    }
    return 0;
}

