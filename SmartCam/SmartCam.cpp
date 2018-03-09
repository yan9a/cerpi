//File: SmartCam.cpp
//Description: Smart surveillance camera using body detection and PIR sensor
//WebSite: http://cool-emerald.blogspot.sg
//MIT License (https://opensource.org/licenses/MIT)
//Copyright (c) 2018 Yan Naing Aye


#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <time.h>
#include "ce_io.h"

#define CE_RASPICAM -1
#define CE_WEBCAM 0

//Choose the camera type
#define CE_CAMERA_TYPE CE_RASPICAM

#if CE_CAMERA_TYPE==CE_RASPICAM
    #include <raspicam/raspicam_cv.h>
#endif // CE_CAMERA_TYPE

using namespace std;
using namespace cv;

#define CE_DAILY 3
#define CE_HOURLY 2
#define CE_MINUTELY 1
#define CE_MAKEVIDEO CE_MINUTELY
#define CE_PERIOD_NORMAL 100
#define CE_PERIOD_FAST 500

const string wpath="/home/pi/SmartCam/";
string body_cascade_name = wpath+"haarcascade_fullbody.xml";
//string body_cascade_name = wpath+"haarcascade_frontalface_alt.xml";
CascadeClassifier body_cascade;

int detectAndDisplay(Mat& frame)
{
	std::vector<Rect> bodies;
	Mat frame_gray;
    int r=0;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	body_cascade.detectMultiScale(frame_gray, bodies, 1.1, 2, 0 |
        CV_HAAR_SCALE_IMAGE, Size(50, 100));// Detect bodies

	for (size_t i = 0; i < bodies.size(); i++)
	{
		Point p1(bodies[i].x, bodies[i].y);
        Point p2(bodies[i].x + bodies[i].width, bodies[i].y + bodies[i].height);
        rectangle(frame, p1, p2, Scalar(0, 255, 0), 4, 8, 0);
        r=1;
	}
	return r;
}

string GetTimeStr()
{
    time_t t = time(0);//now
    struct tm * ts=localtime(&t);
    string str="";
    string tstr="";
    tstr=to_string(ts->tm_year+1900);
    str+=string(4-tstr.length(),'0')+tstr+"-";
    tstr=to_string(ts->tm_mon+1);
    str+=string(2-tstr.length(),'0')+tstr+"-";
    tstr=to_string(ts->tm_mday);
    str+=string(2-tstr.length(),'0')+tstr;
    #if (CE_MAKEVIDEO < CE_DAILY)
        str+=" ";
        tstr=to_string(ts->tm_hour);
        str+=string(2-tstr.length(),'0')+tstr;
        #if (CE_MAKEVIDEO < CE_HOURLY)
            str+=":";
            tstr=to_string(ts->tm_min);
            //str+=string(2-tstr.length(),'0')+tstr+":";
            //tstr=to_string(ts->tm_sec);
            str+=string(2-tstr.length(),'0')+tstr;
        #endif // MAKEVIDEO
    #endif // MAKEVIDEO
    return str;
}

int main(int argc, char** argv)
{
    printf("SmartCam ... \n");
    printf("Select the image frame and press ESC to exit.\n");

    //init IO
    CE_IO pir_sensor(23,INPUT);

	//Load the cascades
	if (!body_cascade.load(body_cascade_name)) { printf("Error in loading haarcascade.\n"); return -1; };

#if CE_CAMERA_TYPE==CE_RASPICAM
    raspicam::RaspiCam_Cv cap;
    if (!cap.open())
#else
    VideoCapture cap(0);//Default camera
	if (!cap.isOpened())
#endif // CE_CAMERA_TYPE
    { printf("Video is not opened. \n"); return -1; }
	else
	{ printf("Video is opened. \n"); }


	Size S = Size((int)cap.get(CV_CAP_PROP_FRAME_WIDTH), (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
	printf("Frame size: %d x %d \n", S.width, S.height);

    //If you want to change the camera resolution
	//S = Size(640,480);
    //cap.set(CV_CAP_PROP_FRAME_WIDTH, S.width);
    //cap.set(CV_CAP_PROP_FRAME_HEIGHT, S.height);
    //cap.release();
    //cap.open();//need to reopen with the new size

	double rate = cap.get(CV_CAP_PROP_FPS);//Frame rate
	printf("Frame rate: %f \n", rate);

    //int ex = CV_FOURCC('X', '2', '6', '4');//small file size
    int ex = CV_FOURCC('M', 'J', 'P', 'G');
    rate = 10;//changed frame rate to 10 fps
    int dperiod = 100;//period to wait
    string fname=GetTimeStr()+".avi";
    string previousfname=fname;
    VideoWriter *outputVideo;
    outputVideo=new VideoWriter(wpath+fname, ex , rate, S, true);
    if(!outputVideo->isOpened())
        {
            cout << "Could not open video writer"<< endl;
            waitKey(5000);
            return -1;
        }
	Mat frame;
	int dFlag=0;
	int pFlag=0;

	for (int i = 0;; i++) {
  #if CE_CAMERA_TYPE==CE_RASPICAM
        cap.grab();
        cap.retrieve(frame);
  #else
        if (!cap.read(frame)) break;
  #endif // CE_CAMERA_TYPE
		dFlag=detectAndDisplay(frame);
		pFlag=pir_sensor.Read();

		if(pFlag){
            putText(frame,"PIR DETECTED", Point(40,40), FONT_HERSHEY_PLAIN,2.0, CV_RGB(255, 0, 0),2.0);
		}

		fname=GetTimeStr()+".avi";
		if(previousfname!=fname){
            previousfname=fname;
            outputVideo->release();
            outputVideo=new VideoWriter(wpath+fname, ex , rate, S, true);
            if(!outputVideo->isOpened())
            {
                cout << "Could not open video writer."<< endl;
                waitKey(5000);
                return -1;
            }
		}
		else{
            *outputVideo << frame;
		}

        imshow("Frame", frame);
        if(dFlag || pFlag){
            dperiod=CE_PERIOD_NORMAL;
        }
        else
        {
            dperiod=CE_PERIOD_FAST;
        }
		if (waitKey(dperiod) == 27) break; //if 'Esc' key is pressed
	}
	cap.release();
    outputVideo->release();
	//waitKey(5000);
	return 0;
}

