#include<opencv2/opencv.hpp>
#include<math.h>
#include <iostream> 
#include <stdlib.h> 
#include <time.h>  
#include<vector>

using namespace cv;
using namespace std;

#define MY_OK 1
#define MY_FAIL 0
#define MY_BACKGROUND 0
#define MY_SCAN_LEN 4

#define SHOW

int ustc_ConnectedComponentLabeling(
	Mat grayImg,
	Mat& labelImg,
	int bin_th,
	int &label_max);

int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours){

	int mRows = binaryImg.rows;  
	int mCols = binaryImg.cols;  
	int bin_th = 128;
	int label_max = 0;

	Mat labelImg(binaryImg.rows, binaryImg.cols, CV_32FC1);
	int *data_label = (int *)labelImg.data;

	ustc_ConnectedComponentLabeling(binaryImg, labelImg, bin_th, label_max);

	Mat ex_labelImg(binaryImg.rows + 2, binaryImg.cols + 2, CV_32FC1);
	ex_labelImg.setTo(MY_BACKGROUND);
	int *before_position = (int *)ex_labelImg.data;
	int mRows_ex = ex_labelImg.rows;  
	int mCols_ex = ex_labelImg.cols; 
	for (int i = 0; i < binaryImg.rows; i++)
	{
		for (int j = 0; j < binaryImg.cols; j++)
		{
			before_position[(i + 1)*mCols_ex + (j + 1)] = data_label[i*binaryImg.cols + j];
		}
	}
	int* label_table = new int[label_max + 1]();
	int label_now=0;
	int vector_num = 0;
		for (int i = 1; i < mRows_ex - 1; i++){
			for (int j = 1; j < mCols_ex - 1; j++){
				label_now = before_position[(i)*mCols_ex + (j)];
				if (label_now && (!label_table[label_now])){
					label_table[label_now] = 1;                      
					Point pt_start(i-1, j-1);
					vector_num++;
					contours[vector_num].push_back(pt_start);
					int t_end = 0;                       
					int recent_position = 6;
					int x=i, y=j;
		
					do{
						switch (recent_position){
						case 0:
							if (before_position[(x + 1)*mCols_ex + (y - 1)] == label_now){
								x = x + 1;
								y = y - 1;
								recent_position = 5;
							}
							else if (before_position[(x+1)*mCols_ex + (y)] == label_now){
								x = x + 1;
								y = y;
								recent_position = 6;
							}
							else if (before_position[(x + 1)*mCols_ex + (y + 1)] == label_now){
								x = x + 1;
								y = y + 1;
								recent_position = 7;
							}
							else if (before_position[(x)*mCols_ex + (y + 1)] == label_now){
								x = x;
								y = y + 1;
								recent_position = 0;
							}
							else if (before_position[(x - 1)*mCols_ex + (y + 1)] == label_now){
								x = x - 1;
								y = y + 1;
								recent_position = 1;
							}
							else if (before_position[(x - 1)*mCols_ex + (y)] == label_now){
								x = x - 1;
								y = y;
								recent_position = 2;
							}
							else if (before_position[(x - 1)*mCols_ex + (y - 1)] == label_now){
								x = x - 1;
								y = y - 1;
								recent_position = 3;
							}
							else{
								t_end++;
								recent_position = 4;
							}
							break;
						case 1:
							if (before_position[(x + 1)*mCols_ex + (y)] == label_now){
								x = x + 1;
								y = y ;
								recent_position = 6;
							}
							else if (before_position[(x + 1)*mCols_ex + (y + 1)] == label_now){
								x = x + 1;
								y = y + 1;
								recent_position = 7;
							}
							else if (before_position[(x)*mCols_ex + (y + 1)] == label_now){
								x = x;
								y = y + 1;
								recent_position = 0;
							}
							else if (before_position[(x - 1)*mCols_ex + (y + 1)] == label_now){
								x = x - 1;
								y = y + 1;
								recent_position = 1;
							}
							else if (before_position[(x - 1)*mCols_ex + (y)] == label_now){
								x = x - 1;
								y = y;
								recent_position = 2;
							}
							else if (before_position[(x - 1)*mCols_ex + (y - 1)] == label_now){
								x = x - 1;
								y = y - 1;
								recent_position = 3;
							}
							else if (before_position[(x)*mCols_ex + (y - 1)] == label_now){
								x = x;
								y = y - 1;
								recent_position = 4;
							}
							else{
								t_end++;
								recent_position = 5;
							}
							break;
						case 2:
							if (before_position[(x + 1)*mCols_ex + (y + 1)] == label_now){
								x = x + 1;
								y = y + 1;
								recent_position = 7;
							}
							else if (before_position[(x)*mCols_ex + (y + 1)] == label_now){
								x = x;
								y = y + 1;
								recent_position = 0;
							}
							else if (before_position[(x - 1)*mCols_ex + (y + 1)] == label_now){
								x = x - 1;
								y = y + 1;
								recent_position = 1;
							}
							else if (before_position[(x - 1)*mCols_ex + (y)] == label_now){
								x = x - 1;
								y = y;
								recent_position = 2;
							}
							else if (before_position[(x - 1)*mCols_ex + (y - 1)] == label_now){
								x = x - 1;
								y = y - 1;
								recent_position = 3;
							}
							else if (before_position[(x)*mCols_ex + (y - 1)] == label_now){
								x = x ;
								y = y - 1;
								recent_position = 4;
							}
							else if (before_position[(x + 1)*mCols_ex + (y - 1)] == label_now){
								x = x + 1;
								y = y - 1;
								recent_position = 5;
							}
							else{
								t_end++;
								recent_position = 6;
							}
							break;
						case 3:
							if (before_position[(x)*mCols_ex + (y + 1)] == label_now){
								x = x;
								y = y + 1;
								recent_position = 0;
							}
							else if (before_position[(x - 1)*mCols_ex + (y + 1)] == label_now){
								x = x - 1;
								y = y + 1;
								recent_position = 1;
							}
							else if (before_position[(x - 1)*mCols_ex + (y)] == label_now){
								x = x - 1;
								y = y;
								recent_position = 2;
							}
							else if (before_position[(x - 1)*mCols_ex + (y - 1)] == label_now){
								x = x - 1;
								y = y - 1;
								recent_position = 3;
							}
							else if (before_position[(x)*mCols_ex + (y - 1)] == label_now){
								x = x;
								y = y - 1;
								recent_position = 4;
							}
							else if (before_position[(x + 1)*mCols_ex + (y - 1)] == label_now){
								x = x + 1;
								y = y - 1;
								recent_position = 5;
							}
							else if (before_position[(x + 1)*mCols_ex + (y)] == label_now){
								x = x + 1;
								y = y;
								recent_position = 6;
							}
							else{
								t_end++;
								recent_position = 7;
							}
							break;
						case 4:
							if (before_position[(x - 1)*mCols_ex + (y + 1)] == label_now){
								x = x - 1;
								y = y + 1;
								recent_position = 1;
							}
							else if (before_position[(x - 1)*mCols_ex + (y)] == label_now){
								x = x - 1;
								y = y;
								recent_position = 2;
							}
							else if (before_position[(x - 1)*mCols_ex + (y - 1)] == label_now){
								x = x - 1;
								y = y - 1;
								recent_position = 3;
							}
							else if (before_position[(x)*mCols_ex + (y - 1)] == label_now){
								x = x;
								y = y - 1;
								recent_position = 4;
							}
							else if (before_position[(x + 1)*mCols_ex + (y - 1)] == label_now){
								x = x + 1;
								y = y - 1;
								recent_position = 5;
							}
							else if (before_position[(x + 1)*mCols_ex + (y)] == label_now){
								x = x + 1;
								y = y;
								recent_position = 6;
							}
							else if (before_position[(x + 1)*mCols_ex + (y + 1)] == label_now){
								x = x + 1;
								y = y + 1;
								recent_position = 7;
							}
							else{
								t_end++;
								recent_position = 0;
							}
							break;
						case 5:
							if (before_position[(x - 1)*mCols_ex + (y)] == label_now){
								x = x - 1;
								y = y;
								recent_position = 2;
							}
							else if (before_position[(x - 1)*mCols_ex + (y - 1)] == label_now){
								x = x - 1;
								y = y - 1;
								recent_position = 3;
							}
							else if (before_position[(x)*mCols_ex + (y - 1)] == label_now){
								x = x;
								y = y - 1;
								recent_position = 4;
							}
							else if (before_position[(x + 1)*mCols_ex + (y - 1)] == label_now){
								x = x + 1;
								y = y - 1;
								recent_position = 5;
							}
							else if (before_position[(x + 1)*mCols_ex + (y)] == label_now){
								x = x + 1;
								y = y;
								recent_position = 6;
							}
							else if (before_position[(x + 1)*mCols_ex + (y + 1)] == label_now){
								x = x + 1;
								y = y + 1;
								recent_position = 7;
							}
							else if (before_position[(x)*mCols_ex + (y + 1)] == label_now){
								x = x;
								y = y + 1;
								recent_position = 0;
							}
							else{
								t_end++;
								recent_position = 1;
							}
							break;
						case 6:
							if (before_position[(x - 1)*mCols_ex + (y - 1)] == label_now){
								x = x - 1;
								y = y - 1;
								recent_position = 3;
							}
							else if (before_position[(x)*mCols_ex + (y - 1)] == label_now){
								x = x;
								y = y - 1;
								recent_position = 4;
							}
							else if (before_position[(x + 1)*mCols_ex + (y - 1)] == label_now){
								x = x + 1;
								y = y - 1;
								recent_position = 5;
							}
							else if (before_position[(x + 1)*mCols_ex + (y)] == label_now){
								x = x + 1;
								y = y;
								recent_position = 6;
							}
							else if (before_position[(x + 1)*mCols_ex + (y + 1)] == label_now){
								x = x + 1;
								y = y + 1;
								recent_position = 7;
							}
							else if (before_position[(x)*mCols_ex + (y + 1)] == label_now){
								x = x;
								y = y + 1;
								recent_position = 0;
							}
							else if (before_position[(x - 1)*mCols_ex + (y + 1)] == label_now){
								x = x - 1;
								y = y + 1;
								recent_position = 1;
							}
							else{
								t_end++;
								recent_position = 2;
							}
							break;
						case 7:
							if (before_position[(x)*mCols_ex + (y - 1)] == label_now){
								x = x;
								y = y - 1;
								recent_position = 4;
							}
							else if (before_position[(x + 1)*mCols_ex + (y - 1)] == label_now){
								x = x + 1;
								y = y - 1;
								recent_position = 5;
							}
							else if (before_position[(x + 1)*mCols_ex + (y)] == label_now){
								x = x + 1;
								y = y;
								recent_position = 6;
							}
							else if (before_position[(x + 1)*mCols_ex + (y + 1)] == label_now){
								x = x + 1;
								y = y + 1;
								recent_position = 7;
							}
							else if (before_position[(x)*mCols_ex + (y + 1)] == label_now){
								x = x;
								y = y + 1;
								recent_position = 0;
							}
							else if (before_position[(x - 1)*mCols_ex + (y + 1)] == label_now){
								x = x - 1;
								y = y + 1;
								recent_position = 1;
							}
							else if (before_position[(x - 1)*mCols_ex + (y)] == label_now){
								x = x - 1;
								y = y;
								recent_position = 2;
							}
							else{
								t_end++;
								recent_position = 3;
							}
							break;
						default:
							break;
						}
						Point pt_now(x-1, y-1);
						contours[vector_num].push_back(pt_now);
						if ((pt_now == pt_start &&t_end!=1) || t_end==2){
							break;
						}
					} while (1);


				}





			}
		}
	
	


	
#ifdef SHOW
	Mat printImg(mRows_ex, mCols_ex, CV_8UC1);
	uchar *data_print = (uchar *)printImg.data;
	for (int i = 0; i < mRows_ex; i++)
	{
	for (int j = 0; j < mCols_ex; j++)
	{
		data_print[i*mCols_ex + j] = 50 *before_position[i*mCols_ex + j];
	}
	cout << endl;
	}

	Mat border_labelImg(mRows_ex, mCols_ex, CV_8UC1);
	border_labelImg.setTo(MY_BACKGROUND);
	for (int i = 1; i < 1000; i++)
	{
		for (int j = 0; j < contours[i].size(); j++){
			border_labelImg.data[contours[i][j].x * mCols_ex + contours[i][j].y] = 255;
		}
	}

	namedWindow("Img", 0);
	imshow("Img", binaryImg);

	namedWindow("exImg", 0);
	imshow("exImg", printImg);

	namedWindow("borderImg", 0);
	imshow("borderImg", border_labelImg);
#endif
	return 0;
}

void my_connect(int *label, int first, int second);

int ustc_ConnectedComponentLabeling(Mat grayImg, Mat & labelImg, int bin_th,int &label_max)
{
	uchar *data_img = (uchar *)grayImg.data;
	if (NULL == data_img)
	{
		cout << "failed to read img!" << endl;
		return MY_FAIL;
	}

	int *data_label = (int *)labelImg.data;
	if (NULL == data_label)
	{
		cout << "labelImg is NULL!" << endl;
		return MY_FAIL;
	}

	int mRows = grayImg.rows;  
	int mCols = grayImg.cols;  
	int len = mRows*mCols / 4;  
	int *label = new int[len]();  
	int now_label = 1;  
	int scan[MY_SCAN_LEN];  
	int num = 0;  

	for (int i = 0; i < len; i++)
	{
		label[i] = i;
	}

	
	Mat ex_labelImg(mRows + 2, mCols + 2, CV_32FC1);
	ex_labelImg.setTo(MY_BACKGROUND);
	int *before_position = (int *)ex_labelImg.data;
	int mRows_ex = ex_labelImg.rows;  
	int mCols_ex = ex_labelImg.cols;  
	for (int i = 0; i < mRows; i++)
	{
		for (int j = 0; j < mCols; j++)
		{
			before_position[(i + 1)*mCols_ex + (j + 1)] = data_img[i*mCols + j];
		}
	}

	
	for (int i = 1; i < mRows + 1; i++)
	{
		for (int j = 1; j < mCols + 1; j++)
		{
			int index = i*mCols_ex + j;
			int pix = before_position[index];

			
			if (pix < bin_th)
			{
				before_position[index] = MY_BACKGROUND;
			}
			
			else
			{
				int index_left_top = (i - 1)*mCols_ex + (j - 1);
				int label_left_top = before_position[index_left_top];
				int label_top = before_position[index_left_top + 1];
				int label_right_top = before_position[index_left_top + 2];
				int label_left = before_position[index_left_top + mCols_ex];

				
				if (label_left_top != MY_BACKGROUND)
				{
					scan[num] = label_left_top;
					num++;
				}
				if (label_top != MY_BACKGROUND)
				{
					scan[num] = label_top;
					num++;
				}
				if (label_right_top != MY_BACKGROUND)
				{
					scan[num] = label_right_top;
					num++;
				}
				if (label_left != MY_BACKGROUND)
				{
					scan[num] = label_left;
					num++;
				}

				
				if (num == 0)
				{
					before_position[index] = now_label;
					now_label++;
				}
				
				else
				{
					
					int min_label = scan[0];
					for (int k = 1; k < num; k++)
					{
						if (min_label > scan[k])
						{
							min_label = scan[k];
						}
					}
					before_position[index] = min_label;

					
					for (int k = 1; k < num; k++)
					{
						my_connect(label, scan[k - 1], scan[k]);
					}
				}
				num = 0;  

			}
		}
	}

	
	for (int i = 1; i < now_label; i++)
	{
		int tag = label[i];
		
		while (label[tag] != tag)
		{
			tag = label[tag];
		}
		label[i] = tag;
	}

	
	for (int i = 1; i < mRows + 1; i++)
	{
		for (int j = 1; j < mCols + 1; j++)
		{
			int index_ex = i*mCols_ex + j;
			int index = (i - 1)*mCols + (j - 1);
			int tag = before_position[index_ex];
			data_label[index] = label[tag];
		}
	}
	label_max = now_label - 1;
	delete[]label;
	return MY_OK;
}

void my_connect(int * label, int first, int second)
{
	if (first == second)
	{
		return;
	}

	
	int label_b = first;
	int label_s = second;
	int cash;  
	if (first < second)
	{
		label_b = second;
		label_s = first;
	}


	while (label[label_b] != label_b)
	{
		label_b = label[label_b];
		
		if (label_b == label_s)
		{
			return;
		}

		
		if (label_b < label_s)
		{
			cash = label_b;
			label_b = label_s;
			label_s = cash;
		}
	}

	label[label_b] = label_s;
	return;
}
