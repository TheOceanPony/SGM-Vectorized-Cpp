#include <iostream>
#include <stdint.h>
#include <time.h>
#include <opencv2/opencv.hpp>

#include "funcs.h"

using namespace cv;
using namespace std;

int WindowStartX = 1000, WindowStartY = 300, WindowMargin = 10;
float Scale = 1; // Windows settings


float alpha = 10;

int Dx_min = 0;
int Dx_max = 50;
int Dy_min = 0;
int Dy_max = 0;

int Dx_size = Dx_max - Dx_min + 1;
int Dy_size = Dy_max - Dy_min + 1;

int main()
{


    ////////////// Img ////////////////
    Mat imL = imread("imgs/c1.png", IMREAD_GRAYSCALE);
    Mat imR = imread("imgs/c2.png", IMREAD_GRAYSCALE);
    imL.convertTo(imL, CV_32F);
    imR.convertTo(imR, CV_32F);

    int im_width = imL.cols;
    int im_height = imL.rows;

    time_t start, end;
    time(&start);

    /////////////// D /////////////////
    cout << ">> D Init..." << endl;
    int D_size = (Dx_max - Dx_min + 1) * (Dy_max - Dy_min + 1);
    Mat D = Mat(D_size,2, CV_32S);
    initD(D, Dx_min, Dx_max, Dy_min, Dy_max);
    cout << "D_size: "<< D_size << endl;
    //showD(D);


    /////////////// H /////////////////
    cout << ">> H Init..." << endl;
    int size[3] = { im_height, im_width, D_size };
    Mat H = Mat(3, size, CV_32F, Scalar(0));
    initH(H, imL, imR, D);

    
    /////////////// G /////////////////
    cout << ">> G Init..." << endl;
    Mat G = Mat(D_size, D_size, CV_32F, Scalar(0));
    initG(G, D, alpha);


    /////////////// L /////////////////
    cout << ">> L Init..." << endl;
    Mat L = Mat(3, size, CV_32F, Scalar(0));

    for (int y = 0; y < im_height; y++)
    {
        for (int x = 0; x < im_width; x++)
        {
            for (int d = 0; d < D_size; d++)
            {
                L.at<float>(y, x, d) = Left(y, x, d, L, H, G, D);
            }
        }
    }


    /////////////// U /////////////////
    cout << ">> U Init..." << endl;
    Mat U = Mat(3, size, CV_32F, Scalar(0));

    for (int y = 0; y < im_height; y++)
    {
        for (int x = 0; x < im_width; x++)
        {
            for (int d = 0; d < D_size; d++)
            {
                U.at<float>(y, x, d) = Up(y, x, d, U, H, G, D);
            }
        }
    }


    /////////////// R /////////////////
    cout << ">> R Init..." << endl;
    Mat R = Mat(3, size, CV_32F, Scalar(0));

    for (int y = im_height-1; y >= 0 ; y--)
    {
        for (int x = im_width-1; x >= 0; x--)
        {
            for (int d = 0; d < D_size; d++)
            {
                R.at<float>(y, x, d) = Right(y, x, d, R, H, G, D);
            }
        }
    }


    /////////////// B /////////////////
    cout << ">> B Init..." << endl;
    Mat B = Mat(3, size, CV_32F, Scalar(0));

    for (int y = im_height - 1; y >= 0; y--)
    {
        for (int x = im_width - 1; x >= 0; x--)
        {
            for (int d = 0; d < D_size; d++)
            {
                B.at<float>(y, x, d) = Bottom(y, x, d, B, H, G, D, im_height);
            }
        }
    }



    ////////////// Res ////////////////
    cout << ">> Reconstructing..." << endl;
    Mat Res = Mat(im_height, im_width, CV_32F, Scalar(0));
    reconstruct(Res, L, R, U, B, H, G, D);

    
    time(&end);
    double time_taken = double(end - start);
    cout << "Time taken by program is : " << fixed
        << time_taken << setprecision(5);
    cout << " sec " << endl;

    ////////////// Clr ////////////////
    cout << ">> Clr..." << endl;
    Mat Bw = Mat(im_height, im_width, CV_32F, Scalar(0));

    Mat Clr = Mat(im_height, im_width, CV_8UC3, Scalar(0, 0, 0));

    for (int y = 0; y < im_height; y++)
    {
        for (int x = 0; x < im_width; x++)
        {

            int best_d = Res.at<float>(y, x);
            int dy = D.at<int>(best_d, 0);
            int dx = D.at<int>(best_d, 1);

            int maxD = max(Dx_size, Dy_size);
            Bw.at<float>(y, x) = sqrt(dy * dy + dx * dx) * 255 / maxD;

            if (dx >= 0)
            {
                Clr.at<Vec3b>(y, x)[0] = abs(dx) * 255/ Dx_size;
            }
            else
            {
                Clr.at<Vec3b>(y, x)[2] = abs(dx) * 255 / Dx_size;
            }

            if (dy >= 0)
            {
                Clr.at<Vec3b>(y, x)[1] = abs(dy) * 255 / Dy_size;
            }
            else
            {
                Clr.at<Vec3b>(y, x)[0] = abs(dy) * 122 / Dy_size;
                Clr.at<Vec3b>(y, x)[1] = abs(dy) * 122 / Dy_size;
            }

        }
    }

    Res.convertTo(Res, CV_8U);
    Bw.convertTo(Bw, CV_8U);

    imshow("BW", Bw);
    /*imshow("R", Red);
    imshow("G", Green);
    imshow("B", Blue);
    imshow("Y", Yellow);*/

    imshow("Clr", Clr);
    //imshow("Clr", Clr);
    waitKey();
}

//Mat imL = imread("imgs/a1.png", IMREAD_GRAYSCALE);
//Mat imR = imread("imgs/a2.png", IMREAD_GRAYSCALE);
//
//int im_width = imL.cols;
//int im_height = imL.rows;
//
//Mat H = Mat(im_height, im_height, CV_32F);
//
///*
//Mat testMat = Mat(400, 400, CV_32S);
//testFunc(testMat);
//testMat.convertTo(testMat, CV_8U);
//imshow("test", testMat);
//*/
//
//
//namedWindow("Left", WINDOW_FREERATIO);
//namedWindow("Right", WINDOW_FREERATIO);
//
//imshow("Left", imL);
//imshow("Right", imR);
//
//
//moveWindow("Left", WindowStartX, WindowStartY);
//moveWindow("Right", WindowStartX + 300, WindowStartY);
//
//waitKey();