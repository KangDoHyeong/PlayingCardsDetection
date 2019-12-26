#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Point2f srcQuadp[4], dstQuad[4];

int main()
{
	Mat src = imread("origin_card/C2.jpg", IMREAD_GRAYSCALE);
	/* 
	src = imread("", IMREAD_COLOR);
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	*/

	if (src.empty()) {
		cerr << "Image load failed!" <<	endl;
		return -1;
	}
	
	namedWindow("image");
	imshow("image", src);
	
	waitKey(0);
	destroyAllWindows();

	return 0;
}

/*
void preprocess_perspective(Mat src)
{
	Mat bin;
	threshold(src, bin, 100, 255, THRESH_BINARY_INV|THRESH_OTSU)

	vector<vector<Point>> contours;
	
	findContours(src, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	RotatedRect rect_rotate;
	for (vector<Point>&pts : contours) {
		rect_rotate = minAreaRect(pts)

	}

}
*/
