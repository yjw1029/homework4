#include "FindContours.h"
int T[100000] = { 0 };
//全局变量防止被释放空间///
Mat labelImgplus;

//////////递归函数///////////
void Insert(int big, int small)
{
	if (T[big] == small)return;
	while (T[big] > small) {
		if (T[big] == big) {
			T[big] = small;
			return;
		}
		big = T[big];
	}
	int next = T[big];
	T[big] = small;
	Insert(small, next);
}

int ustc_ConnectedComponentLabeling(
	Mat grayImg,
	Mat& labelImg
)
{
	if (NULL == grayImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}

	if (grayImg.channels() != 1)
	{
		cout << "channels not match" << endl;
		return MY_FAIL;
	}
	///////create labelImg & binaryImg/////////////
	int height = grayImg.rows;
	int width = grayImg.cols;
	labelImg.create(height, width, CV_32SC1);
	labelImgplus.create(height + 2, width + 2, CV_32SC1);
	//////////////值赋Imgplus//////////////
	int heightPlus = 1 + height + 1;
	int widthPlus = 1 + width + 1;
	int *ImgPlus = (int*)labelImgplus.data;
	int total = (heightPlus - 1)*widthPlus;


	//////////////最外围赋值零//////////////
	for (int i = 0; i != widthPlus; i++)
	{
		ImgPlus[i] = 0;
		ImgPlus[total + i] = 0;
	}
	int widPlusminus1 = widthPlus - 1;
	int heiPlusminus1 = heightPlus - 1;
	for (int row = 1; row != heightPlus; row++)
	{
		int temp1 = (row)*widthPlus;
		ImgPlus[temp1] = 0;
		ImgPlus[temp1 + widPlusminus1] = 0;
	}

	//////////////first scan//////////////
	int m = 1;
	uchar *graydata = grayImg.data;
	int p, q, maxTpq;
	uchar* pr = graydata;
	for (int row = 1; row != heiPlusminus1; row++)
	{
		//e的行指针
		int* data_e = (int*)labelImgplus.ptr(row);
		//a的行指针
		int* data_a = (int*)labelImgplus.ptr(row - 1);
		for (int col = 1; col != widPlusminus1; col++)
		{
			// a b c
			// d e 
			//data[e] = bi_array[*(pr)];
			if (data_e[col] = *pr)
			{
				int d = data_e[col - 1];
				int b = data_a[col];
				int a = data_a[col - 1];
				int c = data_a[col + 1];
				int option = 8 * !(!a) + 4 * !(!b) + 2 * !(!c) + !(!d);

				switch (option)
				{
				case 0:
					data_e[col] = m;
					T[m] = m;
					m++;
					break;
				case 1:
					data_e[col] = T[d];
					break;
				case 2:
					data_e[col] = T[c];
					break;
				case 3:
					p = c;
					q = d;
					data_e[col] = T[p] > T[q] ? T[q] : T[p];
					maxTpq = T[p] > T[q] ? p : q;
					Insert(maxTpq, data_e[col]);
					break;
				case 4:
					data_e[col] = T[b];
					break;
				case 5:
					data_e[col] = T[d];
					break;
				case 6:
					data_e[col] = T[c];
					break;
				case 7:
					p = c;
					q = d;
					data_e[col] = T[p] > T[q] ? T[q] : T[p];
					maxTpq = T[p] > T[q] ? p : q;
					Insert(maxTpq, data_e[col]);
					break;
				case 8:
					data_e[col] = T[a];
					break;
				case 9:
					data_e[col] = T[d];
					break;
				case 10:
					p = a;
					q = c;
					data_e[col] = T[p] > T[q] ? T[q] : T[p];
					maxTpq = T[p] > T[q] ? p : q;
					Insert(maxTpq, data_e[col]);
					break;
				case 11:
					p = c;
					q = d;
					data_e[col] = T[p] > T[q] ? T[q] : T[p];
					maxTpq = T[p] > T[q] ? p : q;
					Insert(maxTpq, data_e[col]);
					break;
				case 12:
					data_e[col] = T[b];
					break;
				case 13:
					data_e[col] = T[d];
					break;
				case 14:
					data_e[col] = T[c];
					break;
				case 15:
					p = c;
					q = d;
					data_e[col] = T[p] > T[q] ? T[q] : T[p];
					maxTpq = T[p] > T[q] ? p : q;
					Insert(maxTpq, data_e[col]);
				}
			}
			pr++;
		}
	}

	//递归更新T表
	for (int beg = 1; beg != m; beg++)
	{
		int temp = beg;
		while (T[temp] != temp)
		{
			temp = T[temp];
		}
		T[beg] = temp;
	}

	for (int ii = 0; ii < heightPlus; ii++) {
		int *Mi = (int*)labelImgplus.ptr(ii);
		for (int jj = 0; jj < widthPlus; jj++) {
			Mi[jj] = T[Mi[jj]];
		}
	}
	labelImg = labelImgplus;
	return m;
}



int follow(vector<Point>&Points_label, Point Pfirst, Point Psecond, int *data, int height, int width)
{
	Point Ptemp;
	Point P_before = Pfirst, P_now = Psecond;
	int label = data[Pfirst.x*width + Pfirst.y];
	int t = 0;//记录终结点个数
	while (P_now != Pfirst)
	{
		//| -9  |  -8   | -7
		//| -1  |  now  | 1
		//| 7   |  8    | 9
		int deta_x = P_now.x - P_before.x;
		int deta_y = P_now.y - P_before.y;//得到deta_x和deta_y，即可得到当前点second在first中的位置
		int condition = 8 * deta_x + deta_y;
		int flag = 0;//标记是否有next点，有则为1
		switch (condition)
		{
			//0位置
		case 1:
		{
			int deta_x_order[7] = { 1,1,0,-1,-1,-1,1 };
			int deta_y_order[7] = { 0,1,1,1,0,-1,-1 };
			int next_row;
			int next_col;
			for (int i = 0; i < 7; i++)
			{
				next_row = P_now.x + deta_x_order[i];
				next_col = P_now.y + deta_y_order[i];
				if (data[next_row*width + next_col] == label)
				{
					Ptemp.x = next_row;
					Ptemp.y = next_col;
					flag = 1;
					break;
				}
			}
			break;
		}
		//1位置
		case -7:
		{
			int deta_x_order[7] = { 1,0,-1,-1,-1,0,1 };
			int deta_y_order[7] = { 1,1,1,0,-1,-1,0 };
			int next_row;
			int next_col;
			for (int i = 0; i < 7; i++)
			{
				next_row = P_now.x + deta_x_order[i];
				next_col = P_now.y + deta_y_order[i];
				if (data[next_row*width + next_col] == label)
				{
					Ptemp.x = next_row;
					Ptemp.y = next_col;
					flag = 1;
					break;
				}
			}
			break;
		}
		//2位置
		case -8:
		{
			int deta_x_order[7] = { 0,-1,-1,-1,0 ,1,1 };
			int deta_y_order[7] = { 1,1,0,-1,-1,-1,1 };
			int next_row;
			int next_col;
			for (int i = 0; i < 7; i++)
			{
				next_row = P_now.x + deta_x_order[i];
				next_col = P_now.y + deta_y_order[i];
				if (data[next_row*width + next_col] == label)
				{
					Ptemp.x = next_row;
					Ptemp.y = next_col;
					flag = 1;
					break;
				}
			}
			break;
		}
		//三位置
		case -9:
		{
			int deta_x_order[7] = { -1,-1,-1,0,1,1,0 };
			int deta_y_order[7] = { 1,0,-1,-1,-1,0,1 };
			int next_row;
			int next_col;
			for (int i = 0; i < 7; i++)
			{
				next_row = P_now.x + deta_x_order[i];
				next_col = P_now.y + deta_y_order[i];
				if (data[next_row*width + next_col] == label)
				{
					Ptemp.x = next_row;
					Ptemp.y = next_col;
					flag = 1;
					break;
				}
			}
			break;
		}
		//四位置
		case -1:
		{
			int deta_x_order[7] = { -1,-1,0,1,1,1,-1 };
			int deta_y_order[7] = { 0,-1,-1,-1,0,1,1 };
			int next_row;
			int next_col;
			for (int i = 0; i < 7; i++)
			{
				next_row = P_now.x + deta_x_order[i];
				next_col = P_now.y + deta_y_order[i];
				if (data[next_row*width + next_col] == label)
				{
					Ptemp.x = next_row;
					Ptemp.y = next_col;
					flag = 1;
					break;
				}
			}
			break;
		}
		//五位置
		case 7:
		{
			int deta_x_order[7] = { -1,0,1,1,1,0,-1 };
			int deta_y_order[7] = { -1,-1,-1,0,1,0 };
			int next_row;
			int next_col;
			for (int i = 0; i < 7; i++)
			{
				next_row = P_now.x + deta_x_order[i];
				next_col = P_now.y + deta_y_order[i];
				if (data[next_row*width + next_col] == label)
				{
					Ptemp.x = next_row;
					Ptemp.y = next_col;
					flag = 1;
					break;
				}
			}
			break;
		}
		//六位置
		case 8:
		{
			int deta_x_order[7] = { 0,1,1,1,0,-1,-1 };
			int deta_y_order[7] = { -1,-1,0,1,1,1,-1 };
			int next_row;
			int next_col;
			for (int i = 0; i < 7; i++)
			{
				next_row = P_now.x + deta_x_order[i];
				next_col = P_now.y + deta_y_order[i];
				if (data[next_row*width + next_col] == label)
				{
					Ptemp.x = next_row;
					Ptemp.y = next_col;
					flag = 1;
					break;
				}
			}
			break;
		}
		//七位置 
		case 9:
		{
			int deta_x_order[7] = { 1,1,1,0,-1,-1,0 };
			int deta_y_order[7] = { -1,0,1,1,1,0,-1 };
			int next_row;
			int next_col;
			for (int i = 0; i < 7; i++)
			{
				next_row = P_now.x + deta_x_order[i];
				next_col = P_now.y + deta_y_order[i];
				if (data[next_row*width + next_col] == label)
				{
					Ptemp.x = next_row;
					Ptemp.y = next_col;
					flag = 1;
					break;
				}
			}
			break;
		}
		}

		if (flag)
		{
			//如果有next点
			P_before = P_now;
			P_now = Ptemp;
			Points_label.push_back(P_now);
			continue;
		}


		if (!flag)
		{
			t += 1;
			if (t == 2) return MY_OK;
			P_before = Pfirst;
			//按n=4找下个点
			int flxg = 0;//标记是否有next点，有则为1
			int deta_x_order[5] = { -1,-1,0,1,1 };
			int deta_y_order[5] = { 0,-1,-1,-1,0 };
			for (int i = 0; i < 5; i++)
			{
				int next_row = P_before.x + deta_x_order[i];
				int next_col = P_before.y + deta_y_order[i];
				if (data[next_row*width + next_col] == label) {
					P_now.x = next_row;
					P_now.y = next_col;
					Points_label.push_back(P_now);
					flxg = 1;
					break;
				}
			}
			if (!flxg)//如果没有，说明已经遍历完此label
			{
				return MY_OK;
			}
		}
	}
	return MY_OK;
}



int USTC_Find_Contours(
	Mat binaryImg,
	vector < vector < cv::Point >>& contours)
{
	//做好标记
	int width = binaryImg.cols;
	int height = binaryImg.rows;
	int widthplus = width + 2;
	int heightplus = height + 2;
	contours.clear();
	//Mat labelImg(height, width, CV_32SC1);
	//防御
	if (NULL == binaryImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}

	if (binaryImg.channels() != 1)
	{
		cout << "channels not match" << endl;
		return MY_FAIL;
	}



	Mat labelImgplus(height + 2, width + 2, CV_32SC1);
	int max_T_num = ustc_ConnectedComponentLabeling(binaryImg, labelImgplus);

	int max_label = 0;
	for (int i = 1; i < max_T_num; i++)
	{
		if (T[i] > max_label) max_label = T[i];
	}


	int* Judge = new int[max_label + 1];//数组保存该标记是否已经找过
	memset(Judge, 0, sizeof(int)*(max_label + 1));


	Judge[0] = 1;//为了最内层循环少判断，直接认为0点是已经跑过的点
	int* data = (int*)labelImgplus.data;

	int heightplusminus = heightplus - 1;
	int widthplusminus = widthplus - 1;
	for (int row = 1; row < heightplusminus; row++)
	{
		int temp = row*widthplus;
		for (int col = 1; col < widthplusminus; col++)
		{
			if (Judge[data[temp + col]]) continue;//该标记已经跑过，或者为背景，continue
			int label_now = data[temp + col];//当前要找的边界的label值
			Judge[label_now] = 1;//说明已经标记过
			vector<Point> Points_label;//同一标记的点的集合
									   //从左到右，从上到下找第一个的next点等于预期的label,记为Pfirst
									   //然后在P的first八领域找到一个标签纸也为label的，记为Psecond,记录Psecond在Pfirst八领域中的位置为last_pos
									   //找到了就进入follow函数，完成follow跳出本次扫描，寻找下一个label值
			Point Pfirst(row, col);
			Points_label.push_back(Pfirst);//将Pfirst放入集合 
										   //| 9  |     8    |  7
										   //| 1  |   Pfirst | -1
										   //| -7 |    -8    | -9
										   //一开始对应0
			Point Psecond;
			int deta_x_order[5] = { 1,1,0,-1,-1 };
			int deta_y_order[5] = { 0,1,1,1,0 };
			int second_row;
			int second_col;
			for (int i = 0; i < 5; i++) {
				second_row = Pfirst.x + deta_x_order[i];
				second_col = Pfirst.y + deta_y_order[i];
				if (data[second_row*widthplus + second_col] == label_now)
				{
					Psecond.x = second_row;
					Psecond.y = second_col;
					Points_label.push_back(Psecond);
					follow(Points_label, Pfirst, Psecond, data, heightplus, widthplus);
					contours.push_back(Points_label);
					break;
				}
			}
		}
	}
	for (auto beg = contours.begin(); beg != contours.end(); beg++)
	{
		vector<cv::Point> Points = *beg;
		for (auto Pbeg = Points.begin(); Pbeg != Points.end(); Pbeg++)
		{
			Pbeg->x = Pbeg->x - 1;
			Pbeg->y = Pbeg->y - 1;
		}
	}
	return MY_OK;
}
