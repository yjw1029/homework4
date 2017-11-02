#include "FindContours.h"
int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours) {
if (NULL == binaryImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}
	int height = binaryImg.rows;
	int width = binaryImg.cols;
	contours.clear();
	vector < cv::Point > border;
	cv::Point soldiers;
	border.clear();

	int height_2 = height + 2;
	int width_2 = width + 2;
	int max_xy = width_2*height_2;
	Mat thresholded_2(height_2, width_2, CV_8UC1);
	int* xy = (int *)malloc((max_xy) * sizeof(int));
	int max_xylen = 0;
	for (int i = 0; i < height; i++)    
	{
		int temp0 = (i + 1)*width_2;
		int t_2 = i*width;
		for (int j = 0; j < width; j++)
		{
			int temp = temp0 + j + 1;
			int t_1 = t_2 + j;
			int pix = binaryImg.data[t_1];
			if (pix) {
				thresholded_2.data[temp] = 1;
				xy[max_xylen] = temp;
				max_xylen++;
			}
			else thresholded_2.data[temp] = 0;
		}
	}
	int last_row = width_2*(height_2 - 1);
	for (int row_i = 0; row_i < width_2; row_i++) {
		thresholded_2.data[row_i] = 0;
		thresholded_2.data[ last_row + row_i] = 0;
	}
	for (int row_i = 1; row_i < height_2; row_i++) {
		int temp = row_i*width_2;
		thresholded_2.data[temp] = 0;
		thresholded_2.data[temp + width_2 - 1] = 0;
	}
	int* T_start = (int *)malloc((width*height / 4) * sizeof(int));
	int* T = (int *)malloc((width*height / 4) * sizeof(int));
	int max_start = 0;
	int max_len = 0;
	int* L = (int *)malloc((max_xy) * sizeof(int));
	//背景色是0
	int m = 1;
	for(int row_i=0;row_i<max_xylen;row_i++){
		int temp = xy[row_i];
				int a = (thresholded_2.data[temp - 1]);
				int b = (thresholded_2.data[temp - width_2 - 1]);
				int c = (thresholded_2.data[temp - width_2]);
				int d = (thresholded_2.data[temp + 1 - width_2]);
				int binary_sum = 8 * d + 4 * c + 2 * b + a;
				switch (binary_sum)
				{
				case 0: {
					T[m] = m;
					L[temp] = m;
					T_start[m] = temp;
					m++;
				}
						break;
				case 1:L[temp] = L[temp - 1]; break;
				case 2:L[temp] = L[temp - 1 - width_2]; break;
				case 3:L[temp] = L[temp - 1]; break;
				case 4:L[temp] = L[temp - width_2]; break;
				case 5:L[temp] = L[temp - width_2]; break;
				case 6:L[temp] = L[temp - width_2]; break;
				case 7:L[temp] = L[temp - width_2]; break;
				case 8:L[temp] = L[temp - width_2 + 1]; break;
				case 12:L[temp] = L[temp - width_2 + 1]; break;
				case 13:L[temp] = L[temp - width_2 + 1]; break;
				case 14:L[temp] = L[temp - width_2 + 1]; break;
				case 15:L[temp] = L[temp - width_2 + 1]; break;
				case 9: {int label1 = L[temp - 1];
					int label2 = L[temp + 1 - width_2];
					int maximum = 0;
					int minimum = 0;
					if (T[label1] > T[label2]) {
						maximum = T[label1];
						minimum = T[label2];
					}
					else {
						maximum = T[label2];
						minimum = T[label1];
					}
					for (int i = 1; i <= m; i++)
					{
						if (T[i] == maximum) T[i] = minimum;
					}
					L[temp] = minimum;
				}
						break;
				case 11: {int label1 = L[temp - 1];
					int label2 = L[temp + 1 - width_2];
					int maximum = 0;
					int minimum = 0;
					if (T[label1] > T[label2]) {
						maximum = T[label1];
						minimum = T[label2];
					}
					else {
						maximum = T[label2];
						minimum = T[label1];
					}
					for (int i = 1; i <= m; i++)
					{
						if (T[i] == maximum) T[i] = minimum;
					}
					L[temp] = minimum;
				}
						 break;
				case 10: {int label1 = L[temp - 1 - width_2];
					int label2 = L[temp + 1 - width_2];
					int maximum = 0;
					int minimum = 0;
					if (T[label1] > T[label2]) {
						maximum = T[label1];
						minimum = T[label2];
					}
					else {
						maximum = T[label2];
						minimum = T[label1];
					}
					for (int i = 1; i <= m; i++)
					{
						if (T[i] == maximum) T[i] = minimum;
					}
					L[temp] = minimum;
				}
						 break;
				}
		}
	for (int i = 1; i < m; i++) {
		if (T[i]==i) {
			T_start[max_start] = T_start[i];
			max_start++;
		}
	}
	free(T);
	free(L);
	free(xy);


	int deviation[8] = { 1,1 - width_2,-width_2,-1 - width_2 ,-1,width_2 - 1,width_2 ,width_2 + 1 };
	int binary_subtraction[8] = { 7,0,1,2,3,4,5,6 };
	int convert[8] = { 2,3,4,5,6,7,0,1 };
	int ch_x[8] = { 1, 1, 0, -1,-1,-1,0, 1 };
	int ch_y[8] = { 0,-1,-1,-1,0,1,1,1 };
	for(int row_i=0;row_i<max_start;row_i++){
		int reference = T_start[row_i];
		int reference_2 = T_start[row_i];
		int reference_y = reference / width_2 - 1;
		int reference_x = reference - reference_y*width_2 - 1;
		soldiers.y = reference_y;
		soldiers.x = reference_x;
		border.push_back(soldiers);
		int b_m = 0;
		for (b_m = 7; b_m >= 0; b_m--)    
		{
			int index = reference + deviation[b_m];
			int pixvalue = thresholded_2.data[index];
			if (pixvalue)
			{
				reference_y += ch_y[b_m];
				reference_x += ch_x[b_m];
				soldiers.y = reference_y;
				soldiers.x = reference_x;
				border.push_back(soldiers);
				reference = index;
				break;
			}
		}
		int indicator = 1;
		if (b_m == -1) {
			indicator = 0;
		}
			int terminal = 0;
			while (indicator)
			{
				int n = 0;
				b_m = convert[b_m];
				for (n = 0; n < 5; n++)
				{
					int index = reference + deviation[b_m];
					int pixvalue = thresholded_2.data[index];
					if (pixvalue)
					{
						if (index == reference_2)
						{
							indicator = 0;
							break;
						}
						else 
						{
							reference_y += ch_y[b_m];
							reference_x += ch_x[b_m];
							soldiers.y = reference_y;
							soldiers.x = reference_x;
							border.push_back(soldiers);
							reference = index;
							break;
						}
					}
					b_m = binary_subtraction[b_m];
				}
			}
		contours.push_back(border);
		border.clear();
	}
	free(T_start);
	return MY_OK;
}
