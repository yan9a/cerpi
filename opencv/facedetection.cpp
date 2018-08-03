#include <stdio.h>
#include <opencv2/opencv.hpp>
#include<string>
using namespace std;
using namespace cv;

string face_cascade_name = "./haarcascade_frontalface_alt.xml";
string eyes_cascade_name = "./haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | 
        CV_HAAR_SCALE_IMAGE, Size(120, 120));// Detect faces

	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);

		Mat faceROI = frame_gray(faces[i]);
		std::vector<Rect> eyes;
		
		eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | 
            CV_HAAR_SCALE_IMAGE, Size(30, 30)); // In each face, detect eyes

		for (size_t j = 0; j < eyes.size(); j++)
		{
			Point center(faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
			circle(frame, center, radius, Scalar(255, 0, 0), 4, 8, 0);
		}
	}
	imshow("Frame", frame);
}

int main(int argc, char** argv)
{
	//Load the cascades
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading\n"); return -1; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading\n"); return -1; };

	VideoCapture cap(0);//Default camera
	if (!cap.isOpened()) { printf("Video is not opened. \n"); return -1; }
	else { printf("Video is opened. \n"); }

	union { int v; char c[5]; } uEx;
	uEx.v = static_cast<int>(cap.get(CAP_PROP_FOURCC));
	uEx.c[4] = '\0';
	printf("Codec: %s \n", uEx.c);

	//cap.set(CAP_PROP_FRAME_WIDTH, 640);
	//cap.set(CAP_PROP_FRAME_HEIGHT, 480);
	//cap.set(CAP_PROP_FPS, 30);
	Size S = Size((int)cap.get(CAP_PROP_FRAME_WIDTH), (int)cap.get(CAP_PROP_FRAME_HEIGHT));
	printf("Frame size: %d x %d \n", S.width, S.height);

	double rate = cap.get(CAP_PROP_FPS);//Frame rate
	printf("Frame rate: %f \n", rate);
	int dperiod = 33;

	Mat frame;
	for (int i = 0;; i++) {
		if (!cap.read(frame)) break;
		detectAndDisplay(frame);
		if (waitKey(dperiod) == 27) break; //if 'Esc' key is pressed
	}
	cap.release();
	//waitKey(5000);
	return 0;
}

