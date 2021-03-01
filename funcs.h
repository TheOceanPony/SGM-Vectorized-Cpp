#pragma once
#include "opencv2/opencv.hpp"
using namespace cv;

void testFunc(Mat& testMat);

void initD(Mat& D, int Dx_min, int Dx_max, int Dy_min, int Dy_max);
void showD(Mat& D);


void initH(Mat& H, Mat imL, Mat imR, Mat D);
void initG(Mat& G, Mat D, float alpha);

float Left(int y, int x, int d, Mat& L, Mat H, Mat G, Mat D);
float Right(int y, int x, int d, Mat& R, Mat H, Mat G, Mat D);
float Up(int y, int x, int d, Mat& U, Mat H, Mat G, Mat D);
float Bottom(int y, int x, int d, Mat& B, Mat H, Mat G, Mat D);

void reconstruct(Mat& Res, Mat L, Mat R, Mat U, Mat B, Mat H, Mat G, Mat D);