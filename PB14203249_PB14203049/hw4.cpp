#include "FindContours.h"

#define FOREGROUND_COLOR	255
#define BACKGROUND_COLOR	0

//#define IMG_SHOW
#define DEFINCE

//////////////////////////////////////////连通域检测////////////////////////////////////////////////

//功能说明：找到二值图像中所有的连通域，并且进行标记
//grayImg：用来进行连通域分析的目标图，8UC1
//labelImg：用来存储标记结果的图像，CV_32SC1，尺寸与grayImg一致
//bin_th：用来对灰度图进行二值化的阈值
//返回值：MY_OK或者MY_FAIL

//在周围加上一层
int ustc_Add_Frame(Mat Img, Mat& frameImg)
{
#ifdef DEFINCE
	if (NULL == Img.data || NULL == frameImg.data)
	{
		cout << "binaryImg is NULL." << endl;
		return MY_FAIL;
	}
#endif // DEFINCE

	int height = Img.rows;
	int width = Img.cols;

	uchar* frameImgData = frameImg.data;

	//四周增加一层边框
	int location1 = (height + 1) * (width + 2);
	for (int col_j = 0; col_j < width + 2; col_j++)
	{
		frameImgData[col_j] = BACKGROUND_COLOR;
		frameImgData[location1 + col_j] = BACKGROUND_COLOR;

	}

	for (int row_i = 0; row_i < height + 2; row_i++)
	{
		frameImgData[row_i * (width + 2)] = BACKGROUND_COLOR;
		frameImgData[row_i * (width + 2) + width + 1] = BACKGROUND_COLOR;
	}

	//赋值内部
	uchar* ImgData = Img.data;
	for (int row_i = 0; row_i < height; row_i++)
	{
		int loca = row_i * width;
		for (int col_j = 0; col_j < width; col_j++)
		{
			frameImgData[loca + 2 * row_i + width + col_j + 3] = ImgData[loca + col_j];//*FOREGROUND_COLOR
		}
	}

#ifdef IMG_SHOW
	namedWindow("frameImg", 0);
	imshow("frameImg", frameImg);
	waitKey(0);
#endif // IMG_SHOW
	return MY_OK;
}

//连续的图像进行拼接，每次遇到周围有两种不同的色块的时候都把所有大的label置为0
int ustc_ChangeLabel(int* labelImgDataInt, int connectSmall, int connectBig, int location, int connectBig_max)
{
	//connectBig_max 添加这个变量以大幅度减少运行时间 不用每次都遍历完全图 只要把所有等于connectBig位置都替换就可以
	//这个想法挺好的 程序的执行时间少了很多很多
	int connectBig_num = 0;
	for (int loca = location; (loca >= 0) && (connectBig_num < connectBig_max); loca--)
	{
		if (labelImgDataInt[loca] == connectBig)
		{
			labelImgDataInt[loca] = connectSmall;
			connectBig_num++;
		}
	}
	return MY_OK;
}

//选择相邻四个中,小的label进行赋值，并调用图像拼接
int ustc_ChooseSmallLable_ChangeLabel(int* labelImgDataInt, int label_1, int label_2, int location, int* label_count)
{
	if (label_1 == label_2)
	{
		labelImgDataInt[location] = label_1;
		label_count[label_1]++;
	}
	else if (label_1 < label_2)
	{
		labelImgDataInt[location] = label_1;
		ustc_ChangeLabel(labelImgDataInt, label_1, label_2, location, label_count[label_2]);
		label_count[label_1] += label_count[label_2] + 1;
		label_count[label_2] = 0;
	}
	else
	{
		labelImgDataInt[location] = label_2;
		ustc_ChangeLabel(labelImgDataInt, label_2, label_1, location, label_count[label_1]);
		label_count[label_2] += label_count[label_1] + 1;
		label_count[label_1] = 0;
	}
	return MY_OK;
}

//连通域
int ustc_ConnectedComponentLabeling(
	Mat binaryImg,
	Mat& labelImg,
	int* labelCount)
{
#ifdef DEFINCE
	if (NULL == binaryImg.data || NULL == labelImg.data)
	{
		cout << "binaryImg or labelImg is NULL." << endl;
		return MY_FAIL;
	}
	if (int(binaryImg.channels()) != 1 || int(labelImg.channels()) != 1)
	{
		cout << "binaryImg or labelImg is not channel 1" << endl;
		return MY_FAIL;
	}

#endif // DEFINCE
	int width_gray = binaryImg.cols;
	int height_gray = binaryImg.rows;

	int width = width_gray + 2;
	int height = height_gray + 2;

	Mat newlabelImg(height, width, CV_32SC1);
	newlabelImg.setTo(0);

	Mat frame_binaryImg(height, width, CV_8UC1);
	//增加一层边框
	ustc_Add_Frame(binaryImg, frame_binaryImg);

	int label_num = 1;

	int* labelImgDataInt = ((int*)(newlabelImg.data));
	uchar* frame_binaryImgData = frame_binaryImg.data;

	int label_count[30000] = { 0 };


	for (int row_i = 1; row_i < height; row_i++)
	{
		for (int col_j = 1; col_j < width; col_j++)
		{
			int location = row_i * width + col_j;
			
			if (FOREGROUND_COLOR == frame_binaryImgData[location])
			{
				
				//四位01序列：左上.上.右上.左
				int situation =
					(frame_binaryImgData[location - 1]
					+ 10 * frame_binaryImgData[location - width + 1]
					+ 100 * frame_binaryImgData[location - width]
					+ 1000 * frame_binaryImgData[location - width - 1]) / FOREGROUND_COLOR;
				switch (situation)
				{
					//周围没被标记
				case 0:
					labelImgDataInt[location] = label_num;
					label_count[label_num]++;
					label_num++;
					break;
					//周围有一个被标记
				case 1:
					labelImgDataInt[location] = labelImgDataInt[location - 1];
					label_count[labelImgDataInt[location - 1]]++;
					break;
				case 10:
					labelImgDataInt[location] = labelImgDataInt[location - width + 1];
					label_count[labelImgDataInt[location - width + 1]]++;
					break;
				case 100:
					labelImgDataInt[location] = labelImgDataInt[location - width];
					label_count[labelImgDataInt[location - width]]++;
					break;
				case 1000:
					labelImgDataInt[location] = labelImgDataInt[location - width - 1];
					label_count[labelImgDataInt[location - width - 1]]++;
					break;
					//周围有两个被标记
				case 11:
					ustc_ChooseSmallLable_ChangeLabel(labelImgDataInt, labelImgDataInt[location - width + 1], labelImgDataInt[location - 1], location, label_count);
					break;
				case 101:
					labelImgDataInt[location] = labelImgDataInt[location - 1];
					label_count[labelImgDataInt[location - 1]]++;
					break;
				case 110:
					labelImgDataInt[location] = labelImgDataInt[location - width];
					label_count[labelImgDataInt[location - width]]++;
					break;
				case 1001:
					labelImgDataInt[location] = labelImgDataInt[location - 1];
					label_count[labelImgDataInt[location - 1]]++;
					break;
				case 1010:
					ustc_ChooseSmallLable_ChangeLabel(labelImgDataInt, labelImgDataInt[location - width - 1], labelImgDataInt[location - width + 1], location, label_count);
					break;
				case 1100:
					labelImgDataInt[location] = labelImgDataInt[location - width];
					label_count[labelImgDataInt[location - width]]++;
					break;
					//周围有三个被标记
				case 111:
					labelImgDataInt[location] = labelImgDataInt[location - 1];
					label_count[labelImgDataInt[location - 1]]++;
					break;
				case 1011:
					ustc_ChooseSmallLable_ChangeLabel(labelImgDataInt, labelImgDataInt[location - 1], labelImgDataInt[location - width + 1], location, label_count);
					break;
				case 1101:
					labelImgDataInt[location] = labelImgDataInt[location - 1];
					label_count[labelImgDataInt[location - 1]]++;
					break;
				case 1110:
					labelImgDataInt[location] = labelImgDataInt[location - width];
					label_count[labelImgDataInt[location - width]]++;
					break;
					//周围四个都被标记
				case 1111:
					labelImgDataInt[location] = labelImgDataInt[location - 1];
					label_count[labelImgDataInt[location - 1]]++;
					break;
				default:
					cout << "situation is wrong!" << endl;
					cout << "situation is " << situation << endl;
					cout << "row_i " << row_i << "\tcol_j " << col_j << endl;
					getchar();
					break;
				}
			}
		}
	}
	
	//把内部定义的newlabelImg赋值给外部的labelImg
	for (int row_i = 0; row_i < height_gray; row_i++)
	{
		for (int col_j = 0; col_j < width_gray; col_j++)
		{
			int location_label = row_i * width_gray + col_j;
			int location_label_new = (row_i + 1) * (width_gray + 2) + col_j + 1;
			((int*)(labelImg.data))[location_label] = labelImgDataInt[location_label_new];
		}
	}

	//把内部定义的label_count赋值给外部的labelCount
	for (int i = 0; i < 30000; i++)
	{
		labelCount[i] = label_count[i];
	}
	return MY_OK;
}

// 0  1  2
// 7  *  3
// 6  5  4
//根据当前位置和下标找到下一个位置
int find_location_with_index(int index, int location, int width)
{
	switch (index)
	{
		case 0:
			return location - width - 1;
		case 1:
			return location - width;
		case 2:
			return location - width + 1;
		case 3:
			return location + 1;
		case 4:
			return location + width + 1;
		case 5:
			return location + width;
		case 6:
			return location + width - 1;
		case 7:
			return location - 1;
	}
}

//找到下一个连通域的边缘位置相对于现在的pt_location的下标
int find_next_index(int* labelImgData, int curr_label, int pt_location, int previous_pt_index, int width, int max_location)
{
	int next_pt_index = (previous_pt_index + 1) % 8;

	while (next_pt_index != previous_pt_index)
	{
		int next_pt_location = find_location_with_index(next_pt_index, pt_location, width);
		if (next_pt_location >= 0 && next_pt_location < max_location)
		{
			if (labelImgData[next_pt_location] == curr_label)
			{
				return next_pt_index;
			}
		}
		next_pt_index = (next_pt_index + 1) % 8;
	}
	return next_pt_index;
}


//把当前label从label图中移除（置为0）
void ustc_RemoveLabel(int* labelImgDataInt, int curr_label, int max_label_num, int location)
{
	while (max_label_num > 0)
	{
		if (labelImgDataInt[location] == curr_label)
		{
			labelImgDataInt[location] = 0;
			max_label_num--;
		}
		location++;
	}
}

//寻找连通域边界
int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours)
{
#ifdef DEFINCE
	if (NULL == binaryImg.data || binaryImg.channels() != 1)
	{
		cout << "binaryImg is Null or channel is not one." << endl;
		return MY_FAIL;
	}
#endif // DEFINCE

	int width = binaryImg.cols;
	int height = binaryImg.rows;

	Mat labelImg(height, width, CV_32SC1);
	int labelCount[30000];

	//生成标记图，同时返回一个记录某标记的数量的数组，用来提升性能
	time_t start = clock();
	ustc_ConnectedComponentLabeling(binaryImg, labelImg, labelCount);
	time_t end = clock();
	cout << "ConnectedComponentLabeling  time = " << (end - start) / 1000.0 << endl;
	
	test_IMG_SHOW_labelImg(labelImg);

	contours.clear();

	int curr_label = 0;
	int* labelImgDataInt = ((int*)(labelImg.data));
	int max_location = width * height;

	for (int row_i = 0; row_i < height; row_i++)
	{
		int loca = row_i * width;
		for (int col_j = 0; col_j < width; col_j++)
		{
			int location = loca + col_j;
			int curr_label = labelImgDataInt[location];
			//是前景色则进入
			if (curr_label != 0)
			{
				vector < cv::Point >  curr_contour;
				int first_pt_location = location;
				int local_pt_location = first_pt_location;
				int next_pt_location;
				//意味着一开始从0号位遍历
				int previous_pt_index = 7; 

				//当前点插入
				cv::Point first_pt;
				first_pt.x = col_j;
				first_pt.y = row_i;
				curr_contour.push_back(first_pt);

				//如果当前label只有一个点，重新插入该点，结束遍历
				if (labelCount[curr_label] == 1)
				{
					curr_contour.push_back(first_pt);
				}
				else
				{
					///用来检验是否中途会回到初始点，若回到初始点，检验次下标和回到初始点的下一个下标是否相等，若相等认为结束
					int first_next_pt_index = find_next_index(labelImgDataInt, curr_label, local_pt_location, previous_pt_index, width, max_location);

					while (true)
					{
						cv::Point next_pt;
						int next_pt_index = find_next_index(labelImgDataInt, curr_label, local_pt_location, previous_pt_index, width, max_location);
						next_pt_location = find_location_with_index(next_pt_index, local_pt_location, width);
						next_pt.x = next_pt_location % width;
						next_pt.y = next_pt_location / width;

						//连通域碰到边上则停止
						if (next_pt.x == 0 || next_pt.x == width || next_pt.y == 0 || next_pt.y == height)
						{							
							break;
						}
						else
						{
							local_pt_location = next_pt_location;
							previous_pt_index = (next_pt_index + 4) % 8;
							curr_contour.push_back(next_pt);
						}
						if (next_pt_location == first_pt_location)
						{
							///用来检验是否中途会回到初始点，若回到初始点，检验次下标和回到初始点的下一个下标是否相等，若相等认为结束
							next_pt_index = find_next_index(labelImgDataInt, curr_label, local_pt_location, previous_pt_index, width, max_location);
							if (next_pt_index == first_next_pt_index)
							{
								break;
							}		
						}
					}
				}				
				contours.push_back(curr_contour);

				ustc_RemoveLabel(labelImgDataInt, curr_label, labelCount[curr_label], location);
			}
		}
	}

	return MY_OK;
}
