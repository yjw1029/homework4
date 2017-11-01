#include "FindContours.h"
#define PIX_BACK 0 //背景像素

int FirstScan(Mat img, int *table)
{
	int &m = table[0];
	m = 1;
	for (int i = 1; i < img.rows - 1; i++)
	{
		for (int j = 1; j < img.cols - 1; j++)
		{
			if (img.ptr<int>(i)[j] != PIX_BACK)
			{
				int a = img.ptr<int>(i - 1)[j - 1];
				int b = img.ptr<int>(i - 1)[j];
				int c = img.ptr<int>(i - 1)[j + 1];
				int d = img.ptr<int>(i)[j - 1];
				int abcd = ((a != PIX_BACK) << 3) | ((b != PIX_BACK) << 2)
					| ((c != PIX_BACK) << 1) | (d != PIX_BACK);
				switch (abcd) //switch中的操作取自[1]Appendix A
				{
				case 0:
					img.ptr<int>(i)[j] = table[m] = m;
					m++;
					break;
				case 1:
					img.ptr<int>(i)[j] = table[d];
					break;
				case 2:
					img.ptr<int>(i)[j] = table[c];
					break;
				case 3:
					if (table[c] < table[d]) img.ptr<int>(i)[j] = table[d] = table[c];
					else img.ptr<int>(i)[j] = table[c] = table[d];
					break;
				case 4:
					img.ptr<int>(i)[j] = table[b];
					break;
				case 5:
					img.ptr<int>(i)[j] = table[d];
					break;
				case 6:
					img.ptr<int>(i)[j] = table[c];
					break;
				case 7:
					if (table[c] < table[d]) img.ptr<int>(i)[j] = table[d] = table[c];
					else img.ptr<int>(i)[j] = table[c] = table[d];
					break;
				case 8:
					img.ptr<int>(i)[j] = table[a];
					break;
				case 9:
					img.ptr<int>(i)[j] = table[d];
					break;
				case 10:
					if (table[a] < table[c]) img.ptr<int>(i)[j] = table[c] = table[a];
					else img.ptr<int>(i)[j] = table[a] = table[c];
					break;
				case 11:
					if (table[c] < table[d]) img.ptr<int>(i)[j] = table[d] = table[c];
					else img.ptr<int>(i)[j] = table[c] = table[d];
					break;
				case 12:
					img.ptr<int>(i)[j] = table[b];
					break;
				case 13:
					img.ptr<int>(i)[j] = table[d];
					break;
				case 14:
					img.ptr<int>(i)[j] = table[c];
					break;
				case 15:
					if (table[c] < table[d]) img.ptr<int>(i)[j] = table[d] = table[c];
					else img.ptr<int>(i)[j] = table[c] = table[d];
					break;
				default:
					break;
				}
			}
		}
	}
	return 0;
}

int LaterScan(Mat img, int *table, char direction)
{
	int &m = table[0];
	int unfinished = 0;
	for (int i = img.rows - 2; i > 0; i--)
	{
		for (int j = img.cols - 2; j > 0; j--)
		{
			if (img.ptr<int>(i)[j] != PIX_BACK)
			{
				int a, b, c, d;
				if (direction == 'f')
				{
					a = img.ptr<int>(i - 1)[j - 1];
					b = img.ptr<int>(i - 1)[j];
					c = img.ptr<int>(i - 1)[j + 1];
					d = img.ptr<int>(i)[j - 1];
				}
				else
				{
					a = img.ptr<int>(i + 1)[j + 1];
					b = img.ptr<int>(i + 1)[j];
					c = img.ptr<int>(i + 1)[j - 1];
					d = img.ptr<int>(i)[j + 1];
				}
				int e = img.ptr<int>(i)[j];
				int abcd = ((a != PIX_BACK) << 3) | ((b != PIX_BACK) << 2)
					| ((c != PIX_BACK) << 1) | (d != PIX_BACK);
				switch (abcd) //switch中的操作取自[1]Appendix A
				{
				case 0:
					img.ptr<int>(i)[j] = table[e];
					break;
				case 1:
					if (table[d] < table[e]) img.ptr<int>(i)[j] = table[e] = table[d];
					else img.ptr<int>(i)[j] = table[d] = table[e];
					break;
				case 2:
					if (table[c] < table[e])img.ptr<int>(i)[j] = table[e] = table[c];
					else img.ptr<int>(i)[j] = table[c] = table[e];
					break;
				case 3:
					if (table[c] < table[d] && table[c] < table[e])
						img.ptr<int>(i)[j] = table[d] = table[e] = table[c];
					else
					{
						if (table[d] < table[e])
							img.ptr<int>(i)[j] = table[c] = table[e] = table[d];
						else
							img.ptr<int>(i)[j] = table[c] = table[d] = table[e];
					}
					break;
				case 4:
					if (table[b] < table[e]) img.ptr<int>(i)[j] = table[e] = table[b];
					else img.ptr<int>(i)[j] = table[b] = table[e];
					break;
				case 5:
					if (table[d] < table[e]) img.ptr<int>(i)[j] = table[e] = table[d];
					else img.ptr<int>(i)[j] = table[d] = table[e];
					break;
				case 6:
					if (table[c] < table[e]) img.ptr<int>(i)[j] = table[e] = table[c];
					else img.ptr<int>(i)[j] = table[c] = table[e];
					break;
				case 7:
					if (table[c] < table[d] && table[c] < table[e])
						img.ptr<int>(i)[j] = table[d] = table[e] = table[c];
					else
					{
						if (table[d] < table[e])
							img.ptr<int>(i)[j] = table[c] = table[e] = table[d];
						else
							img.ptr<int>(i)[j] = table[c] = table[d] = table[e];
					}
					break;
				case 8:
					if (table[a] < table[e]) img.ptr<int>(i)[j] = table[e] = table[a];
					else img.ptr<int>(i)[j] = table[a] = table[e];
					break;
				case 9:
					if (table[d] < table[e]) img.ptr<int>(i)[j] = table[e] = table[d];
					else img.ptr<int>(i)[j] = table[d] = table[e];
					break;
				case 10:
					if (table[a] < table[c] && table[a] < table[e])
						img.ptr<int>(i)[j] = table[c] = table[e] = table[a];
					else
					{
						if (table[c] < table[e])
							img.ptr<int>(i)[j] = table[a] = table[e] = table[c];
						else
							img.ptr<int>(i)[j] = table[a] = table[c] = table[e];
					}
					break;
				case 11:
					if (table[c] < table[d] && table[c] < table[e])
						img.ptr<int>(i)[j] = table[d] = table[e] = table[c];
					else
					{
						if (table[d] < table[e])
							img.ptr<int>(i)[j] = table[c] = table[e] = table[d];
						else
							img.ptr<int>(i)[j] = table[c] = table[d] = table[e];
					}
					break;
				case 12:
					if (table[b] < table[e]) img.ptr<int>(i)[j] = table[e] = table[b];
					else img.ptr<int>(i)[j] = table[b] = table[e];
					break;
				case 13:
					if (table[d] < table[e]) img.ptr<int>(i)[j] = table[e] = table[d];
					else img.ptr<int>(i)[j] = table[d] = table[e];
					break;
				case 14:
					if (table[c] < table[e]) img.ptr<int>(i)[j] = table[e] = table[c];
					else img.ptr<int>(i)[j] = table[c] = table[e];
					break;
				case 15:
					if (table[c] < table[d] && table[c] < table[e])
						img.ptr<int>(i)[j] = table[d] = table[e] = table[c];
					else
					{
						if (table[d] < table[e])
							img.ptr<int>(i)[j] = table[c] = table[e] = table[d];
						else
							img.ptr<int>(i)[j] = table[c] = table[d] = table[e];
					}
					break;
				default:
					break;
				}
				if (e != img.ptr<int>(i)[j]) unfinished = 1;
			}
		}
	}
	return unfinished;
}

int ConnectComponents(Mat binaryImg, Mat &labelImg)
{
	if (!(binaryImg.data && binaryImg.depth() == CV_8U && binaryImg.channels() == 1
		&& labelImg.depth() == CV_32S && labelImg.channels() == 1
		&& binaryImg.size() == labelImg.size()))
		return MY_FAIL;
	Mat tempImg(binaryImg.rows + 2, binaryImg.cols + 2, CV_32SC1);
	binaryImg.convertTo(tempImg(Rect(1, 1, binaryImg.cols, binaryImg.rows)), CV_32S, -1.0, 0.0);
	tempImg.row(0).setTo(PIX_BACK);
	tempImg.row(tempImg.rows - 1).setTo(PIX_BACK);
	tempImg.col(0).setTo(PIX_BACK);
	tempImg.col(tempImg.cols - 1).setTo(PIX_BACK);
	int *table = new int[tempImg.rows * tempImg.cols / 4];
	//连通域的个数不可能超过tempImg.rows * tempImg.cols / 4
	if (table == NULL) return MY_FAIL;
	FirstScan(tempImg, table);
	int unfinished;
	char direction = 'b'; //第2次扫描为后向扫描
	do
	{
		unfinished = LaterScan(tempImg, table, direction);
		if (direction == 'b') direction = 'f';
		else direction = 'b';
		//改变扫描的方向
	} while (unfinished);
	delete[]table;
	tempImg(Rect(1, 1, labelImg.cols, labelImg.rows)).copyTo(labelImg);
	return MY_OK;
}

int FollowContour(Mat labelImg, int startX, int startY, vector<Point> &contour)
{
	if (!(labelImg.data && labelImg.depth() == CV_32S && labelImg.channels() == 1
		&& startX > 0 && startX < labelImg.cols - 1
		&& startY > 0 && startY < labelImg.rows - 1))
		return MY_FAIL;
	static int offsetX[8] = { -1,  0,  1,  1,  1,  0, -1, -1 };
	static int offsetY[8] = { -1, -1, -1,  0,  1,  1,  1,  0 };
	//从左上角开始，八邻域的编号顺时针依次为0, 1, 2, 3, 4, 5, 6, 7
	contour.clear();
	contour.push_back(Point(startX - 1, startY - 1));
	int i = 3;
	while (i < 7 && labelImg.ptr<int>(startY + offsetY[i])[startX + offsetX[i]] == PIX_BACK) i++;
	//依次检查(startX, startY)的3, 4, 5, 6号邻域，如果它们都为背景像素，则该连通域只有一个像素，否
	//则需要进行下面if语句中的跟踪
	if (i < 7)
	{
		int x = startX + offsetX[i];
		int y = startY + offsetY[i];
		int iLast = i; //iLast为当前像素相对于上一个像素的位置编号
		contour.push_back(Point(x - 1, y - 1));
		bool neighbor6 = (labelImg.ptr<int>(startY + offsetY[6])[startX + offsetX[6]] != PIX_BACK);
		do
		{
			i = (iLast + 6) & 7; //搜索的起点为j逆时针旋转90度的位置
			while (labelImg.ptr<int>(y + offsetY[i])[x + offsetX[i]] == PIX_BACK)
				i = (i + 1) & 7; //按顺时针搜索八邻域中的下一个像素，用 & 7 代替效率低下的 % 8 操作
			x = x + offsetX[i];
			y = y + offsetY[i];
			iLast = i;
			contour.push_back(Point(x - 1, y - 1));
			//由于跟踪前在图像周边围了一圈背景像素，实际的轮廓点坐标为(x - 1, y - 1)
		} while (x != startX || y != startY
			|| (x == startX && y == startY && iLast == 0 && neighbor6)
			|| (x == startX && y == startY && iLast == 7 && neighbor6));
	}
	return MY_OK;
}

int SetPixelTo0(Mat img, int pixel)
{
	if (!(img.data && img.depth() == CV_32S && img.channels() == 1)) return MY_FAIL;
	for (int i = 0; i < img.rows; i++)
	{
		int *ithRow = img.ptr<int>(i);
		for (int j = 0; j < img.cols; j++) ithRow[j] *= (ithRow[j] != pixel);
	}
	return MY_OK;
}

int USTC_Find_Contours(Mat binaryImg, vector<vector<Point>> &contours)
{
	if (!(binaryImg.data && binaryImg.depth() == CV_8U && binaryImg.channels() == 1)) return MY_FAIL;
	Mat labelImg(binaryImg.rows + 2, binaryImg.cols + 2, CV_32SC1);
	ConnectComponents(binaryImg, labelImg(Rect(1, 1, binaryImg.cols, binaryImg.rows)));
	labelImg.row(0).setTo(PIX_BACK);
	labelImg.row(labelImg.rows - 1).setTo(PIX_BACK);
	labelImg.col(0).setTo(PIX_BACK);
	labelImg.col(labelImg.cols - 1).setTo(PIX_BACK);
	//在图像周边围一圈背景像素
	contours.clear();
	for (int startY = 1; startY < labelImg.rows - 1; startY++)
	{
		for (int startX = 1; startX < labelImg.cols - 1; startX++)
		{
			if (labelImg.ptr<int>(startY)[startX] != PIX_BACK)
			{
				vector<Point> contour;
				FollowContour(labelImg, startX, startY, contour);
				contours.push_back(contour);
				SetPixelTo0
				(
					labelImg(Rect(1, startY, binaryImg.cols, labelImg.rows - startY - 1)),
					labelImg.ptr<int>(startY)[startX]
				); //跟踪完一个连通域的轮廓后，从图中删除该连通域
			}
		}
	}
	return MY_OK;
}
