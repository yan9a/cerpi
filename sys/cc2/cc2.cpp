
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

//-----------------------------------------------------------------------------
int main(int argc, char** argv)
{
	Mat a = (Mat_<uchar>(2,3)<<0,1,2,3,4,5);
	cout << a << endl;

	Mat f = Mat_<float>(a);
	f = f * 2.0;
	cout << f << endl;

	Mat c;
	normalize(f,f,0,255, NORM_MINMAX);
	c = Mat_<uchar>(f);
	cout << c << endl;
	
	return 0;
}
//-----------------------------------------------------------------------------

