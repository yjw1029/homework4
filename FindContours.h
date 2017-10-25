#ifndef FIND_CONTOURS_H_
#define FIND_CONTOURS_H_

#define MY_OK 1
#define MY_FAIL -1

#include <stdio.h>
#include <iostream>
#include <string>
#include "math.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;

int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours);

#endif
