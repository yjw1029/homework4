#include "FindContours.h"

#define min_ab(x,y)	((x)<(y)?(x):(y))
#define max_ab(x,y)	((x)>(y)?(x):(y))
int ustc_ConnectedComponentLabeling(
	Mat grayImg,
	Mat& labelImg,
	int bin_th);

int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours) 
{
	if (NULL == binaryImg.data)
	{
		cout << "error:binaryImg!" << endl;
		return MY_FAIL;
	}								
	
	if (binaryImg.channels() != 1)
	{
		cout << "image is not binaryImg!" << endl;
		return MY_FAIL;
	}
	

	Mat labelImg(binaryImg.rows,binaryImg.cols,CV_8UC1);

	ustc_ConnectedComponentLabeling(binaryImg, labelImg, 50);

	int rows, cols;
	rows = binaryImg.rows;
	cols = binaryImg.cols;

	uchar *data = labelImg.data;
	int *flag;
	flag = new int[256];
	memset(flag, 0, sizeof(int) * 256);
	int label=0;
	contours.clear();

	for (int i = 1; i < rows-1; i++)
	{
		for (int j = 1; j < cols-1; j++)
		{
			label = data[i*cols + j];
			if (label==0 || flag[label] == 1)
				continue;											

			
			vector < cv::Point > contour;
			cv::Point p;
			char type = 0;
			char special = 0;

			p.x = i;
			p.y = j;
			contour.push_back(p);

			if (data[i*cols + j + 1] == label)
			{
				p.x = i;
				p.y = j + 1;
				type = 8;
				contour.push_back(p);
				if (data[(i + 1)*cols + j - 1] == label)
					special = 1;
			}
			else if (data[(i + 1)*cols + j + 1] == label)
			{
				p.x = i + 1;
				p.y = j + 1;
				type = 1;
				contour.push_back(p);
				if (data[(i + 1)*cols + j - 1] == label)
					special = 1;
			}
			else if (data[(i + 1)*cols + j ] == label)
			{
				p.x = i + 1;
				p.y = j ;
				type = 2;
				contour.push_back(p);
			}
			else if (data[(i + 1)*cols + j - 1] == label)
			{
				p.x = i + 1;
				p.y = j - 1;
				type = 3;
				contour.push_back(p);
			}
				

			while ((p.x != i) || (p.y != j))					
			{
				switch (type)
				{
					case 1: {
						if (data[(p.x - 1)*cols + p.y + 1] == label)			
							p.x -= 1, p.y += 1, type = 7;
						else if (data[p.x*cols + p.y + 1] == label)
							p.y += 1, type = 8;
						else if (data[(p.x + 1)*cols + p.y + 1] == label)
							p.x += 1, p.y += 1, type = 1;
						else if (data[(p.x + 1)*cols + p.y] == label)
							p.x += 1, type = 2;
						else if (data[(p.x + 1)*cols + p.y - 1] == label)
							p.x += 1, p.y -= 1, type = 3;
						else 
							type = 5;
					}break;														
					case 2: {
						if (data[p.x*cols + p.y + 1] == label)
							p.y += 1, type = 8;
						else if (data[(p.x + 1)*cols + p.y + 1] == label)
							p.x += 1, p.y += 1, type = 1;
						else if (data[(p.x + 1)*cols + p.y] == label)
							p.x += 1, type = 2;
						else if (data[(p.x + 1)*cols + p.y - 1] == label)
							p.x += 1, p.y -= 1, type = 3;
						else if (data[p.x*cols + p.y - 1] == label)
							p.y -= 1, type = 4;
						else
							type = 6;
					}break;
					case 3: {
						if (data[(p.x + 1)*cols + p.y + 1] == label)
							p.x += 1, p.y += 1, type = 1;
						else if (data[(p.x + 1)*cols + p.y] == label)
							p.x += 1, type = 2;
						else if (data[(p.x + 1)*cols + p.y - 1] == label)
							p.x += 1, p.y -= 1, type = 3;
						else if (data[p.x*cols + p.y - 1] == label)
							p.y -= 1, type = 4;
						else if (data[(p.x - 1)*cols + p.y - 1] == label)
							p.x -= 1, p.y -= 1, type = 5;
						else
							type = 7;
					}break;
					case 4: {
						if (data[(p.x + 1)*cols + p.y] == label)
							p.x += 1, type = 2;
						else if (data[(p.x + 1)*cols + p.y - 1] == label)
							p.x += 1, p.y -= 1, type = 3;
						else if (data[p.x*cols + p.y - 1] == label)
							p.y -= 1, type = 4;
						else if (data[(p.x - 1)*cols + p.y - 1] == label)
							p.x -= 1, p.y -= 1, type = 5;
						else if (data[(p.x - 1)*cols + p.y] == label)
							p.x -= 1, type = 6;
						else
							type = 8;
					}break;
					case 5: {
						if (data[(p.x + 1)*cols + p.y - 1] == label)
							p.x += 1, p.y -= 1, type = 3;
						else if (data[p.x*cols + p.y - 1] == label)
							p.y -= 1, type = 4;
						else if (data[(p.x - 1)*cols + p.y - 1] == label)
							p.x -= 1, p.y -= 1, type = 5;
						else if (data[(p.x - 1)*cols + p.y] == label)
							p.x -= 1, type = 6;
						else if (data[(p.x - 1)*cols + p.y + 1] == label)
							p.x -= 1, p.y += 1, type = 7;
						else
							type = 1;
					}break;
					case 6: {
						if (data[p.x*cols + p.y - 1] == label)
							p.y -= 1, type = 4;
						else if (data[(p.x - 1)*cols + p.y - 1] == label)
							p.x -= 1, p.y -= 1, type = 5;
						else if (data[(p.x - 1)*cols + p.y] == label)
							p.x -= 1, type = 6;
						else if (data[(p.x - 1)*cols + p.y + 1] == label)
							p.x -= 1, p.y += 1, type = 7;
						else if (data[p.x*cols + p.y + 1] == label)
							p.y += 1, type = 8;
						else
							type = 2;
					}break;
					case 7: {
						if (data[(p.x - 1)*cols + p.y - 1] == label)
							p.x -= 1, p.y -= 1, type = 5;
						else if (data[(p.x - 1)*cols + p.y] == label)
							p.x -= 1, type = 6;
						else if (data[(p.x - 1)*cols + p.y + 1] == label)
							p.x -= 1, p.y += 1, type = 7;
						else if (data[p.x*cols + p.y + 1] == label)
							p.y += 1, type = 8;
						else if (data[(p.x + 1)*cols + p.y + 1] == label)
							p.x += 1, p.y += 1, type = 1;
						else
							type = 3;
					}break;
					case 8: {
						if (data[(p.x - 1)*cols + p.y] == label)
							p.x -= 1, type = 6;
						else if (data[(p.x - 1)*cols + p.y + 1] == label)
							p.x -= 1, p.y += 1, type = 7;
						else if (data[p.x*cols + p.y + 1] == label)
							p.y += 1, type = 8;
						else if (data[(p.x + 1)*cols + p.y + 1] == label)
							p.x += 1, p.y += 1, type = 1;
						else if (data[(p.x + 1)*cols + p.y] == label)
							p.x += 1, type = 2;
						else
							type = 4;
					}break;
				}
				contour.push_back(p);
			}

			if (special)
			{
				p.x = i + 1;
				p.y = j - 1;
				type = 3;
				contour.push_back(p);
				while ((p.x != i) && (p.y != j))					
				{
					switch (type)
					{
					case 1: {
						if (data[(p.x - 1)*cols + p.y + 1] == label)			
							p.x -= 1, p.y += 1, type = 7;
						else if (data[p.x*cols + p.y + 1] == label)
							p.y += 1, type = 8;
						else if (data[(p.x + 1)*cols + p.y + 1] == label)
							p.x += 1, p.y += 1, type = 1;
						else if (data[(p.x + 1)*cols + p.y] == label)
							p.x += 1, type = 2;
						else if (data[(p.x + 1)*cols + p.y - 1] == label)
							p.x += 1, p.y -= 1, type = 3;
						else
							type = 5;
					}break;														
					case 2: {
						if (data[p.x*cols + p.y + 1] == label)
							p.y += 1, type = 8;
						else if (data[(p.x + 1)*cols + p.y + 1] == label)
							p.x += 1, p.y += 1, type = 1;
						else if (data[(p.x + 1)*cols + p.y] == label)
							p.x += 1, type = 2;
						else if (data[(p.x + 1)*cols + p.y - 1] == label)
							p.x += 1, p.y -= 1, type = 3;
						else if (data[p.x*cols + p.y - 1] == label)
							p.y -= 1, type = 4;
						else
							type = 6;
					}break;
					case 3: {
						if (data[(p.x + 1)*cols + p.y + 1] == label)
							p.x += 1, p.y += 1, type = 1;
						else if (data[(p.x + 1)*cols + p.y] == label)
							p.x += 1, type = 2;
						else if (data[(p.x + 1)*cols + p.y - 1] == label)
							p.x += 1, p.y -= 1, type = 3;
						else if (data[p.x*cols + p.y - 1] == label)
							p.y -= 1, type = 4;
						else if (data[(p.x - 1)*cols + p.y - 1] == label)
							p.x -= 1, p.y -= 1, type = 5;
						else
							type = 7;
					}break;
					case 4: {
						if (data[(p.x + 1)*cols + p.y] == label)
							p.x += 1, type = 2;
						else if (data[(p.x + 1)*cols + p.y - 1] == label)
							p.x += 1, p.y -= 1, type = 3;
						else if (data[p.x*cols + p.y - 1] == label)
							p.y -= 1, type = 4;
						else if (data[(p.x - 1)*cols + p.y - 1] == label)
							p.x -= 1, p.y -= 1, type = 5;
						else if (data[(p.x - 1)*cols + p.y] == label)
							p.x -= 1, type = 6;
						else
							type = 8;
					}break;
					case 5: {
						if (data[(p.x + 1)*cols + p.y - 1] == label)
							p.x += 1, p.y -= 1, type = 3;
						else if (data[p.x*cols + p.y - 1] == label)
							p.y -= 1, type = 4;
						else if (data[(p.x - 1)*cols + p.y - 1] == label)
							p.x -= 1, p.y -= 1, type = 5;
						else if (data[(p.x - 1)*cols + p.y] == label)
							p.x -= 1, type = 6;
						else if (data[(p.x - 1)*cols + p.y + 1] == label)
							p.x -= 1, p.y += 1, type = 7;
						else
							type = 1;
					}break;
					case 6: {
						if (data[p.x*cols + p.y - 1] == label)
							p.y -= 1, type = 4;
						else if (data[(p.x - 1)*cols + p.y - 1] == label)
							p.x -= 1, p.y -= 1, type = 5;
						else if (data[(p.x - 1)*cols + p.y] == label)
							p.x -= 1, type = 6;
						else if (data[(p.x - 1)*cols + p.y + 1] == label)
							p.x -= 1, p.y += 1, type = 7;
						else if (data[p.x*cols + p.y + 1] == label)
							p.y += 1, type = 8;
						else
							type = 2;
					}break;
					case 7: {
						if (data[(p.x - 1)*cols + p.y - 1] == label)
							p.x -= 1, p.y -= 1, type = 5;
						else if (data[(p.x - 1)*cols + p.y] == label)
							p.x -= 1, type = 6;
						else if (data[(p.x - 1)*cols + p.y + 1] == label)
							p.x -= 1, p.y += 1, type = 7;
						else if (data[p.x*cols + p.y + 1] == label)
							p.y += 1, type = 8;
						else if (data[(p.x + 1)*cols + p.y + 1] == label)
							p.x += 1, p.y += 1, type = 1;
						else
							type = 3;
					}break;
					case 8: {
						if (data[(p.x - 1)*cols + p.y] == label)
							p.x -= 1, type = 6;
						else if (data[(p.x - 1)*cols + p.y + 1] == label)
							p.x -= 1, p.y += 1, type = 7;
						else if (data[p.x*cols + p.y + 1] == label)
							p.y += 1, type = 8;
						else if (data[(p.x + 1)*cols + p.y + 1] == label)
							p.x += 1, p.y += 1, type = 1;
						else if (data[(p.x + 1)*cols + p.y] == label)
							p.x += 1, type = 2;
						else
							type = 4;
					}break;
					}
					contour.push_back(p);
				}
				special = 0;
			}

			contours.push_back(contour);

			
			flag[label] = 1;
		}
	}

	return MY_OK;
}

int ustc_ConnectedComponentLabeling(
	Mat grayImg,
	Mat& labelImg,
	int bin_th)
{
	if (NULL == grayImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}
	if (grayImg.channels() != 1)
	{
		cout << "image is bgr not gray." << endl;
		return MY_FAIL;
	}
	if (NULL == labelImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}
	if (labelImg.channels() != 1)
	{
		cout << "image is bgr not gray." << endl;
		return MY_FAIL;
	}
	if (grayImg.rows != labelImg.rows || grayImg.cols != labelImg.cols)
	{
		cout << "not match" << endl;
		return MY_FAIL;
	}
	int height = grayImg.rows;
	int width = grayImg.cols;
	int t[10000];
	int min_tbx;
	int min_bx;
	int m = 0;
	int bin[256];
	for (int i = 0; i <= 255; i++)
	{
		if (i < bin_th)
		{
			bin[i] = 0;
		}
		else
		{
			bin[i] = 255;
		}
	}
	int index1 = 0;
	index1 = width*height - 1;
	for (int index = 0; index <= index1; index++)
	{
		grayImg.data[index] = bin[grayImg.data[index]];
	}

	index1 = width - 1;
	for (int index = 0; index <= index1; index++)
	{
		labelImg.data[index] = 0;
	}
	index1 = grayImg.cols*grayImg.rows - 1;
	for (int index = index1 - width + 1; index <= index1; index++)
	{
		labelImg.data[index] = 0;
	}
	for (int index = 0; index <= height - 1; index++)
	{
		labelImg.data[index*width] = 0;
	}
	for (int index = 0; index <= height - 1; index++)
	{
		labelImg.data[index*width + width - 1] = 0;
	}
	for (int row = 1; row < height - 1; row++)
	{
		for (int col = 1; col < width - 1; col++)
		{
			int rcw = row*width + col;
			int b1 = labelImg.data[rcw - width - 1];
			int b2 = labelImg.data[rcw - width];
			int b3 = labelImg.data[rcw - width + 1];
			int b4 = labelImg.data[rcw - 1];
			int b = grayImg.data[rcw];
			if (b == 0)
			{
				labelImg.data[rcw] = 0;
				continue;
			}
			int casen = (!(!b1)) * 8 + (!(!b2)) * 4 + (!(!b3)) * 2 + (!(!b4));
			switch (casen)
			{
			case 0:
				m++;
				labelImg.data[rcw] = m;
				t[m] = m;
				continue;
				break;
			case 1:
				labelImg.data[rcw] = b4;
				break;
			case 2:
				labelImg.data[rcw] = b3;
				break;
			case 4:
				labelImg.data[rcw] = b2;
				break;
			case 8:
				labelImg.data[rcw] = b1;
				break;
			case 3:
				labelImg.data[rcw] = min_ab(b4, b3);
				t[max_ab(b3, b4)] = t[min_ab(b3, b4)];
				break;
			case 5:
				labelImg.data[rcw] = min_ab(b4, b2);
				t[max_ab(b2, b4)] = t[min_ab(b2, b4)];
				break;
			case 6:
				labelImg.data[rcw] = min_ab(b2, b3);
				t[max_ab(b2, b3)] = t[min_ab(b3, b2)];
				break;
			case 9:
				labelImg.data[rcw] = min_ab(b1, b4);
				t[max_ab(b1, b4)] = t[min_ab(b1, b4)];
				break;
			case 10:
				labelImg.data[rcw] = min_ab(b1, b3);
				t[max_ab(b1, b3)] = t[min_ab(b1, b3)];
				break;
			case 12:
				labelImg.data[rcw] = min_ab(b2, b1);
				t[max_ab(b2, b1)] = t[min_ab(b1, b2)];
				break;
			case 7:
				min_bx = min_ab(b3, b4);
				min_bx = min_ab(b2, min_bx);
				t[b4] = t[min_bx];
				t[b3] = t[min_bx];
				t[b2] = t[min_bx];
				labelImg.data[rcw] = min_bx;
				break;
			case 11:
				min_bx = min_ab(b3, b4);
				min_bx = min_ab(b1, min_bx);
				t[b4] = t[min_bx];
				t[b3] = t[min_bx];
				t[b1] = t[min_bx];
				labelImg.data[rcw] = min_bx;
				break;
			case 13:
				min_bx = min_ab(b2, b4);
				min_bx = min_ab(b1, min_bx);
				t[b4] = t[min_bx];
				t[b1] = t[min_bx];
				t[b2] = t[min_bx];
				labelImg.data[rcw] = min_bx;
				break;
			case 14:
				min_bx = min_ab(b3, b2);
				min_bx = min_ab(b1, min_bx);
				t[b1] = t[min_bx];
				t[b3] = t[min_bx];
				t[b2] = t[min_bx];
				labelImg.data[rcw] = min_bx;
				break;
			case 15:
				min_bx = min_ab(b3, b4);
				min_bx = min_ab(b2, min_bx);
				min_bx = min_ab(b1, min_bx);
				t[b1] = t[min_bx];
				t[b3] = t[min_bx];
				t[b2] = t[min_bx];
				t[b4] = t[min_bx];
				labelImg.data[rcw] = min_bx;
				break;
			}


		}

	}
	int num = 0;
	int n[10000];
	for (int i = 1; i <= m; i++)
	{
		if (t[i] == i)
		{
			num++;
			n[i] = num;
		}
		else
		{
			t[i] = t[t[i]];
		}
	}
	for (int i = 1; i <= m; i++)
	{
		t[i] = n[t[i]];

	}
	n[0] = 0;
	t[0] = 0;
	index1 = labelImg.cols*labelImg.rows - 1;
	for (int index = 0; index <= index1; index++)
	{
		labelImg.data[index] = t[labelImg.data[index]];
	}

	return MY_OK;
}
