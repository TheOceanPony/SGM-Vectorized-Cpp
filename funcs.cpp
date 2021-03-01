#include <iostream>
#include <cmath> 
#include <limits>

#include "funcs.h"

using namespace std;


void testFunc(Mat &testMat)
{

	int width = testMat.cols;
	int height = testMat.rows;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			testMat.at<int>(y, x) = y % 255;
		}
	}

}


///////////// D ///////////////
void initD(Mat &D, int Dx_min, int Dx_max, int Dy_min, int Dy_max)
{

	int dx = (Dx_max - Dx_min + 1);
	int dy = (Dy_max - Dy_min + 1);

	for (int i = 0; i < dx*dy; i++)
	{
		// print(f"{i} | ( {dy_min + (i % (dy_max - dy_min + 1))}, {dx_min + (i % (dx_max - dx_min + 1))} )")
		D.at<int>(i, 0) = Dy_min + (i % (Dy_max - Dy_min + 1));
		D.at<int>(i, 1) = Dx_min + (i / (Dy_max - Dy_min + 1));
	}
}


void showD(Mat &D)
{
	int D_size = D.rows;

	for (int i = 0; i < D_size; i++)
	{
		std::cout << i << " | " << D.at<int>(i, 0) << ", " << D.at<int>(i, 1) << std::endl;
	}
}


///////////// H ///////////////
void initH(Mat& H, Mat imL, Mat imR, Mat D)
{
	int im_width = imL.cols;
	int im_height = imL.rows;
	int D_size = D.rows;

	for (int y = 0; y < im_height; y++)
	{
		for (int x = 0; x < im_width; x++)
		{
			for (int d = 0; d < D_size; d++)
			{
				int dy = D.at<int>(d, 0);
				int dx = D.at<int>(d, 1);

				if ( y - dy >= 0 && y - dy < im_height
				  && x - dx >= 0 && x - dx < im_width )
				{
					H.at<float>(y, x, d) = abs( imL.at<float>(y, x) - imR.at<float>(y - dy, x - dx) );
				}
			}
		}
	}
}


///////////// G ///////////////
void initG(Mat& G, Mat D, float alpha)
{
	int D_size = D.rows;

	for (int d1 = 0; d1 < D_size; d1++)
	{
		for (int d2 = 0; d2 < D_size; d2++)
		{
			int d1y = D.at<int>(d1, 0);
			int d1x = D.at<int>(d1, 1);
			int d2y = D.at<int>(d2, 0);
			int d2x = D.at<int>(d2, 1);

			G.at<float>(d1, d2) = alpha * ( pow( d1y - d2y, 2) + pow(d1x - d2x, 2) );
		}
	}
}


///////////// L ///////////////
float Left(int y, int x, int d, Mat& L, Mat H, Mat G, Mat D)
{
	int D_size = D.rows;

	if (x == 0)
	{
		return 0;
	}
	else
	{
		float minleft = std::numeric_limits<float>::infinity();
		float foo = 0;

		for (int d2 = 0; d2 < D_size; d2++)
		{
			foo = L.at<float>(y, x - 1, d2) + H.at<float>(y, x - 1, d2) + G.at<float>(d, d2);
			if (foo < minleft)
			{
				minleft = foo;
			}
		}

		return minleft;
	}
}


///////////// U ///////////////
float Up(int y, int x, int d, Mat& U, Mat H, Mat G, Mat D)
{
	int D_size = D.rows;

	if (x == 0)
	{
		return 0;
	}
	else
	{
		float minup = std::numeric_limits<float>::infinity();
		float foo = 0;

		for (int d2 = 0; d2 < D_size; d2++)
		{
			foo = U.at<float>(y, x - 1, d2) + H.at<float>(y, x - 1, d2) + G.at<float>(d, d2);
			if (foo < minup)
			{
				minup = foo;
			}
		}

		return minup;
	}
}


///////////// R ///////////////
float Right(int y, int x, int d, Mat& R, Mat H, Mat G, Mat D)
{
	int im_width = R.cols;
	int D_size = D.rows;

	if (x == im_width-1)
	{
		return 0;
	}
	else
	{
		float minright = std::numeric_limits<float>::infinity();
		float foo = 0;

		for (int d2 = 0; d2 < D_size; d2++)
		{
			foo = R.at<float>(y, x + 1, d2) + H.at<float>(y, x + 1, d2) + G.at<float>(d, d2);
			if (foo < minright)
			{
				minright = foo;
			}
		}

		return minright;
	}
}


///////////// D ///////////////
float Bottom(int y, int x, int d, Mat& B, Mat H, Mat G, Mat D)
{
	int im_width = B.cols;
	int D_size = D.rows;

	if (x == im_width - 1)
	{
		return 0;
	}
	else
	{
		float minbottom = std::numeric_limits<float>::infinity();
		float foo = 0;

		for (int d2 = 0; d2 < D_size; d2++)
		{
			foo = B.at<float>(y, x + 1, d2) + H.at<float>(y, x + 1, d2) + G.at<float>(d, d2);
			if (foo < minbottom)
			{
				minbottom = foo;
			}
		}

		return minbottom;
	}
}



///////////// Res /////////////
void reconstruct(Mat& Res, Mat L, Mat R, Mat U, Mat B, Mat H, Mat G, Mat D)
{
	int im_width = Res.cols;
	int im_height = Res.rows;
	int D_size = D.rows;

	for (int y = 0; y < im_height; y++)
	{
		for (int x = 0; x < im_width; x++)
		{
			double min_score = std::numeric_limits<double>::infinity();
			int best_d = 24;
			float foo = 0;

			for (int d = 0; d < D_size; d++)
			{
				foo = L.at<float>(y, x, d) + R.at<float>(y, x, d) + U.at<float>(y, x, d) + B.at<float>(y, x, d) + H.at<float>(y, x, d);
				//cout << foo << endl;
				if (foo < min_score)
				{
					min_score = foo;
					best_d = d;
				}
			}

			//Res.at<float>(y, x) = sqrt(dy*dy + dx*dx)*255/10;
			Res.at<float>(y, x) = best_d;
		}
	}
}