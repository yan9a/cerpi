#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    VideoCapture cap(0);//Default camera
    //VideoCapture cap("./sample.mp4");//open video file

    if (!cap.isOpened()) {
        printf("Video is not opened. \n");
        return -1;
    }
    else {
        printf("Video is opened. \n");
    }

    union { int v; char c[5]; } uEx;
    uEx.v = static_cast<int>(cap.get(CAP_PROP_FOURCC));
    uEx.c[4] = '\0';
    printf("Codec: %s \n",uEx.c);

    Size S = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),(int)cap.get(CAP_PROP_FRAME_HEIGHT));
    printf("Frame size: %d x %d \n",S.width,S.height);

    double rate = cap.get(CAP_PROP_FPS);//Frame rate
    printf("Frame rate: %f \n", rate);
    int dperiod = 1000 / rate;
    //dperiod = 1;
    int ex = VideoWriter::fourcc('M', 'J', 'P', 'G');
    //int ex = VideoWriter::fourcc('X', 'V', 'I', 'D');//https://www.xvid.com/
    //int ex = VideoWriter::fourcc('X', '2', '6', '4');
    //int ex = -1;//pop up window to choose
    
    const string vpath="./capture.avi";
    VideoWriter outputVideo(vpath, ex , rate, S, true);
    if (!outputVideo.isOpened())
    {
        cout << "Could not open the output video to write."<< endl;
        waitKey(5000);
        return -1;
    }


    Mat frame;
    for (int i = 0;; i++) {
        if (!cap.read(frame)) break;
        outputVideo << frame;
        imshow("Frame", frame);
        if (waitKey(dperiod) == 27) break; //if 'Esc' key is pressed
    }
    cap.release();
    outputVideo.release();
    //waitKey(5000);
    return 0;
}
