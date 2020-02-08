#include "opencv2/opencv.hpp"
#include <iostream>
#include <string> 
//#include <io.h>  //파일 입출력 확인

using namespace cv;
using namespace std;


class Card 
{
private:
	Mat card_img;
public:
	Card(Mat img)
	{
		if (img.empty())
		{
			cout << "Image load failed!" << endl;
			// 그다음 예외처리를 할 수 있는 무언가가 있으면...!!
		}
		card_img = img;
	}
	/*
	void card_isempty() const
	{
		if (card_img.empty())
		{
			cout << "Image load failed!" << endl;
		}
	}
	*/

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
	Mat newCardImg;
public:
	CardPreProcess(Mat img)
		: Card(img) {}

	void SortVertex()// 생성자에 넣어도 괜찮을듯
	{
		Mat src = Card::CardImg();
		Mat bin;
		threshold(src, bin, 100, 255, THRESH_BINARY_INV | THRESH_OTSU);

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

		// 예외처리 어떻게??
		if (i != 1)
		{
			if (i == 0)
			{
				cerr << "Cannot find Contour" << endl;
			}
			if (i > 1)
			{
				cerr << "too many Contours" << endl;
			}
		}

		
		/*
		깔끔한 함수는 아니고 정렬알고리즘을 쓸까하다가
		어차피 4개의 점밖에없는거 경우의 수 나눠서 줄여보자 해서 일단 대강 짜본거
		이걸로 쓸지말지는 고민해야할문제!
		*/
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
		SortVertex();
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
		
		if (newCardImg.empty())
		{
			cout << "Image load failed!" << endl;
			// 그다음 예외처리를 할 수 있는 무언가가 있으면...!!
		}
		return newCardImg;
	}

};

class CardDeck
{
private:
	Card * OriginCardList[52];
	string OriginCardName[52];
public:
	CardDeck()
	{ 
		string Name[52] = { "C10.jpg", "C2.jpg", "C3.jpg", "C4.jpg", "C5.jpg", "C6.jpg", "C7.jpg", "C8.jpg", "C9.jpg", "CA.jpg", "CJ.jpg", "CK.jpg", "CQ.jpg", "D10.jpg", "D2.jpg", "D3.jpg", "D4.jpg", "D5.jpg", "D6.jpg", "D7.jpg", "D8.jpg", "D9.jpg", "DA.jpg", "DJ.jpg", "DK.jpg", "DQ.jpg", "H10.jpg", "H2.jpg", "H3.jpg", "H4.jpg", "H5.jpg", "H6.jpg", "H7.jpg", "H8.jpg", "H9.jpg", "HA.jpg", "HJ.jpg", "HK.jpg", "HQ.jpg", "S10.jpg", "S2.jpg", "S3.jpg", "S4.jpg", "S5.jpg", "S6.jpg", "S7.jpg", "S8.jpg", "S9.jpg", "SA.jpg", "SJ.jpg", "SK.jpg", "SQ.jpg" };

		for (int i = 0; i < 52; i++)
		{
			OriginCardName[i] = Name[i];
			OriginCardList[i] = new Card(cv::imread(".\\origin_card\\" + Name[i], IMREAD_GRAYSCALE));
		}
		
	}

	~CardDeck()
	{
		for (int i = 0; i < 52; i++)
			delete OriginCardList[i];
		cout << "delete complete" << endl;
	}

	void shape_num(Mat cardimgimg);

};


int main()
{
	CardDeck deck;
	CardPreProcess card(imread("../C2_0_57.jpg", IMREAD_GRAYSCALE));
	card.PerspectiveImg();
	
	deck.shape_num(card.CardImg());
	card.ShowCard();
	

	return 0;
}


void CardDeck::shape_num(Mat cardimgimg)
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
}
