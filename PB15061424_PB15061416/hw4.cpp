#include "FindContours.h"
#define VO 255

void ustc_ConnectedComponentLabeling(
	Mat binaryImg,
	Mat& labelImg);

int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours)
{
	if (NULL == binaryImg.data)
	{
		printf("binaryImg is NULL!\n");
		return MY_FAIL;
	}

	Mat label_img;
	ustc_ConnectedComponentLabeling(binaryImg, label_img);

	int height = label_img.rows;
	int width = label_img.cols;
	int* data = (int*)label_img.data;
	int* label_value = (int*)malloc(height * width * sizeof(int)); 
	int* p_label_value = label_value;
	int* p_pre_value = label_value; 
	int label_num = 0;

	
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (data[y * width + x])
			{
				*p_label_value = data[y * width + x];
				
				while (p_pre_value != p_label_value)
				{
					if (*p_pre_value == *p_label_value)
					{
						*p_label_value = 0;
						p_label_value--;
						label_num--;
						break;
					}
					p_pre_value++;
				}
				p_pre_value = label_value;
				p_label_value++;
				label_num++;
			}
		}
	}
	p_label_value = label_value;

	Mat label_img_pro;
	copyMakeBorder(label_img, label_img_pro, 1, 1, 1, 1, BORDER_CONSTANT, 0);
	label_img_pro.convertTo(label_img_pro, CV_32SC1);
	int height_pro = label_img_pro.rows;
	int width_pro = label_img_pro.cols;
	int* data_pro = (int*)label_img_pro.data;

	
	int x_offset[8] = { 1,1,0,-1,-1,-1,0,1 };
	int y_offset[8] = { 0,-1,-1,-1,0,1,1,1 };
	
	vector<Point> single_outline;
	int num = 0;
	int current_label; 
	int rest_label_num = label_num;	
	if (label_num > 0)
	{
		current_label = *p_label_value;
		rest_label_num--;
	}

	for (int y = 1; y < height_pro - 1; y++)
	{
		for (int x = 1; x < width_pro - 1; x++)
		{
			if (data_pro[y * width_pro + x] == current_label)
			{
				int x_index = x;
				int y_index = y;
				int next_x;
				int next_y;
				single_outline.push_back(Point(x_index - 1, y_index - 1));
				int t = 0;
				int location = 0;
				int i;

				while (1)
				{
					location = (location + 6) % 8;
					for (i = 0; i < 7; i++)
					{
						next_x = x_index + x_offset[location];
						next_y = y_index + y_offset[location];

						if (data_pro[next_y * width_pro + next_x] == current_label)
						{
							x_index = next_x;
							y_index = next_y;
							single_outline.push_back(Point(x_index - 1, y_index - 1));
							break;
						}
						location = (location + 1) % 8;
					}

					
					if (i == 7)
					{
						t++;
						single_outline.push_back(Point(x_index - 1, y_index - 1));
						if (t == 1)
						{
							x_index = x;
							y_index = y;
							location = 4;
							continue;
						}
						else
						{
							contours.push_back(single_outline);
							num++;
							if (rest_label_num > 0)
							{
								single_outline.clear();
								p_label_value++;
								current_label = *p_label_value;
								rest_label_num--;
								t = 0;
								break;
							}
							
							else
							{
								return MY_OK;
							}
						}
					}

					
					if (x_index == x && y_index == y)
					{
						contours.push_back(single_outline);
						num++;
						
						if (rest_label_num > 0)
						{
							single_outline.clear();
							p_label_value++;
							current_label = *p_label_value;
							rest_label_num--;
							break;
						}
						
						else
						{
							return MY_OK;
						}
					}
				}
			}
		}
	}
	
	return MY_OK;
}

void ustc_ConnectedComponentLabeling(
	Mat binaryImg,
	Mat& labelImg)
{
	int Height = binaryImg.rows;
	int Width = binaryImg.cols;
	labelImg = Mat(Height, Width, CV_32SC1, Scalar(0));
	
	binaryImg.convertTo(binaryImg, CV_32SC1, 1, 0);

	//image pretreatment: add border
	Mat Img_Pro;
	copyMakeBorder(binaryImg, Img_Pro, 1, 1, 1, 1, BORDER_CONSTANT, 0);
	int Height_Pro = Img_Pro.rows;
	int Width_Pro = Img_Pro.cols;
	int* Ptr_Pro = (int*)Img_Pro.data;

	//first scan
	int* Label_Ptr = (int*)labelImg.data;
	int m = 1;//label_index
	int* Label_Temp = (int*)malloc(Height*Width * sizeof(int));
	int* next = (int*)malloc(Height*Width * sizeof(int));
	int* tail = (int*)malloc(Height*Width * sizeof(int));
	int u = 0, v = 0, i = 0;

	for (int Y = 1; Y < Height_Pro - 1; Y++) {
		for (int X = 1; X < Width_Pro - 1; X++) {
			int Curr_Label_Index = Y*Width_Pro + X;
			if (Ptr_Pro[Curr_Label_Index] == VO)
			{

				int c1 = Ptr_Pro[Y *Width_Pro + X - 1];
				int c2 = Ptr_Pro[(Y - 1) *Width_Pro + X - 1];
				int c3 = Ptr_Pro[(Y - 1) *Width_Pro + X];
				int c4 = Ptr_Pro[(Y - 1) *Width_Pro + X + 1];


				if (c3) {
					Ptr_Pro[Curr_Label_Index] = c3;//Ptr_Pro[(Y - 1)*Width_Pro + X];//=c3
				}
				else if (c1) {
					Ptr_Pro[Curr_Label_Index] = c1;//Ptr_Pro[Y*Width_Pro + X - 1];//=c1
					if (c4) {
						//resolve(c1,c4)
						if (Label_Temp[c1] < Label_Temp[c4])
						{
							u = Label_Temp[c1];
							v = Label_Temp[c4];
						}
						else if (Label_Temp[c1] > Label_Temp[c4])
						{
							u = Label_Temp[c4];
							v = Label_Temp[c1];
						}
						if (Label_Temp[c1] != Label_Temp[c4])
						{
							i = v;
							while (i != -1)
							{
								Label_Temp[i] = u;
								i = next[i];
							}
							next[tail[u]] = v;
							tail[u] = tail[v];
						}
					}
				}
				else if (c2) {
					Ptr_Pro[Curr_Label_Index] = c2;//Ptr_Pro[(Y - 1)*Width_Pro + X - 1];//=c2
					if (c4) {
						//resolve(c4,c2)
						if (Label_Temp[c2] < Label_Temp[c4])
						{
							u = Label_Temp[c2];
							v = Label_Temp[c4];
						}
						else if (Label_Temp[c2] > Label_Temp[c4])
						{
							u = Label_Temp[c4];
							v = Label_Temp[c2];
						}
						if (Label_Temp[c2] != Label_Temp[c4])
						{
							i = v;
							while (i != -1)
							{
								Label_Temp[i] = u;
								i = next[i];
							}
							next[tail[u]] = v;
							tail[u] = tail[v];
						}
					}
				}
				else if (c4) {
					Ptr_Pro[Curr_Label_Index] = c4;//Ptr_Pro[(Y - 1)*Width_Pro + X + 1];//=c4
				}
				else {
					Ptr_Pro[Curr_Label_Index] = m;
					Label_Temp[m] = m;
					next[m] = -1;
					tail[m] = m;
					m++;
				}

			}
		}
	}
	labelImg = Img_Pro(Range(1, Height + 1), Range(1, Width + 1)).clone();
	Label_Ptr = (int*)labelImg.data;

	//second scan
	for (int Label_Y = 0; Label_Y < Height; Label_Y++) {
		for (int Label_X = 0; Label_X < Width; Label_X++) {
			if (Label_Ptr[Label_Y*Width + Label_X] != 0) {
				Label_Ptr[Label_Y*Width + Label_X] = Label_Temp[Label_Ptr[Label_Y*Width + Label_X]];
			}
		}

	}

}
