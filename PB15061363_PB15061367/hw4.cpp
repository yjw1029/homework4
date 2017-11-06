#include "FindContours.h"
int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours) 
{
	if (!binaryImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}
	if (binaryImg.channels() != 1)
	{
		cout << "channels wrong!" << endl;
		return MY_FAIL;
	}
	if (binaryImg.depth() != CV_8U)
	{
		cout << "depth wrong!" << endl;
		return MY_FAIL;
	}
	int m = 1;   //labelImg的标签的值
	int width = binaryImg.cols, height = binaryImg.rows;
	int u, v, j;
	int T[10000] = { 0 };    //Table数组
	int Next[10000] = { 0 }, tail[10000] = { 0 };
	Mat labelImg(height, width, CV_32SC1);
	labelImg.setTo(0);
	//c4=b(x+1,y-1),c3=b(x,y-1),c2=b(x-1,y-1),c1=b(x-1,y)
	//****************
	//** c2  c3  c4 **
	//** c1   x     **
	//****************

	//First scan
	((int*)labelImg.data)[0] = (binaryImg.data[0] != 0);
	for (int x = 1; x < width; x++)
	{
		if (binaryImg.data[x])
		{
			int mode = (binaryImg.data[x - 1] != 0);
			switch (mode)
			{
			case 0:
				((int*)labelImg.data)[x] = m;
				T[m] = m;
				Next[m] = -1;
				tail[m] = m;
				m++;
				break;
			case 1:
				((int*)labelImg.data)[x] = ((int*)labelImg.data)[x - 1];
				break;
			}
		}
	}

	for (int y = 1; y < height; y++)
	{
		int temp = y*width;
		if (binaryImg.data[temp])
		{
			int mode0 = 2 * (binaryImg.data[temp - width + 1] != 0) + (binaryImg.data[temp - width] != 0);
			switch (mode0)
			{
			case 0:
				((int*)labelImg.data)[temp] = m;
				T[m] = m;
				Next[m] = -1;
				tail[m] = m;
				m++;
				break;
			case 1:
				((int*)labelImg.data)[temp] = ((int*)labelImg.data)[temp - width];
				break;
			case 2:
			case 3:
				((int*)labelImg.data)[temp] = ((int*)labelImg.data)[temp - width + 1];
				break;
			}
		}
		for (int x = 1; x < width - 1; x++)
		{
			if (binaryImg.data[temp + x])
			{
				//计算mode,mode=8*c4+4*c3+2*c2+c1
				int index = temp + x;
				int mode = 8 * (binaryImg.data[index - width + 1] != 0)
					+ 4 * (binaryImg.data[index - width] != 0)
					+ 2 * (binaryImg.data[index - width - 1] != 0)
					+ (binaryImg.data[index - 1] != 0);

				switch (mode)
				{
				case 0:
					((int*)labelImg.data)[index] = m;
					T[m] = m;
					Next[m] = -1;
					tail[m] = m;
					m++;
					break;
				case 8:
				case 12:
				case 13:
				case 14:
				case 15:
					//取c4
					((int*)labelImg.data)[index] = ((int*)labelImg.data)[index - width + 1];
					break;
				case 4:
				case 5:
				case 6:
				case 7:
					//取c3
					((int*)labelImg.data)[index] = ((int*)labelImg.data)[index - width];
					break;
				case 2:
				case 3:
					//取c2
					((int*)labelImg.data)[index] = ((int*)labelImg.data)[index - width - 1];
					break;
				case 1:
					//取c1
					((int*)labelImg.data)[index] = ((int*)labelImg.data)[index - 1];
					break;

				case 9:
					//取c4
					((int*)labelImg.data)[index] = ((int*)labelImg.data)[index - width + 1];
					u = T[((int*)labelImg.data)[index - width + 1]], v = T[((int*)labelImg.data)[index - 1]];
					if (u < v)
					{
						j = v;
						while (j != -1)
						{
							T[j] = u;
							j = Next[j];
						}
						Next[tail[u]] = v;
						tail[u] = tail[v];
					}
					else if (u > v)
					{
						j = u;
						while (j != -1)
						{
							T[j] = v;
							j = Next[j];
						}
						Next[tail[v]] = u;
						tail[v] = tail[u];
					}
					break;
				case 10:
					//取c4
					((int*)labelImg.data)[index] = ((int*)labelImg.data)[index - width + 1];
					u = T[((int*)labelImg.data)[index - width + 1]], v = T[((int*)labelImg.data)[index - width - 1]];
					if (u < v)
					{
						j = v;
						while (j != -1)
						{
							T[j] = u;
							j = Next[j];
						}
						Next[tail[u]] = v;
						tail[u] = tail[v];
					}
					else if (u > v)
					{
						j = u;
						while (j != -1)
						{
							T[j] = v;
							j = Next[j];
						}
						Next[tail[v]] = u;
						tail[v] = tail[u];
					}
					break;
				case 11:
					//取c4
					((int*)labelImg.data)[index] = ((int*)labelImg.data)[index - width + 1];
					u = T[((int*)labelImg.data)[index - width + 1]], v = T[((int*)labelImg.data)[index - 1]];
					if (u < v)
					{
						j = v;
						while (j != -1)
						{
							T[j] = u;
							j = Next[j];
						}
						Next[tail[u]] = v;
						tail[u] = tail[v];
					}
					else if (u > v)
					{
						j = u;
						while (j != -1)
						{
							T[j] = v;
							j = Next[j];
						}
						Next[tail[v]] = u;
						tail[v] = tail[u];
					}
					break;

				}
			}
		}
		temp = (y + 1)*width - 1;
		if (binaryImg.data[temp])
		{
			int mode1 = 4 * (binaryImg.data[temp - width] != 0) + 2 * (binaryImg.data[temp - width - 1] != 0) + (binaryImg.data[temp - 1] != 0);
			switch (mode1)
			{
			case 0:
				((int*)labelImg.data)[temp] = m;
				T[m] = m;
				Next[m] = -1;
				tail[m] = m;
				m++;
				break;
			case 1:
				((int*)labelImg.data)[temp] = ((int*)labelImg.data)[temp - 1];
				break;
			case 2:
			case 3:
				((int*)labelImg.data)[temp] = ((int*)labelImg.data)[temp - width - 1];
				break;
			case 4:
			case 5:
			case 6:
			case 7:
				((int*)labelImg.data)[temp] = ((int*)labelImg.data)[temp - width];
				break;
			}
		}
	}

	//Second scan
	int xx;
	for (int y = 0; y < height; y++)
	{
		for (xx = 0; xx < width - 7; xx += 8)
		{
			int index = y*width + xx;
			((int*)labelImg.data)[index] = T[((int*)labelImg.data)[index]];
			((int*)labelImg.data)[index + 1] = T[((int*)labelImg.data)[index + 1]];
			((int*)labelImg.data)[index + 2] = T[((int*)labelImg.data)[index + 2]];
			((int*)labelImg.data)[index + 3] = T[((int*)labelImg.data)[index + 3]];
			((int*)labelImg.data)[index + 4] = T[((int*)labelImg.data)[index + 4]];
			((int*)labelImg.data)[index + 5] = T[((int*)labelImg.data)[index + 5]];
			((int*)labelImg.data)[index + 6] = T[((int*)labelImg.data)[index + 6]];
			((int*)labelImg.data)[index + 7] = T[((int*)labelImg.data)[index + 7]];
		}
		for (xx -= 8; xx< width; xx++)
		{
			((int*)labelImg.data)[y*width + xx] = T[((int*)labelImg.data)[y*width + xx]];
		}
	}

	//这些都是之前的程序
	int largeheight = height + 2;
	int largewidth = width + 2;
	Mat large(largeheight, largewidth, CV_32SC1);

	copyMakeBorder(labelImg, large, 1, 1, 1, 1, BORDER_CONSTANT, 0);
	//   3 2 1
	//   4 P 0
	//   5 6 7
	//int count = 0;
	int biaozhi[10000] = { 0 };
	biaozhi[0] = 1;
	contours.clear();
	int contour_num = 0;
	for (int y = 1; y < largeheight - 1; y++)
	{
		for (int x = 1; x < largewidth - 1; x++)
		{
			if (!biaozhi[((int*)large.data)[y*largewidth + x]])    //一个轮廓
			{
				int x0 = x, y0 = y;
				int xqian = x, yqian = y;
				vector < cv::Point > contour;       //新建轮廓
				contour.push_back(cv::Point(xqian - 1, yqian - 1));//存入contour x-1,y-1
				int biaoqian = ((int*)large.data)[y*largewidth + x];
				biaozhi[biaoqian] = 1;
				int weizhi = 6;
				int k = 1;
				if (((int*)large.data)[y*largewidth + x + 1] == 0 && ((int*)large.data)[(y + 1)*largewidth + x + 1] == 0 && ((int*)large.data)[(y + 1)*largewidth + x] == 0 && ((int*)large.data)[(y + 1)*largewidth + x - 1] == 0 && ((int*)large.data)[y*largewidth + x - 1] == 0 && ((int*)large.data)[(y - 1)*largewidth + x - 1] == 0 && ((int*)large.data)[(y - 1)*largewidth + x] == 0 && ((int*)large.data)[(y - 1)*largewidth + x + 1] == 0)
				{
					k = 0;     //只有一点直接跳出
				}
				while (k)
				{
					switch (weizhi)
					{
					case 6://从右边开始
						if (((int*)large.data)[yqian*largewidth + xqian + 1] == biaoqian)
						{
							weizhi = 0; xqian += 1;
							contour.push_back(Point(xqian - 1, yqian - 1));//存入contour x-1,y-1					
							k = !(x0 == xqian&&y0 == yqian);    //回到原点k归零
						}
						else
						{
							weizhi = 5;
						}
						break;
					case 5://右下角开始
						if (((int*)large.data)[(yqian + 1)*largewidth + xqian + 1] == biaoqian)
						{
							weizhi = 7; xqian += 1; yqian += 1;
							contour.push_back(Point(xqian - 1, yqian - 1));//存入contour x-1,y-1
							k = !(x0 == xqian&&y0 == yqian);
						}
						else
						{
							weizhi = 4;
						}
						break;
					case 4://下边开始
						if (((int*)large.data)[(yqian + 1)*largewidth + xqian] == biaoqian)
						{
							weizhi = 6; yqian += 1;
							contour.push_back(Point(xqian - 1, yqian - 1));//存入contour x-1,y-1
							k = !(x0 == xqian&&y0 == yqian);
						}
						else
						{
							weizhi = 3;
						}
						break;
					case 3:
						if (((int*)large.data)[(yqian + 1)*largewidth + xqian - 1] == biaoqian)
						{
							weizhi = 5; xqian -= 1; yqian += 1;
							contour.push_back(Point(xqian - 1, yqian - 1));//存入contour x-1,y-1
							k = !(x0 == xqian&&y0 == yqian);
						}
						else
						{
							weizhi = 2;
						}
						break;
					case 2:
						if (((int*)large.data)[yqian*largewidth + xqian - 1] == biaoqian)
						{
							weizhi = 4; xqian -= 1;
							contour.push_back(Point(xqian - 1, yqian - 1));//存入contour x-1,y-1
							k = !(x0 == xqian&&y0 == yqian);
						}
						else
						{
							weizhi = 1;
						}
						break;
					case 1:
						if (((int*)large.data)[(yqian - 1)*largewidth + xqian - 1] == biaoqian)
						{
							weizhi = 3; xqian -= 1; yqian -= 1;
							contour.push_back(Point(xqian - 1, yqian - 1));//存入contour x-1,y-1
							k = !(x0 == xqian&&y0 == yqian);
						}
						else
						{
							weizhi = 0;
						}
						break;
					case 0:
						if (((int*)large.data)[(yqian - 1)*largewidth + xqian] == biaoqian)
						{
							weizhi = 2;  yqian -= 1;
							contour.push_back(Point(xqian - 1, yqian - 1));//存入contour x-1,y-1
							k = !(x0 == xqian&&y0 == yqian);
						}
						else
						{
							weizhi = 7;
						}
						break;
					case 7:
						if (((int*)large.data)[(yqian - 1)*largewidth + xqian + 1] == biaoqian)
						{
							weizhi = 1; xqian += 1; yqian -= 1;
							contour.push_back(Point(xqian - 1, yqian - 1));//存入contour x-1,y-1
							k = !(x0 == xqian&&y0 == yqian);
						}
						else
						{
							weizhi = 6;
						}
						break;
					}//end of switch
				}//end of while
				contours.push_back(contour);      //整个轮廓存入轮廓集
			}//end of if
		}//end of x
	}//end of y
	return MY_OK;
}
