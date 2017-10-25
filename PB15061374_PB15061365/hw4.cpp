#include "FindContours.h"

int ustc_ConnectedComponentLabeling(Mat grayImg, Mat& labelingImg, int bin_th)
{
	int Rows = grayImg.rows;
	int Cols = grayImg.cols;
	int col_big = Cols + 2;
	Mat binaryImg(Rows, Cols, CV_8UC1);
	Mat binaryImg_show(Rows, Cols, CV_8UC1);
	//the create a threhold img
	cv::threshold(grayImg, binaryImg, bin_th, 1, cv::THRESH_BINARY);
	labelingImg.setTo(0);
	if (binaryImg.data == NULL || labelingImg.data == NULL)
	{
		cout << "pic does not exist" << endl;
		return MY_FAIL;
	}
	if (Rows != labelingImg.rows || Cols != labelingImg.cols)
	{
		cout << "size error" << endl;
		return MY_FAIL;
	}
	Mat big_binaryImg(Rows + 1, Cols + 1, CV_8UC1);
	copyMakeBorder(binaryImg, big_binaryImg, 1, 1, 1, 1, 0);

	
	int scan_1dim[10000] = { 0 };
	int label = 1;
	////////////////////////////////////////////first scan
	for (int i = 0; i < Rows; i++)
	{
		for (int j = 0; j < Cols; j++)
		{
			
			if (big_binaryImg.data[(i + 1) *col_big + j+1])
			{
				int n;
				int max = 0;
				int min = 0;
				int a, b;

				n = (big_binaryImg.data[(i + 1)*col_big + j] <<3)
					+ (big_binaryImg.data[i*col_big + j]  <<2)
					+ (big_binaryImg.data[i*col_big + j+1]  <<1)
					+ big_binaryImg.data[i*col_big +j+2];
			
				switch (n)
				{
				case 0:
					((int*)labelingImg.data)[i*Cols + j] = label;
					scan_1dim[label]= label;
					label++;
					break;
				case 1:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[(i - 1)*Cols + j + 1];
					break;
				case 2:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[(i - 1)*Cols + j];
					break;
				case 3:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[(i - 1)*Cols + j];
					break;
				case 4:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[(i - 1)*Cols + j - 1];
					break;
				case 5:
					a = scan_1dim[((int*)labelingImg.data)[(i - 1)*Cols + j - 1]];
					b = scan_1dim[((int*)labelingImg.data)[(i - 1)*Cols + j + 1]];
					((int*)labelingImg.data)[i*Cols + j] = a;
					if (a == b) break;
					if (a < b)
					{
						max = b;
						min = a;
					}
					else
					{
						min = b;
						max =a;
					}

					for (int m = 1; m < label; m++)
					{
						if (scan_1dim[m] == max)
							scan_1dim[m] = min;
					}
					break;
				case 6:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[(i - 1)*Cols + j - 1];
					break;
				case 7:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[(i - 1)*Cols + j - 1];
					break;
				case 8:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[i*Cols + j - 1];
					break;
				case 9:
					a = scan_1dim[((int*)labelingImg.data)[i*Cols + j - 1]];
					b = scan_1dim[((int*)labelingImg.data)[(i - 1)*Cols + j + 1]];
					((int*)labelingImg.data)[i*Cols + j] = a;
					if (a == b) break;
					if (a<= b)
					{
						max = b;
						min = a;
					}
					else
					{
						min = b;
						max = a;
					}
					for (int m = 1; m < label; m++)
					{
						if (scan_1dim[m] == max)
							scan_1dim[m] = min;
					}
					break;
				case 10:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[i*Cols + j - 1];
					break;
				case 11:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[i*Cols + j - 1];
					break;
				case 12:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[i*Cols + j - 1];
					break;
				case 13:
					a = scan_1dim[((int*)labelingImg.data)[i*Cols + j - 1]];
					b = scan_1dim[((int*)labelingImg.data)[(i - 1)*Cols + j + 1]];
					((int*)labelingImg.data)[i*Cols + j] = a;
					if (a == b) break;
					if (a <= b)
					{
						max = b;
						min = a;
					}
					else
					{
						min = b;
						max = a;
					}
					for (int m = 1; m < label; m++)
					{
						if (scan_1dim[m] == max)
							scan_1dim[m] = min;
					}
					break;
				case 14:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[i*Cols + j - 1];
					break;
				case 15:
					((int*)labelingImg.data)[i*Cols + j] = ((int*)labelingImg.data)[i*Cols + j - 1];
					break;
				}
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////second scan
	for (int i = 0; i < Rows; i++)
	{
		for (int j = 0; j < Cols; j++)
		{
			if (((int*)labelingImg.data)[i*Cols + j])
			{
				((int*)labelingImg.data)[i*Cols + j] = scan_1dim[((int*)labelingImg.data)[i*Cols + j]];
			}
		}
	}
#ifdef IMG_SHOW1
	cv::threshold(grayImg, binaryImg_show, bin_th, 255, cv::THRESH_BINARY);
	Mat outimg_show(Rows, Cols, CV_8UC1);
	int max = 1;
	for (int i = 0; i < label;i++) {
		if (scan_1dim[i] > max) max = scan_1dim[i];
	}
	for(int i=0;i<Rows;i++)
		for (int j = 0; j < Cols; j++)
		{
			outimg_show.data[i*Cols + j] = ((int*)labelingImg.data)[i*Cols + j] * 255 / max;
		}
	imwrite("result.bmp", outimg_show);
	imshow("binary_IMG", binaryImg_show);
	imshow("outImg", outimg_show);
	waitKey();
#endif

return MY_OK;
}

int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours) {
	if (binaryImg.data == NULL )
	{
		cout << "pic does not exist" << endl;
		return MY_FAIL;
	}

	int Rows = binaryImg.rows;
	int Cols = binaryImg.cols;
	int Cols_big = Cols + 2;
	Mat labelingImg(Rows, Cols, CV_32SC1);
	contours.clear();

	ustc_ConnectedComponentLabeling(binaryImg, labelingImg, 128);
	int add[8] = { 1,Cols_big+1,Cols_big ,Cols_big -1,-1,-Cols_big - 1,-Cols_big,-Cols_big+1 };
	int addx[8] = { 1,1,0,-1,-1,-1,0,1 };
	int addy[8] = { 0,1,1,1,0,-1,-1,-1 };
	Mat big_labelingImg(Rows + 1, Cols + 1, CV_32SC1);
	copyMakeBorder(labelingImg, big_labelingImg, 1, 1, 1, 1, 0);
	int find_contours[10000] = { 0 };
	find_contours[0]=1;
	vector < cv::Point > a;
	for(int i=1;i<Rows+1;i++)
		for (int j = 1; j < Cols+1; j++) {
			int num = ((int*)big_labelingImg.data)[i*Cols_big + j];
			if (!find_contours[num]) {//找到未标记轮廓的连通域
				find_contours[num] = 1;
				a.clear();
				a.push_back(cv::Point(j-1, i-1));
				int ini_point = i*Cols_big + j;
				int search = 0;
	
				while (search<8 && ((int*)big_labelingImg.data)[ini_point + add[search]] != num) {
					//寻找第二个边界点
					search++;

				}
				if (search == 8) {
					contours.push_back(a);
					continue;
				}
				int Nx = j + addx[search];
				int Ny = i + addy[search];
				int Now_point = Ny*Cols_big + Nx;
				a.push_back(cv::Point(Nx-1, Ny-1));
				int previous_point = (search + 4) % 8;
		
				while (Now_point!= ini_point) {
					
					search = (previous_point + 2) % 8;
					while (((int*)big_labelingImg.data)[Now_point + add[search]] != num) {
						search=(search+1)%8;
					}
					
					Nx = Nx + addx[search];
				    Ny = Ny + addy[search];
					Now_point = Ny*Cols_big + Nx;
					previous_point = (search + 4) % 8;
					a.push_back(cv::Point(Nx-1, Ny-1));
				}
				contours.push_back(a);//找完一个连通域，将边界点集压入contours
			}
		}
#ifdef IMG_SHOW2
	Mat ShowImg(Rows, Cols, CV_8UC1);
	ShowImg.setTo(0);
	for(int i=0;i<contours.size();i++)
		for (int j = 0; j < contours[i].size(); j++) {
			ShowImg.data[contours[i][j].y*Cols + contours[i][j].x] = 255;
		}
	imshow("ShowImg", ShowImg);
	waitKey();
#endif
}
