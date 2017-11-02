
#include "FindContours.h"
using namespace cv;
using namespace std;
typedef struct MLABEL
{
	int labelnum;
	int result;
};
int ustc_ConnectedComponentLabeling(
	Mat grayImg,
	Mat& labelImg,
	int bin_th)
{
	if (grayImg.data == NULL)
	{
		cout << "no image!" << endl;
		return MY_FAIL;
	}
	if (bin_th <= 0 || bin_th >= 255)
	{
		cout << "the threshhold is invalid!" << endl;
		return MY_FAIL;
	}
	Mat BinaryImg;
	threshold(grayImg, BinaryImg, bin_th, 255, CV_THRESH_BINARY);
	Mat borderImg;
	copyMakeBorder(BinaryImg, borderImg, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));
	Mat Label = Label.zeros(borderImg.size(), CV_32SC1);
	int rows = BinaryImg.rows;
	int cols = BinaryImg.cols;
	int row_bor = borderImg.rows;
	int col_bor = borderImg.cols;
	int pix_row = 0, pix_col = 0;
	uchar *dataB = borderImg.data;
	int *dataL = (int*)Label.data;
	int label_num = 1;//label's number,
	vector<MLABEL>Label_array(1);
	vector<MLABEL>Extra_array;
	for (pix_row = 1; pix_row <= rows; pix_row++)
	{
		for (pix_col = 1; pix_col <= cols; pix_col++)
		{
			uchar a_Binary = dataB[(pix_row - 1)*col_bor + pix_col - 1], b_Binary = dataB[(pix_row - 1)*col_bor + pix_col], c_Binary = dataB[(pix_row - 1)*col_bor + pix_col + 1];
			uchar  d_Binary = dataB[pix_row*col_bor + pix_col - 1], e_Binary = dataB[pix_row*col_bor + pix_col];
			int a_Label = dataL[(pix_row - 1)*col_bor + pix_col - 1], b_Label = dataL[(pix_row - 1)*col_bor + pix_col], c_Label = dataL[(pix_row - 1)*col_bor + pix_col + 1];
			int d_Label = dataL[pix_row*col_bor + pix_col - 1], *e_Label = &(dataL[pix_row*col_bor + pix_col]);
			int flag[4] = { a_Label, b_Label, c_Label,  d_Label };
			if (e_Binary == 255)
			{
				uchar zero_num = a_Binary + 1 + b_Binary + 1 + (c_Binary + 1) + (d_Binary + 1);
				/*for (int i = 3; i >= 1; i--)
				{
				for (int j = 0; j < i; j++)
				{
				if (flag[j] < flag[j + 1])
				{
				int temp = flag[j];
				flag[j] = flag[j + 1];
				flag[j + 1] = temp;
				}
				}
				}*/
				int max_btw12 = (((a_Label - b_Label) >> 31) & 1)*b_Label + (((b_Label - a_Label - 1) >> 31) & 1)*a_Label;
				int min_btw12 = (((a_Label - b_Label) >> 31) & 1)*a_Label + (((b_Label - a_Label - 1) >> 31) & 1)*b_Label;
				int max_btw34 = (((c_Label - d_Label) >> 31) & 1)*d_Label + (((d_Label - c_Label - 1) >> 31) & 1)*c_Label;
				int min_btw34 = (((c_Label - d_Label) >> 31) & 1)*c_Label + (((d_Label - c_Label - 1) >> 31) & 1)*d_Label;
				int max_btw1234 = (((max_btw12 - max_btw34) >> 31) & 1)*max_btw34 + (((max_btw34 - max_btw12 - 1) >> 31) & 1)*max_btw12;
				int min_btw1234 = (((min_btw12 - min_btw34) >> 31) & 1)*min_btw12 + (((min_btw34 - min_btw12 - 1) >> 31) & 1)*min_btw34;
				flag[0] = max_btw1234;
				int temp1 = max_btw12 + max_btw34 - max_btw1234;
				int temp2 = min_btw12 + min_btw34 - min_btw1234;
				flag[3] = min_btw1234;
				flag[1] = (((temp1 - temp2) >> 31) & 1)*temp2 + (((temp2 - temp1 - 1) >> 31) & 1)*temp1;
				flag[2] = (((temp1 - temp2) >> 31) & 1)*temp1 + (((temp2 - temp1 - 1) >> 31) & 1)*temp2;
				switch (zero_num)
				{
				case 0:
					*e_Label = flag[3];
					if (flag[0] != flag[3])
					{
						//flag[0]--flag[3]
						if (Label_array[flag[0]].result == flag[0])
						{
							Label_array[flag[0]].result = flag[3];
						}
						else
						{
							MLABEL newlabel;
							newlabel.labelnum = flag[0];
							newlabel.result = flag[3];
							Extra_array.push_back(newlabel);
						}
						//flag[1]--flag[3]
						if (Label_array[flag[1]].result == flag[1])
						{
							Label_array[flag[1]].result = flag[3];
						}
						else
						{
							MLABEL newlabel;
							newlabel.labelnum = flag[1];
							newlabel.result = flag[3];
							Extra_array.push_back(newlabel);
						}
						//flag[2]--flag[3]
						if (Label_array[flag[2]].result == flag[2])
						{
							Label_array[flag[2]].result = flag[3];
						}
						else
						{
							MLABEL newlabel;
							newlabel.labelnum = flag[2];
							newlabel.result = flag[3];
							Extra_array.push_back(newlabel);
						}
					}
					break;
				case 1:
					*e_Label = flag[2];
					if (flag[0] != flag[2])
					{
						//flag[0]--flag[2]
						if (Label_array[flag[0]].result == flag[0])
						{
							Label_array[flag[0]].result = flag[2];
						}
						else
						{
							MLABEL newlabel;
							newlabel.labelnum = flag[0];
							newlabel.result = flag[2];
							Extra_array.push_back(newlabel);
						}
						//flag[1]--flag[2]
						if (Label_array[flag[1]].result == flag[1])
						{
							Label_array[flag[1]].result = flag[2];
						}
						else
						{
							MLABEL newlabel;
							newlabel.labelnum = flag[1];
							newlabel.result = flag[2];
							Extra_array.push_back(newlabel);
						}
					}
					break;
				case 2:
					*e_Label = flag[1];
					if (flag[0] != flag[1])
					{
						//flag[0]--flag[1]
						if (Label_array[flag[0]].result == flag[0])
						{
							Label_array[flag[0]].result = flag[1];
						}
						else
						{
							MLABEL newlabel;
							newlabel.labelnum = flag[0];
							newlabel.result = flag[1];
							Extra_array.push_back(newlabel);
						}
					}
					break;
				case 3:
					*e_Label = flag[0];
					break;
				case 4:
					*e_Label = label_num;
					MLABEL newlabel;
					newlabel.labelnum = label_num;
					newlabel.result = label_num;
					Label_array.push_back(newlabel);
					label_num++;

					break;
				}

			}
		}
	}

	int array_index, temp_size = Label_array.size();
	for (array_index = 1; array_index < temp_size; array_index++)
	{
		int temp_result = Label_array[array_index].result;
		Label_array[array_index].result = Label_array[temp_result].result;
	}
	for (array_index = 0, temp_size = Extra_array.size(); array_index < temp_size; array_index++)
	{
		int connect1 = Extra_array[array_index].labelnum;
		int connect2 = Extra_array[array_index].result;

		int small_num = Label_array[connect1].result;
		int large_num = Label_array[connect2].result;
		if (Label_array[connect1].result != Label_array[connect2].result)
		{
			if (Label_array[connect1].result > Label_array[connect2].result)
			{
				small_num = Label_array[connect2].result;
				large_num = Label_array[connect1].result;
			}
			Label_array[large_num].result = small_num;
			int array_index_t, temp_size_t = Label_array.size();
			for (array_index_t = 1; array_index_t < temp_size_t; array_index_t++)
			{
				int temp_result = Label_array[array_index_t].result;
				Label_array[array_index_t].result = Label_array[temp_result].result;
			}
		}
	}
	temp_size = Label_array.size();
	int* lab_vector2array = new int[temp_size]();
	memset(lab_vector2array, 0, sizeof(int)*temp_size);
	for (array_index = 1; array_index < temp_size; array_index++)
	{
		lab_vector2array[array_index] = Label_array[array_index].result;
	}

	for (pix_row = 1; pix_row <= rows; pix_row++)
	{
		for (pix_col = 1; pix_col <= cols; pix_col++)
		{
			int *_label = &(dataL[pix_row*col_bor + pix_col]);
			int e_label = dataL[pix_row*col_bor + pix_col];

			*_label = lab_vector2array[e_label];
		}
	}
	labelImg = Label(Range(1, row_bor), Range(1, col_bor)).clone();
	delete[]lab_vector2array;
	return MY_OK;
}



int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours)
{

	contours.clear();
	Mat Label;
	Mat binaryImg_;
	copyMakeBorder(binaryImg, binaryImg_, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));   //enlarge on boundary
	int test = ustc_ConnectedComponentLabeling(binaryImg_, Label, 50);  //get grayImg label
	if (test == MY_FAIL)
	{
		cout << "there is an error in labeling connected component ";
		return(MY_FAIL);
	}



	int count_numof_boundary = 0;
	int height = Label.rows;
	int width = Label.cols;
	int *data = (int*)Label.data;

	int threshold = 0;             //the function equal to erase the ConnectedComponent already found 

	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			if (data[i*width + j] <= threshold)
				continue;
			vector<Point>  current_boundary;
			Point start_point(0, 0);
			Point current_point(0, 0);
			int last_flag = 0;
			int label_num = data[i*width + j];


			int* point_array[8] = { data + (i - 1)*width + j - 1, data + (i - 1)*width + j + 0, data + (i - 1)*width + j + 1,
				data + (i + 0)*width + j + 1,                               data + (i + 1)*width + j + 1,
				data + (i + 1)*width + j + 0, data + (i + 1)*width + j - 1, data + (i + 0)*width + j - 1 };
			int x_loca[8] = { i - 1,i - 1,i - 1,
				i,           i + 1,
				i + 1,i + 1,i + 0 };
			int y_loca[8] = { j - 1, j,     j + 1,
				j + 1,        j + 1,
				j + 0,j - 1,  j - 1 };
			start_point.x = i;
			start_point.y = j;

			Point change_xy(start_point.y - 1, start_point.x - 1);        //axis x and y is inverse personal habit
			current_boundary.push_back(change_xy);

			int flag = 0;
			for (int temp_count = 7; temp_count >= 0; temp_count--)
			{
				if (*(point_array[temp_count]) > threshold)
				{
					current_point.x = x_loca[temp_count];
					current_point.y = y_loca[temp_count];
					last_flag = temp_count;
					flag++;
					break;
				}
			}
			if (flag == 0)  continue;               //handle the one point noise

													//the following codes determin how many times the start points will be visited  
			int loca_two_points[2] = { 6,6 };              //to locate the first to neighbor points
			flag = 0;
			for (int temp_count = 7; temp_count >= 0; temp_count--)
			{
				if (*(point_array[temp_count]) > threshold)
				{
					loca_two_points[flag] = temp_count;
					flag++;
					if (flag >= 2) break;
				}
			}


			int startpoint_visit_times = 1;
			if (loca_two_points[0] - loca_two_points[1] > 1)
			{
				startpoint_visit_times = 2;
			}


			while (startpoint_visit_times != 0)
			{
				Point change_xy(current_point.y - 1, current_point.x - 1);
				current_boundary.push_back(change_xy);


				int current_i = current_point.x;
				int current_j = current_point.y;
				int* point_array[8] = { data + (current_i - 1)*width + current_j - 1, data + (current_i - 1)*width + current_j + 0, data + (current_i - 1)*width + current_j + 1,
					data + (current_i + 0)*width + current_j + 1,                                               data + (current_i + 1)*width + current_j + 1,
					data + (current_i + 1)*width + current_j + 0 ,data + (current_i + 1)*width + current_j - 1, data + (current_i + 0)*width + current_j - 1 };
				int x_loca[8] = { current_i - 1,current_i - 1,current_i - 1,
					current_i,                  current_i + 1,
					current_i + 1,current_i + 1,current_i + 0 };
				int y_loca[8] = { current_j - 1,current_j,current_j + 1,
					current_j + 1,          current_j + 1,
					current_j + 0,current_j - 1,current_j - 1 };
				int search_start = (last_flag + 2) % 8;
				int flag = 0;
				for (int temp_i = 0; temp_i < 6; temp_i++)
				{
					if (*(point_array[(search_start - temp_i + 8) % 8]) > threshold)
					{
						current_point.x = x_loca[(search_start - temp_i + 8) % 8];
						current_point.y = y_loca[(search_start - temp_i + 8) % 8];
						last_flag = (search_start - temp_i + 8) % 8;
						flag = 1;
						break;
					}
				}
				if (flag == 0)
				{
					last_flag = (last_flag + 4) % 8;
					current_point.x = x_loca[last_flag];
					current_point.y = y_loca[last_flag];
				}

				if (current_point == start_point)
				{
					startpoint_visit_times--;
				}
			}

			contours.push_back(current_boundary);
			count_numof_boundary++;
			threshold = label_num;
		}
	}
	return(MY_OK);
}
