#include"Find_Contours.h"

#define BIN_TH 120

//#define IMG_SHOW
//#define IMPROVE

////////////////////////////////Connected Detection///////////////////////////////////////////////

//图像二值化
int ustc_Threshold(
	Mat grayImg,
	Mat& binaryImg,
	int th)
{
	int height = grayImg.rows;
	int width = grayImg.cols;
	uchar* data = grayImg.data;
	uchar* Bdata = binaryImg.data;

	//生成比较数组
	uchar* COMP = new uchar[256];
	int i = 0;
	for (; i < th; i++)
	{
		COMP[i] = 0;
	}
	for (; i < 256; i++)
	{
		COMP[i] = 1;
	}

	//遍历图像
	for (int i_row = 0; i_row < height; i_row++)
	{
		int temp_width = width - 3;
		int j_col = 0;
		int mult = i_row * width;
		for (; j_col < temp_width; j_col += 4)
		{
			int indx = mult + j_col;
			Bdata[indx] = COMP[data[indx]];
			Bdata[indx + 1] = COMP[data[indx + 1]];
			Bdata[indx + 2] = COMP[data[indx + 2]];
			Bdata[indx + 3] = COMP[data[indx + 3]];
		}
		for (; j_col < width; j_col++)
		{
			int indx = mult + j_col;
			Bdata[indx] = COMP[data[indx]];
		}
	}

	return MY_OK;
}

////////////////////////////////////////by Lin Zhixian/////////////////////////////////////////////////////
int label_to_min(int* &label_connection_table, int label) {
	int label_tmp = label;
	int label_pre = label_connection_table[label];
	while (label_pre != 0) {
		label_tmp = label_pre;
		label_pre = label_connection_table[label_tmp];
	}
	return label_tmp;
}

// the value in curr_pos is already changed to min_new
void set_to_min(int* &label_connection_table, int curr_pos, int min_old, int min_new, int curr_max) {
	int i;
	//int min_new = label_connection_table[pos_start]; // if it hasn't changed yet
	for (i = curr_pos + 1; i <= curr_max; i++) {
		if (min_old == label_connection_table[i]) {
			label_connection_table[i] = min_new;
		}
	}
}

bool labelImg_update_round2(Mat labelImg, int* &label_connection_table) {
	int width = labelImg.cols;
	int height = labelImg.rows;
	int last_pixel = width * height - 1;
	int curr_pos = last_pixel;
	int label_final_tmp = 0;
	while (curr_pos >= 0) {
		label_final_tmp = label_connection_table[((int *)labelImg.data)[curr_pos]];
		if (label_final_tmp == 0) {
			label_final_tmp = ((int *)labelImg.data)[curr_pos];
		}
		((int *)labelImg.data)[curr_pos] = label_final_tmp;
		curr_pos--;
	}
	return true;
}


// case 0: increase_curr_max and set the pixel to label curr_max; no operation needed for label_connection_table


// after this function,  () is needed 
bool MergeLabels(int* &label_connection_table, int label_low, int label_high, int curr_max) {
	// no need to merge, becase they are the same. 
	if (label_low == label_high) {
		return false;
	}
	int label_tmp = 0;
	if (label_low > label_high) {
		label_tmp = label_high;
		label_high = label_low;
		label_low = label_tmp;
	}

	int label_low_min = label_to_min(label_connection_table, label_low);
	int label_old = label_connection_table[label_high];
	// no need to update
	if (label_old == label_low_min) {
		return false;
	}
	// labels following label_high may have connections with label_high, they should be updated to label_low_min too
	set_to_min(label_connection_table, label_high, label_high, label_low_min, curr_max);
	if (label_old == 0) {
		label_connection_table[label_high] = label_low_min;
	}
	else { // already has connection with another label, we should refresh the connection recursively 
		if (label_old > label_low_min) {
			label_connection_table[label_high] = label_low_min;
			label_connection_table[label_old] = label_low_min;
			// labels following label_high may have connections with label_old, they should be updated to label_low_min too
			set_to_min(label_connection_table, label_old, label_old, label_low_min, curr_max);
		}
		else { // label_old <= label_low_min
			label_connection_table[label_low_min] = label_old;
			// update previous label_low_min to label_old 
			set_to_min(label_connection_table, label_low_min, label_low_min, label_old, curr_max);
		}
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////

//连通域标记
int ustc_ConnectedComponentLabeling(
	Mat grayImg,        //grayImg：用来进行连通域分析的目标图，8UC1
	Mat& labelImg,      //labelImg：用来存储标记结果的图像，CV_32SC1，尺寸与grayImg一致
	int bin_th)         //bin_th：用来对灰度图进行二值化的阈值
{
	////////////////////////////参数防御///////////////////////////////////////
	//图片传入成功与否判定
	if (NULL == grayImg.data || NULL == labelImg.data)
	{
		std::cout << "ERROR: image is NULL" << std::endl;
		return MY_FAIL;
	}

	//阈值是否合适
	if (bin_th <= 0 || bin_th >= 255)
	{
		std::cout << "ERROR: th is abnormal." << std::endl;
		return MY_FAIL;
	}

	int height = grayImg.rows;
	int width = grayImg.cols;
	int channel = grayImg.channels();
	int Lchannel = labelImg.channels();

	//两幅图片大小是否一致
	if (height != labelImg.rows || width != labelImg.cols)
	{
		std::cout << "ERROR: the size of two image is not match" << std::endl;
		return MY_FAIL;
	}

	//通道数是否合法
	if (channel != 1 || Lchannel != 1)
	{
		std::cout << "ERROR: channel != 1" << std::endl;
		return MY_FAIL;
	}

	//////////////////////图像二值化///////////////////////////////////////////
	Mat binaryImg(height, width, CV_8UC1);
	if (MY_FAIL == ustc_Threshold(grayImg, binaryImg, bin_th))
	{
		std::cout << "ERROR: ustc_Threshold failed" << std::endl;
		return MY_FAIL;
	}

	//////////////////////二值图像扩充边缘/////////////////////////////////////
	Mat Bin_Img(height + 2, width + 2, CV_8UC1);
	Mat Label_Img(height + 2, width + 2, CV_32SC1);
	copyMakeBorder(binaryImg, Bin_Img, 1, 1, 1, 1, BORDER_CONSTANT, 0);
	copyMakeBorder(labelImg, Label_Img, 1, 1, 1, 1, BORDER_CONSTANT, 0);
	Label_Img.setTo(0);
	///////////////////////连通域查找第一次遍历////////////////////////////////
	int len = width * height / 4;
	int* Label = new int[len];
	memset(Label, 0, sizeof(int) * len);
	int curr_max = 0;

	int width_plus = width + 2;
	int height_plus = height + 2;
	for (int i_row = 1; i_row <= height; i_row++)
	{
		int row_offset = i_row * width_plus;
		for (int j_col = 1; j_col <= width; j_col++)
		{
			int indx = row_offset + j_col;
			if (Bin_Img.data[indx])
			{
				int c1 = Bin_Img.data[indx - 1];
				int c2 = Bin_Img.data[indx - 1 - width_plus];
				int c3 = Bin_Img.data[indx - width_plus];
				int c4 = Bin_Img.data[indx - width_plus + 1];
				int result = c1 + (c2 << 1) + (c3 << 2) + (c4 << 3);

				int label_low = 0;
				int label_high = 0;
				switch (result)
				{
				case 0:
					curr_max++;
					((int *)(Label_Img.data))[indx] = curr_max;
					break;
				case 1:
				case 3:
				case 5:
				case 7:
				case 13:
				case 15:
					((int *)(Label_Img.data))[indx] = ((int *)(Label_Img.data))[indx - 1];
					break;
				case 2:
				case 6:
				case 14:
					((int *)(Label_Img.data))[indx] = ((int *)(Label_Img.data))[indx - 1 - width_plus];
					break;
				case 12:
				case 4:
					((int *)(Label_Img.data))[indx] = ((int *)(Label_Img.data))[indx - width_plus];
					break;
				case 8:
					((int *)(Label_Img.data))[indx] = ((int *)(Label_Img.data))[indx - width_plus + 1];
					break;

				case 9:
					label_low = ((int *)(Label_Img.data))[indx - 1];
					label_high = ((int *)(Label_Img.data))[indx - width_plus + 1];
#ifdef COMPARE
					if (label_low > label_high)
					{
						int temp = label_low;
						label_low = label_high;
						label_high = temp;
					}
#endif   

					((int *)(Label_Img.data))[indx] = label_low;
					MergeLabels(Label, label_low, label_high, curr_max);
					break;
				case 10:
				case 11:
					label_low = ((int *)(Label_Img.data))[indx - width_plus - 1];
					label_high = ((int *)(Label_Img.data))[indx - width_plus + 1];
#ifdef COMPARE
					if (label_low > label_high)
					{
						int temp = label_low;
						label_low = label_high;
						label_high = temp;
					}
#endif   

					((int *)(Label_Img.data))[indx] = label_low;
					MergeLabels(Label, label_low, label_high, curr_max);
					break;
				}

			}
		}
	}

	////////////////////////去除label图像边缘//////////////////////////////////
	Mat curr_labelImg(height, width, CV_32SC1);
	curr_labelImg = Label_Img(Rect(1, 1, width, height)).clone();

	/////////////////////////第二次图像遍历////////////////////////////////////
	labelImg_update_round2(curr_labelImg, Label);

	labelImg = curr_labelImg;


#ifdef IMG_SHOW
	//连通域标记图
	Mat showImg(height, width, CV_8UC1);
	showImg.setTo(0);
	for (int i_row = 0; i_row < height; i_row++)
	{
		int row_offset = i_row * width;
		for (int j_col = 0; j_col < width; j_col++)
		{
			int indx = row_offset + j_col;
			int pixel = ((int *)curr_labelImg.data)[indx];
			if (pixel)
			{
				showImg.data[indx] = (pixel * 50) % 255;
			}
		}
	}

	//二值化原图
	Mat bin_grayImg(height, width, CV_8UC1);
	bin_grayImg.setTo(0);
	for (int i_row = 0; i_row < height; i_row++)
	{
		int row_offset = i_row * width;
		for (int j_col = 0; j_col < width; j_col++)
		{
			int indx = row_offset + j_col;
			int pixel = grayImg.data[indx];
			if (pixel >= bin_th)
			{
				bin_grayImg.data[indx] = 255;
			}
		}
	}
	//imwrite("F:/大四资料/软工/测试图片/grayImg/7-1.png", showImg);
	imwrite("F:/大四资料/软工/测试图片/grayImg/7-2.png", bin_grayImg);
	//cv::imshow("LabelImg", showImg);
	//cv::imshow("binaryImg", bin_grayImg);
	//cv::waitKey(0);
#endif //IMG_SHOW 

#ifdef DEBUG
	for (int i_row = 1; i_row < height; i_row++)
	{
		int row_offset = i_row * width;
		for (int j_col = 1; j_col < width; j_col++)
		{
			int indx = row_offset + j_col;
			int curr_label = ((int *)LabelImg.data)[indx];
			if (curr_label >= 40 && curr_label < 49)
			{
				cout << ((int *)LabelImg.data)[indx] << endl;
			}
		}
	}

	cout << "done" << endl;
#endif // DEBUG

	return MY_OK;
}

///////////////////////////////////////end of Connected Detection////////////////////////////////////////////////////


// get NeighborIndex 0-7,  return -1 if not neighbored 
// NeighborIndex matrix 
// 0 1 2
// 7   3
// 6 5 4
// difference matrix 
// (-1, -1) ( 0, -1) ( 1, -1) 
// (-1,  0) ( 0,  0) ( 1,  0) 
// (-1,  1) ( 0,  1) ( 1,  1) 
// convert to value matrix: 
//  0 10 20 
//  1    21 
//  2 12 22 
int Position2NeighborIndex(cv::Point destPt, cv::Point centerPt) {
	int index = 0;
	// for the difference (x, y), use 10x+y+11 as the switch_on value 
	int x = destPt.x - centerPt.x;
	int y = destPt.y - centerPt.y;
	int value = 10 * x + y + 11;
	switch (value)
	{
	case 0:
		return 0;
	case 10:
		return 1;
	case 20:
		return 2;
	case 21:
		return 3;
	case 22:
		return 4;
	case 12:
		return 5;
	case 2:
		return 6;
	case 1:
		return 7;
	default:
		// illegal value, the destPt is not an 8-neighbored point of the centerPt 
		return -1;
	}
	return index;
}


// store point value to destPt 
// NeighborIndex 0-7, -1 if not neighbored 
// NeighborIndex matrix 
// 0 1 2
// 7   3
// 6 5 4
// difference matrix 
// (-1, -1) ( 0, -1) ( 1, -1) 
// (-1,  0) ( 0,  0) ( 1,  0) 
// (-1,  1) ( 0,  1) ( 1,  1) 
bool NeighborIndex2Position(Point &destPt, Point centerPt, int index)
{
	int center_x = centerPt.x;
	int center_y = centerPt.y;
	switch (index)
	{
	case 0:
		destPt.x = center_x - 1;
		destPt.y = center_y - 1;
		return true;
	case 1:
		destPt.x = center_x;
		destPt.y = center_y - 1;
		return true;
	case 2:
		destPt.x = center_x + 1;
		destPt.y = center_y - 1;
		return true;
	case 3:
		destPt.x = center_x + 1;
		destPt.y = center_y;
		return true;
	case 4:
		destPt.x = center_x + 1;
		destPt.y = center_y + 1;
		return true;
	case 5:
		destPt.x = center_x;
		destPt.y = center_y + 1;
		return true;
	case 6:
		destPt.x = center_x - 1;
		destPt.y = center_y + 1;
		return true;
	case 7:
		destPt.x = center_x - 1;
		destPt.y = center_y;
		return true;
	default:
		return false;
	}
}


//function to judge the point is edge or not
bool IsEdge(Mat Label_Img, Point curr_point)
{
	int x = curr_point.x;
	int y = curr_point.y;

	int width = Label_Img.cols;
	int height = Label_Img.rows;

	int index = y * width + x;
	int p0 = ((int *)Label_Img.data)[index];

	if (!p0)
	{
		return false;
	}

	int p1 = ((int *)Label_Img.data)[index - width]; //up
	int p2 = ((int *)Label_Img.data)[index - 1]; //left
	int p3 = ((int *)Label_Img.data)[index + 1]; //right
	int p4 = ((int *)Label_Img.data)[index + width]; //down
	
	if (!p1)
	{
		return true;
	}
	if (!p2)
	{
		return true;
	}
	if (!p3)
	{
		return true;
	}
	if (!p4)
	{
		return true;
	}

	return false;
}

#ifdef IMPROVE

//function to save the edge of connected
void SaveEdge(Point edge_point, int &last_row, int &first_col, int &last_col)
{
	int x = edge_point.x;
	int y = edge_point.y;

	last_row = (y > last_row) ? y : last_row;
	first_col = (x < first_col) ? x : first_col;
	last_col = (x > last_col) ? x : last_col;
}

// function to remove the label -- improved
bool RemoveLabel_0(Mat &Label_Img, int first_row, int last_row, int first_col, int last_col, int label)
{
	int width = Label_Img.cols;
	int height = Label_Img.rows;

	for (int i_row = first_row; i_row <= last_row; i_row++)
	{
		int row_offset = i_row * width;
		for (int j_col = first_col; j_col <= last_col; j_col++)
		{
			int index = row_offset + j_col;
			if (label == ((int *)Label_Img.data)[index])
			{
				((int *)Label_Img.data)[index] = 0;
			}
		}
	}

	return true;
}

#else

// function to remove the label 
bool RemoveLabel(Mat &Label_Img, Point first_point, int label)
{
	int width = Label_Img.cols;
	int height = Label_Img.rows;

	for (int i_row = first_point.y; i_row < height; i_row++)
	{
		int row_offset = i_row * width;
		for (int j_col = 0; j_col < width; j_col++)
		{
			int index = row_offset + j_col;
			if (label == ((int *)Label_Img.data)[index])
			{
				((int *)Label_Img.data)[index] = 0;
			}
		}
	}

	return true;
}


#endif // IMPROVE


//function to find next point
int FindNextPoint(Mat Label_Img, Point curr_point, int start_index, Point &next_point)
{
	int width = Label_Img.cols;
	int height = Label_Img.rows;

	Point point_temp;
	for (int i = start_index + 1; i <= 7; i++)
	{
		NeighborIndex2Position(point_temp, curr_point, i);

		if (IsEdge(Label_Img, point_temp))
		{
			next_point = point_temp;
			return MY_OK;
		}
	}

	for (int i = 0; i < start_index; i++)
	{
		NeighborIndex2Position(point_temp, curr_point, i);

		if (IsEdge(Label_Img, point_temp))
		{
			next_point = point_temp;
			return MY_OK;
		}
	}

	NeighborIndex2Position(point_temp, curr_point, start_index);
	if (IsEdge(Label_Img, point_temp))
	{
		next_point = point_temp;
		return MY_OK;
	}

	return MY_FAIL;
}

//function to find contours
int USTC_Find_Contours(
	Mat binaryImg,     //二值图像，8UC1
	vector < vector < cv::Point >>& contours)//轮廓存储向量
{
	if (NULL == binaryImg.data)
	{
		cout << "imag is NULL" << endl;
		return MY_FAIL;
	}

	//clear the vector
	contours.clear();
	vector<Point> curr_contour;

	//size of image
	int height = binaryImg.rows;
	int width = binaryImg.cols;

	//////////////////////////////connected detection///////////////////////////////////////
	Mat labelImg(height, width, CV_32SC1); //define an image to store the label

	ustc_ConnectedComponentLabeling(binaryImg, labelImg, BIN_TH);

	///////////////////////////////extend border////////////////////////////////////////////////////////
	Mat Label_Img(height + 2, width + 2, CV_32SC1);

	copyMakeBorder(labelImg, Label_Img, 1, 1, 1, 1, BORDER_CONSTANT, 0);

	//////////////////////////////find contours///////////////////////////////////////////
	int width_plus = width + 2;
	for (int i_row = 1; i_row <= height; i_row++)
	{
		int row_offset = i_row * width_plus;
		for (int j_col = 1; j_col <= width; j_col++)
		{
			int index = row_offset + j_col;
			int label = ((int *)Label_Img.data)[index];
			if (label)
			{
				Point last_point;
				Point curr_point;
				Point next_point;
				Point first_point = Point(j_col, i_row);
				Point original_point; //the position of original image

#ifdef IMPROVE
				int first_col = width;
				int last_col = 0;
				int last_row = 0;
#endif // IMPROVE

				if (-1 == FindNextPoint(Label_Img, first_point, 1, curr_point))
				{
					//contain only one point
					((int *)Label_Img.data)[index] = 0;
					break;
				}//end if
				else
				{
					original_point = first_point - Point(1, 1);
#ifdef IMPROVE
					SaveEdge(original_point, last_row, first_col, last_col);
#endif // IMPROVE
					curr_contour.push_back(original_point);
					last_point = first_point;

					while (1)
					{
						original_point = curr_point - Point(1, 1);
#ifdef IMPROVE
						SaveEdge(original_point, last_row, first_col, last_col);
#endif // IMPROVE
						curr_contour.push_back(original_point);

						int start_index = Position2NeighborIndex(last_point, curr_point);
						FindNextPoint(Label_Img, curr_point, start_index, next_point);

						if (curr_point == first_point)
						{
							int index_temp = Position2NeighborIndex(next_point, curr_point);
							if (index_temp != 6)
							{
								break;
							}
						}

						last_point = curr_point;
						curr_point = next_point;

					}//end while	
				}//end else

				contours.push_back(curr_contour);

#ifdef IMPROVE
				RemoveLabel_0(Label_Img, first_point.y, last_row, first_col, last_col, label);
#else
				RemoveLabel(Label_Img, first_point, label);
#endif // IMPROVE


				
			}//end if
		}//end for cols
	}//end for rows

#ifdef IMG_SHOW
	Mat contour_Img(height, width, CV_8UC1);
	//contour_Img = binaryImg.clone();
	
	contour_Img.setTo(255);

	int Len = contours.size();
	for (int i = 0; i < Len; i++)
	{
		int len = contours[i].size();
		for (int j = 0; j < len; j++)
		{
			int index = contours[i][j].y * width + contours[i][j].x;
			contour_Img.data[index] = 0;
		}
	}

	namedWindow("contour", 0);
	imshow("contour", contour_Img);
	waitKey();

#endif // IMG_SHOW


	return MY_OK;
}
