#include "FindContours.h"

int ustc_label2zero(Mat &labelImg, int label)
{
	if (labelImg.data == NULL)
		return MY_FAIL;
	if (label <= 0)
		return MY_FAIL;
	int width = labelImg.cols;
	int height = labelImg.rows;
	for (int row_i = 0; row_i < height; row_i++)
	{
		int temp = row_i*width;
		for (int col_j = 0; col_j < width; col_j++)
		{
			int pix = ((int*)labelImg.data)[col_j + temp];
			if (pix == label)
				((int*)labelImg.data)[col_j + temp] = 0;
		}
	}
	return MY_OK;
}

int update_T_label(int* T, int T_min, int label)
{
	if (T[label] > T_min&&T[label] != label)
	{
		int new_label = T[label];
		update_T_label(T, T_min, new_label);
		T[label] = T_min;
	}
	else
	{
		if (T[label] > T_min)
		{
			T[label] = T_min;
		}
	}
	return MY_OK;
}

int ustc_ConnectedComponentLabeling(
	Mat binaryImg_,
	Mat& labelImg)
{
	//判断图片是否为空
	if (NULL == binaryImg_.data || NULL == labelImg.data)
	{
		cout << "one of the IMG is NULL" << endl;
		return MY_FAIL;
	}
	int width = binaryImg_.cols;
	int height = binaryImg_.rows;
	Mat binaryImg(height, width, CV_32SC1);
	//判断灰度图和连通域标记图是否大小一致
	if (labelImg.cols != width || labelImg.rows != height)
	{
		cout << "the size of two IMGs not Match" << endl;
	}
	threshold(binaryImg_, binaryImg, 100, 1, THRESH_BINARY);

	int width_ex = width + 2;
	int height_ex = height + 2;
	//将二值化图像向外延拓长度为1的一圈得到拓展图
	Mat expand_binary(height_ex, width_ex, CV_8UC1);
	expand_binary.setTo(0);//二值化拓展图初始化清零
						   //将二值化小图粘贴到二值化拓展图中央
	Mat Img_ROI = expand_binary(Rect(1, 1, width, height));
	binaryImg.copyTo(Img_ROI);

#ifdef IMG_SHOW
	namedWindow("expand_binary", 0);
	imshow("expand_binary", expand_binary);
	waitKey();
#endif

	//分配一个足够的空间给T数组，用于储存各标号之间的联通关系
	int* T = NULL;
	T = new int[width*height / 4 + 1];
	memset(T, 0, (width*height / 4 + 1) * sizeof(int));//T数组清零
	Mat labelImg_ex(height_ex, width_ex, CV_32SC1);
	labelImg_ex.setTo(0);//连通域拓展图初始化清零

						 //在拓展图中扫描的结束坐标
	int end_x = width + 1;
	int end_y = height + 1;

	//当前最大的标号值
	int current_max_label = 0;

	for (int row_y = 1; row_y < end_y; row_y++)
	{
		//label_ex图上bcd位置的width*height
		int bcd_temp = (row_y - 1)*width_ex;

		for (int col_x = 1; col_x < end_x; col_x++)
		{
			//计算扫描器abcd在拓展图上的相对位置
			int c_pos = bcd_temp + col_x;
			int b_pos = c_pos - 1;
			int d_pos = c_pos + 1;
			int a_pos = b_pos + width_ex;
			int e_pos = a_pos + 1;

			//若当前扫描到的像素点为背景色，跳过该点
			if (0 == expand_binary.data[e_pos])
				continue;

			//按二值化的图片中扫描器上abcd点的像素值进行分类
			int a = expand_binary.data[a_pos];
			int b = expand_binary.data[b_pos];
			int c = expand_binary.data[c_pos];
			int d = expand_binary.data[d_pos];
			int type = (a << 3) + (b << 2) + (c << 1) + d;

			//记下连通域拓展图上当前扫描器abcd位置的标号
			int label_a = ((int*)labelImg_ex.data)[a_pos];
			int label_b = ((int*)labelImg_ex.data)[b_pos];
			int label_c = ((int*)labelImg_ex.data)[c_pos];
			int label_d = ((int*)labelImg_ex.data)[d_pos];

			//扫描器上的abcd的标号 所连通的的标号 中的最小值
			int T_min = 0;
			//扫描器上的abcd标号的最小值


			switch (type)//分情况讨论
			{
			case 0://孤立点
				current_max_label += 1;
				((int*)labelImg_ex.data)[e_pos] = current_max_label;
				T[current_max_label] = current_max_label;
				break;
				//1.标记非孤立点，将扫描点标记为扫描器上最小的标号，
				//2.T数组的更新，将T数组上扫描器上abcd当前非背景色的点的标号与其中最小的标号建立联系
			case 1://d的相邻点
				((int*)labelImg_ex.data)[e_pos] = T[label_d];
				break;
			case 2://c的相邻点
				((int*)labelImg_ex.data)[e_pos] = T[label_c];
				break;
			case 3://cd的相邻点
				   //比较与cd标号联系的标号
				T_min = T[label_c] < T[label_d] ? T[label_c] : T[label_d];
				//比较cd标号
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_c);
				update_T_label(T, T_min, label_d);
				//T[label_c] = T_min;
				//T[label_d] = T_min;
				break;
			case 4://b的相邻点
				((int*)labelImg_ex.data)[e_pos] = T[label_b];
				break;
			case 5://bd的相邻点
				   //比较与bd标号联系的标号
				T_min = T[label_b] < T[label_d] ? T[label_b] : T[label_d];
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_b);
				update_T_label(T, T_min, label_d);
				//T[label_b] = T_min;
				//T[label_d] = T_min;
				break;
			case 6:////bc的相邻点
				   //比较与bc标号联系的标号
				T_min = T[label_b] < T[label_c] ? T[label_b] : T[label_c];
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_b);
				update_T_label(T, T_min, label_c);
				//T[label_b] = T_min;
				//T[label_c] = T_min;
				break;
			case 7://bcd的相邻点
				   //比较与bcd标号联系的标号
				T_min = T[label_b] < T[label_c] ? T[label_b] : T[label_c];
				T_min = T_min < T[label_d] ? T_min : T[label_d];
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_b);
				update_T_label(T, T_min, label_c);
				update_T_label(T, T_min, label_d);
				//T[label_b] = T_min;
				//T[label_c] = T_min;
				//T[label_d] = T_min;
				break;
			case 8://a的相邻点
				   //cout << label_a<<" " << e_pos-width_ex << endl;
				((int*)labelImg_ex.data)[e_pos] = T[label_a];
				break;
			case 9://ad的相邻点
				   //比较与ad标号联系的标号
				T_min = T[label_a] < T[label_d] ? T[label_a] : T[label_d];
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_a);
				update_T_label(T, T_min, label_d);
				//T[label_a] = T_min;
				//T[label_d] = T_min;
				break;
			case 10://ac的相邻点
					//比较与ac标号联系的标号
				T_min = T[label_a] < T[label_c] ? T[label_a] : T[label_c];
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_a);
				update_T_label(T, T_min, label_c);
				//T[label_a] = T_min;
				//T[label_c] = T_min;
				break;
			case 11://acd的相邻点
					//比较与acd标号联系的标号
				T_min = T[label_a] < T[label_c] ? T[label_a] : T[label_c];
				T_min = T_min < T[label_d] ? T_min : T[label_d];
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_a);
				update_T_label(T, T_min, label_c);
				update_T_label(T, T_min, label_d);
				//T[label_a] = T_min;
				//T[label_c] = T_min;
				//T[label_d] = T_min;
				break;
			case 12://ab的相邻点
					//比较与ab标号联系的标号
				T_min = T[label_a] < T[label_b] ? T[label_a] : T[label_b];
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_a);
				update_T_label(T, T_min, label_b);
				//T[label_a] = T_min;
				//T[label_b] = T_min;
				break;
			case 13://abd的相邻点
					//比较与abd标号联系的标号
				T_min = T[label_a] < T[label_b] ? T[label_a] : T[label_b];
				T_min = T_min < T[label_d] ? T_min : T[label_d];
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_a);
				update_T_label(T, T_min, label_b);
				update_T_label(T, T_min, label_d);
				//T[label_a] = T_min;
				//T[label_b] = T_min;
				//T[label_d] = T_min;
				break;
			case 14://abc的相邻点
					//比较与abc标号联系的标号
				T_min = T[label_a] < T[label_b] ? T[label_a] : T[label_b];
				T_min = T_min < T[label_c] ? T_min : T[label_c];
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_a);
				update_T_label(T, T_min, label_b);
				update_T_label(T, T_min, label_c);
				//T[label_a] = T_min;
				//T[label_b] = T_min;
				//T[label_c] = T_min;
				break;
			case 15://abcd的相邻点
					//比较与abcd标号联系的标号
				T_min = T[label_a] < T[label_b] ? T[label_a] : T[label_b];
				T_min = T_min < T[label_c] ? T_min : T[label_c];
				T_min = T_min < T[label_d] ? T_min : T[label_d];
				((int*)labelImg_ex.data)[e_pos] = T_min;
				update_T_label(T, T_min, label_a);
				update_T_label(T, T_min, label_b);
				update_T_label(T, T_min, label_c);
				update_T_label(T, T_min, label_d);
				//T[label_a] = T_min;
				//T[label_b] = T_min;
				//T[label_c] = T_min;
				//T[label_d] = T_min;
				break;
			}
		}
	}//end 1st ROW

	//T数组更新
	for (int i = 1; i <= current_max_label; i++)
	{
		if (T[i] == i)
		{
			continue;
		}

		while (T[i] != T[T[i]])
		{
			T[i] = T[T[i]];
		}
	}

	//第二次循环，将T数组中联通的标号统一变为较小的标号
	for (int row_y = 1; row_y < end_y; row_y++)
	{
		int temp = width_ex*row_y;
		for (int col_x = 1; col_x < end_x; col_x++)
		{
			int pos = temp + col_x;
			int label = ((int*)labelImg_ex.data)[pos];

			((int*)labelImg_ex.data)[pos] = T[label];
		}
	}//end the 2nd ROW

	 //结果返回实参
	labelImg_ex(Rect(1, 1, width, height)).copyTo(labelImg);
	delete[]T;
	return MY_OK;


}//end ConnectedComponentLabeling


int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours)
{

	//参数合法性检查
	if (NULL == binaryImg.data)
	{
		cout << "the labelImg is NULL" << endl;
		return MY_FAIL;
	}
	if (1 != binaryImg.channels())
	{
		cout << "the channels of labelImg ERROR" << endl;
		return MY_FAIL;
	}
	//清空边界序列
	contours.clear();

	int width = binaryImg.cols;
	int height = binaryImg.rows;
	Mat labelImg(height, width, CV_32SC1);

	ustc_ConnectedComponentLabeling(binaryImg, labelImg);
	//对图像进行延拓，避免图像边界的讨论
	int width_ex = width + 2;
	int height_ex = height + 2;
	Mat labelImg_ex(height_ex, width_ex, CV_32SC1);
	labelImg_ex.setTo(0);//大图初始清零
	Mat ROI = labelImg_ex(Rect(1, 1, width, height));//设置ROI
	labelImg.copyTo(ROI);
	//一个点周围8邻域打表
	Point type_8_point[8] = { { 1,0 },{ 1,-1 },{ 0,-1 },{ -1,-1 },{ -1,0 },{ -1,1 },{ 0,1 },{ 1,1 } };
	//根据上一个点到当前点的位置编码值的不同，扫描下一个点的位置编码值时只需要选取5个位置，打表
	int type_8_dir_5[8][5] =
	{
		{ 2,1,0,7,6 },
		{ 3,2,1,0,7 },
		{ 4,3,2,1,0 },
		{ 5,4,3,2,1 },
		{ 6,5,4,3,2 },
		{ 7,6,5,4,3 },
		{ 0,7,6,5,4 },
		{ 1,0,7,6,5 }
	};

	int end_y = 1 + height;//y循环结束结束条件
	int end_x = 1 + width;//x循环结束结束条件
	
	for (int row_y = 1; row_y < end_y; row_y++)
	{
		int y_mul_widthex = row_y*width_ex;
		for (int col_x = 1; col_x < end_x; col_x++)
		{

			int start_pos = y_mul_widthex + col_x;
			int curr_pos = start_pos;//当前点在拓展图上的位置
			int start_pt_pix = ((int*)labelImg_ex.data)[curr_pos];
			//初始点为背景像素
			if (0 == start_pt_pix)
			{
				continue;//跳过此点
			}

			vector<Point> temp_boundry;
			int dir = 0;//上一个点到下一个点的方向，用0~7的整数表示
			int curr_label = start_pt_pix;//当前连通域的标号
			Point start_pt = { col_x - 1,row_y - 1 };//扫描的初始点
			Point curr_pt = start_pt;//扫描的当前点

			temp_boundry.push_back(start_pt);//储存初始点

			//初始点不为背景像素
											

			//单独处理初始点,初始点的1,2,3,4位置像素值为0,可按照表中编码6的情况处理
			for (dir = 0; dir < 8; dir++)
			{
				int next_dir = type_8_dir_5[6][dir];//下一个点的位置编码值
				Point type_pt = type_8_point[next_dir];//按dir方向要加上的向量
				int dy = type_pt.y;//y增量
				int dx = type_pt.x;//x增量
				int next_pos = curr_pos + dy*width_ex + dx;
				if (((int*)labelImg_ex.data)[next_pos] != 0)
				{
					curr_pt = curr_pt + type_pt;//找到了候选点
					curr_pos = next_pos;//位置更新
					temp_boundry.push_back(curr_pt);
					dir = next_dir;
					break;
				}
			}
			if (8 == dir)//初始点是孤立点的情况
			{
				contours.push_back(temp_boundry);
				((int*)labelImg_ex.data)[curr_pos] = 0;//清零
				continue;
			}
			//为解决算法单像素宽度且初始点下方的点为背景，扫描返回初始点后，左下方的图像没扫描到就被清零的问题
			//可在采用如下方法
			//初始点正下方的点像素值为0，则将初始点的像素值赋给此点，扫描轮廓后再删掉

			Point exadd_pt = { 0,0 };
			int flag_addpt_or_not = 0;
			if (((int*)labelImg_ex.data)[start_pos + width_ex] == 0)
			{
				exadd_pt.x = start_pt.x;
				exadd_pt.y = start_pt.y + 1;
				((int*)labelImg_ex.data)[start_pos + width_ex] = curr_label;
				flag_addpt_or_not = 1;
			}

			//核心，非孤立点的轮廓检测
			while (curr_pt.x != start_pt.x || curr_pt.y != start_pt.y)
			{

				int i = 0;

				for (i = 0; i < 5; i++)
				{
					int next_dir = type_8_dir_5[dir][i];//下一个点的位置编码值
					Point type_pt = type_8_point[next_dir];//按找下一个点的位置编码值要加上的向量
					int dy = type_pt.y;//y增量
					int dx = type_pt.x;//x增量
					int next_pos = curr_pos + dy*width_ex + dx;
					if (((int*)labelImg_ex.data)[next_pos] != 0)
					{
						curr_pt = curr_pt + type_pt;//找到了候选点
						temp_boundry.push_back(curr_pt);//储存初始点
						curr_pos = next_pos;//位置更新
						dir = next_dir;//方向更新
						break;
					}
				}
				if (5 == i)//5临域没有下一个点
				{
					dir = (dir + 4) % 8;//取相反方向往回
					for (i = 0; i < 5; i++)
					{
						int next_dir = type_8_dir_5[dir][i];//下一个点的位置编码值
						Point type_pt = type_8_point[next_dir];//按找下一个点的位置编码值要加上的向量
						int dy = type_pt.y;//x增量
						int dx = type_pt.x;//y增量
						int next_pos = curr_pos + dy*width_ex + dx;
						if (((int*)labelImg_ex.data)[next_pos] != 0)
						{
							curr_pt = curr_pt + type_pt;//找到了候选点
							temp_boundry.push_back(curr_pt);//储存初始点
							curr_pos = next_pos;//位置更新
							dir = next_dir;//方向更新
							break;
						}
					}
				}
			}//end while

			if (flag_addpt_or_not == 1)
			{

				for (vector<Point>::iterator q = temp_boundry.begin(); q != temp_boundry.end(); q++)
				{
					if (*q == exadd_pt)
					{
						temp_boundry.erase(q);
						break;
					}
				}
			}
			contours.push_back(temp_boundry);
			ustc_label2zero(labelImg_ex, curr_label);
		}//end x_circle
	}//end y_circle
}
