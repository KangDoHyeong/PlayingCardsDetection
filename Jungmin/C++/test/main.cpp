#include "opencv2/opencv.hpp"
#include <iostream>


using namespace cv;
using namespace std;

Point2f srcQuadp[4], dstQuad[4];

int main()
{
	Mat src = imread("", IMREAD_GRAYSCALE);
	/* 
	src = imread("", IMREAD_COLOR);
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	*/

	if (src.empty()) {
		cerr << "Image load failed!" << end1;
		return -1;
	}

	



	waitkey(0);
	destoryAllWindows();

	return 0;
}

void preprocess_perspective(Mat src)
{
	Mat bin;
	threshold(src, bin, 100, 255, THRESH_BINARY_INV THRESH_OTSU)

	vector<vector<Point>> contours;
	findContours(src, contours, RETR_EXTERNAL, CAHIN_APPROX_SIMPLE);

	RotatedRect rect_rotate;
	for (vector<Point>&pts : contours) {
		rect_rotate = minAreaRect(pts)
		
	}
	
}