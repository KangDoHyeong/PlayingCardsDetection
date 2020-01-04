#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include <io.h>

using namespace cv;
using namespace std;

Mat src, dst;
Mat origin_img[52];
string origin_img_name[52];
Point2f srcQuad[4], dstQuad[4];

int preprocess_perspective();
void sort_points(Point2f rect_pts[4]);
void origin_card();
void shape_num(Mat dst);

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

	origin_card();
	shape_num(dst);

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

	//bin 소멸 가능

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
	
	sort_points(rect_pts);
	//rect_pts 소멸가능

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

void sort_points(Point2f rect_pts[4]) 
{
	/*
	깔끔한 함수는 아니고 정렬알고리즘을 쓸까하다가
	어차피 4개의 점밖에없는거 경우의 수 나눠서 줄여보자 해서 일단 대강 짜본거
	이걸로 쓸지말지는 고민해야할문제!
	*/
	Point2f temp;

	if (rect_pts[0].y > rect_pts[1].y)
	{
		srcQuad[0] = rect_pts[1];
		srcQuad[1] = rect_pts[0];
	}
	else
	{
		srcQuad[0] = rect_pts[0];
		srcQuad[1] = rect_pts[1];
	}

	if (rect_pts[2].y > rect_pts[3].y)
	{
		srcQuad[2] = rect_pts[3];
		srcQuad[3] = rect_pts[2];
	}
	else
	{
		srcQuad[2] = rect_pts[2];
		srcQuad[3] = rect_pts[3];
	}

	if (srcQuad[1].y > srcQuad[2].y)
	{
		if (srcQuad[0].y > srcQuad[2].y)
		{
			temp = srcQuad[0];
			srcQuad[0] = srcQuad[2];
			srcQuad[2] = temp;
			if (srcQuad[2].y > srcQuad[3].y) { // 현재 2에 있는게 1에 있는 것보다 큰 값임
				temp = srcQuad[1];
				srcQuad[1] = srcQuad[3];
				srcQuad[3] = temp;
			}
			else
			{
				temp = srcQuad[1];
				srcQuad[1] = srcQuad[2];
				srcQuad[2] = temp;
				if (srcQuad[2].y > srcQuad[3].y) {
					temp = srcQuad[2];
					srcQuad[2] = srcQuad[3];
					srcQuad[3] = temp;
				}
			}
		}
		else
		{
			temp = srcQuad[1];
			srcQuad[1] = srcQuad[2];
			srcQuad[2] = temp;
			if (srcQuad[2].y > srcQuad[3].y) {
				temp = srcQuad[2];
				srcQuad[2] = srcQuad[3];
				srcQuad[3] = temp;
			}
		}
	}

	if (srcQuad[0].x > srcQuad[1].x) {
		temp = srcQuad[1];
		srcQuad[1] = srcQuad[0];
		srcQuad[0] = temp;
	}

	if (srcQuad[2].x < srcQuad[3].x) {
		temp = srcQuad[2];
		srcQuad[2] = srcQuad[3];
		srcQuad[3] = temp;
	}

}


void origin_card()
{
	string path = ".\\origin_card\\*.*";
	struct _finddata_t fd;

	int i = 0;
	intptr_t handle;
	if ((handle = _findfirst(path.c_str(), &fd)) == -1L)
	{
		cout << "No file in directory!" << endl;
	}

	do
	{
		if (fd.size != 0)
		{
			origin_img_name[i] = fd.name;
			origin_img[i] = imread(".\\origin_card\\" + origin_img_name[i], IMREAD_GRAYSCALE);
			//cout << fd.name << endl;
			i += 1;
		}

	} while (_findnext(handle, &fd) == 0);

	_findclose(handle);
}


void shape_num(Mat dst)
{
	Mat dst_abs;
	Scalar summation;
	int min_abs = 2500000;
	int index = -1;

	for (int i = 0; i < 52; i++)
	{
		absdiff(dst, origin_img[i], dst_abs);
		summation = sum(dst_abs);
		if ((int)summation[0] < min_abs) {
			min_abs = (int)summation[0];
			index = i;
		}
	}

	if (min_abs> 2000000) 
	{
		cout<< "can't classify card"<<endl;
	}
	else
	{
		cout << origin_img_name[index] << endl;
	}
}

