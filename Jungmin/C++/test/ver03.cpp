#include "opencv2/opencv.hpp"
#include <iostream>
#include <string> 

using namespace cv;
using namespace std;


class Card
{
private:
	Mat card_img;
public:
	Card(Mat img) :card_img(img)
	{	}
	Mat CardImg() const
	{
		return card_img;
	}
	void ShowCard() const
	{
		cv::imshow("card", card_img);
		waitKey(0);
		destroyAllWindows();
	}
};

class CardPreProcess :public Card
{
private:
	Point2f CardQuad[4];
	Mat newCardImg;  // 좌측 상단부터 시계방향으로 4개의 점
public:
	CardPreProcess(Mat img)
		: Card(img), newCardImg(img)
	{
		CardQuad[0].x = 0;
		CardQuad[0].y = 0;
		CardQuad[1].x = 100;
		CardQuad[1].y = 0;
		CardQuad[2].x = 100;
		CardQuad[2].y = 120;
		CardQuad[3].x = 0;
		CardQuad[3].y = 120;
	}

	/*
	FindCard에서 findContour을 통해 card 이미지 1개와 꼭짓점4개 찾아냄
	-> card의 꼭짓점을 perspective 변환에 사용할 수 있게 좌측상단부터 시계방향으로 정의함
	-> perspective 함수를 사용하여 100*120의 카드 이미지 생성
	*/

	void FindCard() throw (int) 
	{
		Mat bin;
		threshold(newCardImg, bin, 100, 255, THRESH_BINARY_INV | THRESH_OTSU);

		vector<vector<Point>> contours;
		findContours(bin, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

		int i = 0;

		Point2f rect_pts[4];

		for (vector<Point>&pts : contours)
		{

			RotatedRect rect_rotate = minAreaRect(pts);
			Size2f rect_wh = rect_rotate.size;


			if ((rect_wh.area() < 9000.) | (rect_wh.area() > 15000.))
			{
				continue;
			}

			rect_rotate.points(rect_pts);

			i += 1;
		}

		if (i != 1)
		{
			throw i;
		}
		SortVertex(rect_pts);
	}

	void SortVertex(Point2f * rect_pts) 
	{
		Point2f temp;

		if (rect_pts[0].y > rect_pts[1].y)
		{
			CardQuad[0] = rect_pts[1];
			CardQuad[1] = rect_pts[0];
		}
		else
		{
			CardQuad[0] = rect_pts[0];
			CardQuad[1] = rect_pts[1];
		}

		if (rect_pts[2].y > rect_pts[3].y)
		{
			CardQuad[2] = rect_pts[3];
			CardQuad[3] = rect_pts[2];
		}
		else
		{
			CardQuad[2] = rect_pts[2];
			CardQuad[3] = rect_pts[3];
		}

		if (CardQuad[1].y > CardQuad[2].y)
		{
			if (CardQuad[0].y > CardQuad[2].y)
			{
				temp = CardQuad[0];
				CardQuad[0] = CardQuad[2];
				CardQuad[2] = temp;
				if (CardQuad[2].y > CardQuad[3].y) { // 현재 2에 있는게 1에 있는 것보다 큰 값임
					temp = CardQuad[1];
					CardQuad[1] = CardQuad[3];
					CardQuad[3] = temp;
				}
				else
				{
					temp = CardQuad[1];
					CardQuad[1] = CardQuad[2];
					CardQuad[2] = temp;
					if (CardQuad[2].y > CardQuad[3].y) {
						temp = CardQuad[2];
						CardQuad[2] = CardQuad[3];
						CardQuad[3] = temp;
					}
				}
			}
			else
			{
				temp = CardQuad[1];
				CardQuad[1] = CardQuad[2];
				CardQuad[2] = temp;
				if (CardQuad[2].y > CardQuad[3].y) {
					temp = CardQuad[2];
					CardQuad[2] = CardQuad[3];
					CardQuad[3] = temp;
				}
			}
		}

		if (CardQuad[0].x > CardQuad[1].x) {
			temp = CardQuad[1];
			CardQuad[1] = CardQuad[0];
			CardQuad[0] = temp;
		}

		if (CardQuad[2].x < CardQuad[3].x) {
			temp = CardQuad[2];
			CardQuad[2] = CardQuad[3];
			CardQuad[3] = temp;
		}

	}

	void PerspectiveImg()
	{
		FindCard();
		Point2f dstQuad[4];
		int w = 100;
		int h = 120;
		dstQuad[0] = Point2f(0, 0);
		dstQuad[1] = Point2f(w - 1, 0);
		dstQuad[2] = Point2f(w - 1, h - 1);
		dstQuad[3] = Point2f(0, h - 1);

		Mat pers = getPerspectiveTransform(CardQuad, dstQuad);
		warpPerspective(Card::CardImg(), newCardImg, pers, Size(w, h));
	}

	void ShowCard() const
	{
		cv::imshow("card", newCardImg);
		waitKey(0);
		destroyAllWindows();
	}

	Mat CardImg() const
	{
		return newCardImg;
	}

};

class CardDeckClassification
{
private:
	Card * OriginCardList[52];
	string OriginCardName[52];
public:
	CardDeckClassification()
	{
		string Name[52] = { "C10.jpg", "C2.jpg", "C3.jpg", "C4.jpg", "C5.jpg", "C6.jpg", "C7.jpg", "C8.jpg", "C9.jpg", "CA.jpg", "CJ.jpg", "CK.jpg", "CQ.jpg", "D10.jpg", "D2.jpg", "D3.jpg", "D4.jpg", "D5.jpg", "D6.jpg", "D7.jpg", "D8.jpg", "D9.jpg", "DA.jpg", "DJ.jpg", "DK.jpg", "DQ.jpg", "H10.jpg", "H2.jpg", "H3.jpg", "H4.jpg", "H5.jpg", "H6.jpg", "H7.jpg", "H8.jpg", "H9.jpg", "HA.jpg", "HJ.jpg", "HK.jpg", "HQ.jpg", "S10.jpg", "S2.jpg", "S3.jpg", "S4.jpg", "S5.jpg", "S6.jpg", "S7.jpg", "S8.jpg", "S9.jpg", "SA.jpg", "SJ.jpg", "SK.jpg", "SQ.jpg" };

		for (int i = 0; i < 52; i++)
		{
			OriginCardName[i] = Name[i];
			OriginCardList[i] = new Card(cv::imread(".\\origin_card\\" + Name[i], IMREAD_GRAYSCALE));
		}

	}

	~CardDeckClassification()
	{
		for (int i = 0; i < 52; i++)
			delete OriginCardList[i];
		cout << "delete complete" << endl;
	}

	void shape_num(Mat cardimgimg)
	{

		Mat dst_abs;
		Scalar summation;
		int min_abs = 2500000;
		int index = -1;

		for (int i = 0; i < 52; i++)
		{
			absdiff(cardimgimg, OriginCardList[i]->CardImg(), dst_abs);
			summation = sum(dst_abs);
			if ((int)summation[0] < min_abs) {
				min_abs = (int)summation[0];
				index = i;
			}
		}

		if (min_abs > 2000000)
		{
			cout << "can't classify card" << endl;
		}
		else
		{
			cout << OriginCardName[index] << endl;
		}
	};

};

int main()
{
	CardDeckClassification deck;

	try
	{
		Mat img = imread("../C2_0_57.jpg", IMREAD_GRAYSCALE);
		if (img.empty())
		{
			throw img;
		}
		CardPreProcess card(img);
		card.PerspectiveImg();

		deck.shape_num(card.CardImg());
		card.ShowCard();
	}

	catch (Mat expn)
	{
		cout << "Image load failed!" << endl;
	}
	catch (int expn)
	{
		if (expn == 0)
		{
			cerr << "Cannot find Contour" << endl;
		}
		if (expn > 1)
		{
			cerr << "too many Contours" << endl;
		}
	}

	return 0;
}


