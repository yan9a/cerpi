#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    raspicam::RaspiCam_Cv cap;
    if (!cap.open()) { printf("RaspiCam is not opened. \n");return -1;}
    else { printf("RaspiCam is opened. \n");}
    Mat frame;
    for (int i=0;;i++) {
        cap.grab();
        cap.retrieve(frame);
        imshow("Frame", frame);
        if (waitKey(40) == 27) break; //if 'Esc' key is pressed
    }
    cap.release();
    return 0;
}
