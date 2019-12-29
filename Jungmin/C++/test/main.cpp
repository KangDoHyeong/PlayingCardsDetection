#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat src, dst;
Point2f srcQuad[4], dstQuad[4];

int preprocess_perspective();

int main()
{
	src = imread("../C2_0_57.jpg", IMREAD_GRAYSCALE);
	/* 
	src = imread("", IMREAD_COLOR);
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	*/

	if (src.empty()) {
		cerr << "Image load failed!" <<	endl;
		return -1;
	}

	//namedWindow("image");
	//imshow("image", src);
	
	int a = preprocess_perspective();
	if (a == -1) {
		waitKey(0);
		destroyAllWindows();

		return 0;
	}

	imshow("dst", dst);

	waitKey(0);
	destroyAllWindows();

	return 0;
}


int preprocess_perspective()
{
	Mat bin;
	threshold(src, bin, 100, 255, THRESH_BINARY_INV | THRESH_OTSU);

	vector<vector<Point>> contours;
	
	findContours(bin, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

	//bin ¼Ò¸ê °¡´É

	RotatedRect rect_rotate;
	Point2f rect_pts[4];
	Size2f rect_wh;

	/*
	int i = 0;
	for (vector<Point>&pts : contours) {

		rect_rotate = minAreaRect(pts);
		rect_wh = rect_rotate.size;


		if (rect_wh.area() < 9000. ) {
			continue;
		}

		
		rect_rotate.points(rect_pts);
		printf("%f\n", rect_wh.area());


		Scalar c(rand() & 255, rand() & 255, rand() & 255);
		drawContours(src, contours, i, c, 5);
		i += 1;
	}

	imshow("dst", src);
	*/
	
	int i = 0;

	for (vector<Point>&pts : contours) 
	{
		
		rect_rotate = minAreaRect(pts);
		rect_wh = rect_rotate.size;
		
		
		if ( (rect_wh.area() < 9000.)| (rect_wh.area() > 15000.)) 
		{
			continue;
		}
		
		rect_rotate.points(rect_pts);

		i += 1;
	}
	
	if (i != 1) 
	{
		if (i == 0)
		{
			cerr << "Cannot find Contour" << endl;
			return -1;
		}
		if (i > 1) 
		{
			cerr << "too many Contours" << endl;
			return -1;
		}
	}
	
	Point2f temp;

	if (rect_pts[0].x > rect_pts[1].x) 
	{
		srcQuad[0] = rect_pts[1];
		srcQuad[1] = rect_pts[0];
	}
	else 
	{
		srcQuad[0] = rect_pts[0];
		srcQuad[1] = rect_pts[1];
	}
	
	if (rect_pts[2].x > rect_pts[3].x) 
	{
		srcQuad[2] = rect_pts[3];
		srcQuad[3] = rect_pts[2];
	}
	else 
	{
		srcQuad[2] = rect_pts[2];
		srcQuad[3] = rect_pts[3];
	}


	if (srcQuad[0].x > srcQuad[2].x) 
	{
		temp = srcQuad[1];

	
	}


	

	srcQuad[0] = rect_pts[0];
	srcQuad[1] = rect_pts[1];
	srcQuad[2] = rect_pts[2];
	srcQuad[3] = rect_pts[3];


	int w = 100;
	int h = 120;
	dstQuad[0] = Point2f(0, 0);
	dstQuad[1] = Point2f(w - 1, 0);
	dstQuad[2] = Point2f(w - 1, h - 1);
	dstQuad[3] = Point2f(0, h - 1);

	Mat pers = getPerspectiveTransform(srcQuad, dstQuad);
	warpPerspective(src, dst, pers, Size(w, h));

	
	
	return 0;
}

	