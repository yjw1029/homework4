#include "FindContours.h"
int ustc_ConnectedComponentLabeling(
	Mat grayImg,
	Mat& labelImg,
	int bin_th) {
	int row = grayImg.rows;
	int col = grayImg.cols;
	int m = 1;

	if (NULL == grayImg.data)
	{
		printf("grayImg error\n");
		return MY_FAIL;
	}

	Mat zerov, zeroh;
	zerov.create(row, 1, CV_8U);
	zeroh.create(1, col + 2, CV_8U);
	Mat grayImg_e;
	grayImg_e.create(row + 2, col + 2, CV_8UC1);
	grayImg_e.setTo(0);

	labelImg.create(row, col, CV_32SC1);
	labelImg.setTo(0);
	Mat imgROI = grayImg_e(Rect(1, 1, col, row));
	grayImg.convertTo(imgROI, CV_8UC1);

	Mat binaryImg;
	threshold(grayImg_e, binaryImg, bin_th, 1, THRESH_BINARY);

	//Mat binaryImg_show;
	//threshold(grayImg, binaryImg_show, bin_th, 255, THRESH_BINARY);
	//imshow("binary", binaryImg_show);
	//waitKey(0);


	Mat g;
	g.create(row + 2, col + 2, CV_32SC1);


	int tablesize = row*col / 4;
	int* rtable = (int*)malloc(tablesize);
	int* next = (int*)malloc(tablesize);
	int* tail = (int*)malloc(tablesize);
	g.setTo(0);
	memset(rtable, 0, tablesize);
	memset(next, 0, tablesize);
	memset(tail, 0, tablesize);
	m = 1;
	//first scan
	for (int i = 1; i <= row; i++)
	{
		uchar* row_down = binaryImg.ptr<uchar>(i);
		int* gData_up = g.ptr<int>(i - 1);
		int* gData_down = g.ptr<int>(i);
		for (int j = 1; j <= col; j++)
		{
			if (row_down[j] != 0)//x,y
			{
				int c3 = gData_up[j]; //x-1,y
				if (c3)
					gData_down[j] = c3;
				else
				{
					int c1 = gData_down[j - 1];
					if (c1)
					{
						gData_down[j] = c1;
						int c4 = gData_up[j + 1]; //x-1,y+1
						if (c4)
						{
							int u = rtable[c1]; int v = rtable[c4];
							if (u < v)
							{
								int k = v;
								while (k != -1)
								{
									rtable[k] = u;
									k = next[k];
								}
								next[tail[u]] = v;
								tail[u] = tail[v];
							}
							else if (u > v)
							{
								int k = u;
								while (k != -1)
								{
									rtable[k] = v;
									k = next[k];
								}
								next[tail[v]] = u;
								tail[v] = tail[u];
							}
						}
					}
					else
					{
						int c2 = gData_up[j - 1];  //x-1,y-1
						if (c2)
						{
							gData_down[j] = c2;
							int c4 = gData_up[j + 1]; //x-1,y+1
							if (c4)
							{
								int u = rtable[c2]; int v = rtable[c4];
								if (u < v)
								{
									int k = v;
									while (k != -1)
									{
										rtable[k] = u;
										k = next[k];
									}
									next[tail[u]] = v;
									tail[u] = tail[v];
								}
								else if (u > v)
								{
									int k = u;
									while (k != -1)
									{
										rtable[k] = v;
										k = next[k];
									}
									next[tail[v]] = u;
									tail[v] = tail[u];
								}
							}
						}
						else
						{
							int c4 = gData_up[j + 1]; //x-1,y+1
							if (c4)
								gData_down[j] = c4;
							else {
								gData_down[j] = m;
								rtable[m] = m;
								next[m] = -1;
								tail[m] = m;
								m++;
							}
						}
					}
				}
			}
		}
	}
	//second scan
	for (int i = 1; i <= row; i++)
	{
		int* gData = g.ptr<int>(i);
		int* dstData = labelImg.ptr<int>(i - 1);
		int j;
		int jend = col - 8;
		for (j = 1; j <= jend; j += 8)
		{
			dstData[j - 1] = rtable[gData[j]];
			dstData[j] = rtable[gData[j + 1]];
			dstData[j + 1] = rtable[gData[j + 2]];
			dstData[j + 2] = rtable[gData[j + 3]];
			dstData[j + 3] = rtable[gData[j + 4]];
			dstData[j + 4] = rtable[gData[j + 5]];
			dstData[j + 5] = rtable[gData[j + 6]];
			dstData[j + 6] = rtable[gData[j + 7]];
		}
		j -= 8;
		for (; j <= col; j++)
		{
			dstData[j - 1] = rtable[gData[j]];
		}
	}
	free(rtable);
	free(next);
	free(tail);
	return MY_OK;

}
int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours) {
	//todo:find contours
	int dx[8] = { 1,1,0,-1,-1,-1,0,1 };
	int dy[8] = { 0,-1,-1,-1,0,1,1,1 };

	int height = binaryImg.rows;
	int width = binaryImg.cols;
	int height_e = height + 2;
	int width_e = width + 2;
	Mat LabelImg;
	LabelImg.create(height, width, CV_32SC1);
	LabelImg.setTo(0);
	ustc_ConnectedComponentLabeling(binaryImg, LabelImg, 150);
	Mat LabelImg_e;
	LabelImg_e.create(height_e, width_e, CV_32SC1);
	LabelImg_e.setTo(0);
	int*data = (int *)LabelImg.data;

	Mat imgROI = LabelImg_e(Rect(1, 1, width, height));
	LabelImg.convertTo(imgROI, CV_32SC1);

	int*data_e = (int *)LabelImg_e.data;
	for (int row_i = 1; row_i < height_e - 1; row_i++) {
		for (int col_j = 1; col_j < width_e - 1; col_j++) {
			if (data_e[row_i*width_e + col_j]) {      
				vector<cv::Point> targetcontours;
				targetcontours.push_back(Point(col_j - 1, row_i - 1)); 
				uchar place = 0;
				uchar t = 0;
				int now_y = row_i;
				int now_x = col_j;
				do
				{
					uchar flag = 0;
					for (int i = 0; i < 5; i++) {
						uchar targetpoint = (place + 6 + i) % 8; 
						int target_y = now_y + dy[targetpoint];
						int target_x = now_x + dx[targetpoint];
						if (data_e[target_y*width_e + target_x]) {
							targetcontours.push_back(Point(target_x - 1, target_y - 1));
							place = targetpoint;
							now_x = target_x;
							now_y = target_y;
							flag = 1; 
							break;
						}
					}

					if (flag == 0) {
						uchar targetpoint = (place + 3) % 8;
						int target_y = now_y + dy[targetpoint];
						int target_x = now_x + dx[targetpoint];
						if (data_e[target_y*width_e + target_x]) {
							targetcontours.push_back(Point(target_x - 1, target_y - 1));
							place = targetpoint;
							now_x = target_x;
							now_y = target_y;
						}
						else
						{
							uchar targetpoint = (place + 5) % 8;
							int target_y = now_y + dy[targetpoint];
							int target_x = now_x + dx[targetpoint];
							if (data_e[target_y*width_e + target_x]) {
								targetcontours.push_back(Point(target_x - 1, target_y - 1));
								place = targetpoint;
								now_x = target_x;
								now_y = target_y;
							}
							else {
								t++;
								place = (place + 4) % 8;
								now_y = now_y + dy[place];
								now_x = now_x + dx[place];
							}
						}

					}

				} while (Point(now_x, now_y) != Point(col_j, row_i) || t == 1);
				contours.push_back(targetcontours);
				int label = data_e[row_i*width_e + col_j];
				for (int newrow_i = 1; newrow_i < height_e - 1; newrow_i++) {
					for (int newcol_j = 1; newcol_j < width_e - 1; newcol_j++) {
						int *p = &data_e[newrow_i*width_e + newcol_j];
						if (*p == label) {
							*p = 0;
						}
					}
				}

			}
		}
	}

	return MY_OK;
}
