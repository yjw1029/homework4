#include "FindContours.h"

int USTC_Find_Contours(
	Mat binaryImg, 
	vector < vector < cv::Point >>& contours
	)
{
	//0.0
	if (NULL == binaryImg.data)
	{
		cout << "图片为空！" << endl;
		return MY_FAIL;
	}
	if (binaryImg.channels() != 1) 
	{
		cout << "图片格式错误！" << endl;
		return MY_FAIL;
	}

	//0.1 标记
	Mat labelImg;
	ustc_ConnectedComponentLabeling(
		binaryImg,
		labelImg
		);
	int Rows = labelImg.rows;
	int Cols = labelImg.cols;
	int ExRows = Rows + 2;
	int ExCols = Cols + 2;
	Mat Expand_Img(ExRows, ExCols, CV_32SC1);
	Expand_Img.setTo(0);
	int *p = (int*)Expand_Img.data;
	int *q = (int*)labelImg.data;
	for (int i = 1; i < ExRows - 1; i++)
	{
		p += ExCols;
		for (int j = 1; j < ExCols - 1; j++)
		{
			p[j] = *q;
			q++;
		}
	}
	//Mat showImg(ExRows, ExCols, CV_8UC1);
	//showImg.setTo(0);
	//Expand_Img.convertTo(showImg, CV_8UC1, 120, 0);
	//namedWindow("IMG after", WINDOW_AUTOSIZE);
	//imshow("IMG after", showImg);
	//waitKey(0);

	//1.0 clear
	contours.clear();

	//1.1 label for current components
	
	p = (int*)Expand_Img.data;
	int *temp_p = NULL;
	int relative_num = -1;
	vector<cv::Point> curr_contour;//记录当前轮廓
	//vector<cv::Point> surround_first_point;//记录第一个点周围记录点
	int *num[10000];
	for (int i1 = 0; i1 < 10000; i1++)
	{
		num[i1] = NULL;
	}
	int numsize = 0;
	int max_label[10000]; //最大连通域数目
	for (int index = 0; index < 10000; index++)
	{
		max_label[index] = index;
	}
	for (int i = 1; i < ExRows - 1; i++)
	{
		p += ExCols;
		for (int j = 1; j < ExCols - 1; j++)
		{
			if (!max_label[p[j]])
			{
				continue;
			}
			temp_p = p;

			//第一个点
			curr_contour.clear();
			Point curr_point = Point(j, i);
			Point first_point = Point(j, i);
			Point last_point = Point(j, i);
			curr_contour.push_back(curr_point);
			max_label[p[j]] = 0;
			int label_relative_position[9][2] = { { 1, 0 }, { 1, 1 }, { 0, 1 }, { -1, 1 }, { -1, 0 }, { -1, -1 }, { 0, -1 }, { 1, -1 } };

			//第二个点
			numsize = 0;
			int right = temp_p[j + 1];
			int down_index = j + ExCols;
			int right_down = temp_p[down_index + 1];
			int down = temp_p[down_index];
			int left_down = temp_p[down_index - 1];
			if (right)
			{
				curr_point = Point(j + 1, i);
				relative_num = 4;
				temp_p = temp_p + 1;

			}
			else if (right_down)
			{
				curr_point = Point(j + 1, i + 1);
				relative_num = 5;
				temp_p = temp_p + 1 + ExCols;

			}
			else if (down)
			{
				curr_point = Point(j, i + 1);
				relative_num = 6;
				temp_p = temp_p + ExCols;
			}
			else if (left_down)
			{
				curr_point = Point(j - 1, i - 1);
				relative_num = 7;
				temp_p = temp_p - 1 + ExCols;

			}
			else
				//only one point
			{
				curr_contour.push_back(curr_point);
				contours.push_back(curr_contour);
				continue;
			}
			//surround_first_point.clear();
			//surround_first_point.push_back(curr_point);
			num[numsize] = temp_p + j;
			numsize++;
			last_point = curr_point;
			curr_contour.push_back(curr_point);

			int surround_val[8];
			surround_val[0] = j + 1;
			surround_val[1] = down_index + 1;
			surround_val[2] = down_index;
			surround_val[3] = down_index - 1;
			surround_val[4] = j - 1;
			int up_index = j - ExCols;
			surround_val[5] = up_index - 1;
			surround_val[6] = up_index;
			surround_val[7] = up_index + 1;

			//下一个点
			while (1)
			{
				int last_position = relative_num;
				int rec_change = 0;
				for (int ii = last_position + 1; ii < 8; ii++)
				{
					if (temp_p[surround_val[ii]])
					{
						Point a = Point(label_relative_position[ii][0], label_relative_position[ii][1]);
						curr_point = last_point + a;
						last_point = curr_point;
						relative_num = (4 + ii) % 8;
						rec_change = 1;
						temp_p = temp_p + surround_val[ii] - j;
						curr_contour.push_back(curr_point);
						break;
					}
				}
				if (!rec_change)
				{

					for (int ii = 0; ii < last_position; ii++)
					{
						if (temp_p[surround_val[ii]])
						{
							curr_point = last_point + Point(label_relative_position[ii][0], label_relative_position[ii][1]);
							last_point = curr_point;
							relative_num = (4 + ii) % 8;
							rec_change = 1;
							temp_p = temp_p + surround_val[ii] - j;
							curr_contour.push_back(curr_point);
							break;
						}
					}
				}
				//直线型轮廓到头
				if (!rec_change)
				{
					//直线型轮廓尽头
					if (curr_point == first_point)
					{
						contours.push_back(curr_contour);
						break;
					}
					temp_p[j] = 0;
					curr_contour.push_back(curr_point);
					curr_point = last_point + Point(label_relative_position[relative_num][0], label_relative_position[relative_num][1]);
					last_point = curr_point;
					temp_p = temp_p + label_relative_position[relative_num][0] + label_relative_position[relative_num][1] * ExCols;
					relative_num = (4 + relative_num) % 8;
					curr_contour.push_back(curr_point);
					
					continue;
				}

				//圆型轮廓尽头
				if (curr_point == first_point)
				{
					num[numsize] = temp_p + j + label_relative_position[relative_num][0] + label_relative_position[relative_num][1] * ExCols;
					numsize++;
					for (int iii = 0; iii <numsize; iii++)
					{
						num[iii][0] = 0;
					}
					if (num[0] == temp_p + 1 + j)
					{
						if (num[1] == temp_p + ExCols + j)
						{
							num[1][1] = 0;
						}
						else if (num[1] == temp_p + ExCols -1 + j)
						{
							num[1][1] = 0;
							num[1][2] = 0;
						}
					}
					else if (num[0] == temp_p + ExCols + 1 + j)
					{
						if (num[1] == temp_p + ExCols + j - 1)
						{
							num[1][1] = 0;
						}
					}
					rec_change = 0;
					int last_position = relative_num;
					for (int ii = last_position + 1; ii < 8; ii++)
					{
						if (temp_p[surround_val[ii]])
						{
							Point a = Point(label_relative_position[ii][0], label_relative_position[ii][1]);
							curr_point = last_point + a;
							last_point = curr_point;
							relative_num = (4 + ii) % 8;
							rec_change = 1;
							temp_p = temp_p + surround_val[ii] - j;
							break;
						}
					}
					if (!rec_change)
					{
						for (int ii = 0; ii < last_position; ii++)
						{
							if (temp_p[surround_val[ii]])
							{
								curr_point = last_point + Point(label_relative_position[ii][0], label_relative_position[ii][1]);
								last_point = curr_point;
								relative_num = (4 + ii) % 8;
								rec_change = 1;
								temp_p = temp_p + surround_val[ii] - j;
								break;
							}
						}
					}
					if (rec_change)
					{
						curr_contour.push_back(curr_point);
						//surround_first_point.push_back(curr_point);
						num[numsize] = temp_p + j;
						numsize++;
						continue;
					}
					contours.push_back(curr_contour);
					break;
				}
			}
		}
	}
	
	return MY_OK;
}

int ustc_ConnectedComponentLabeling(
	Mat binaryImg,
	Mat& labelImg
	)
{

	int Rows = binaryImg.rows;
	int Cols = binaryImg.cols;
	int ExRows = Rows + 2;
	int ExCols = Cols + 2;
	Mat Expand_Img(ExRows, ExCols, CV_32SC1);
	Expand_Img.setTo(0);

	
	Mat tempImg = Expand_Img(Rect(1, 1, Cols, Rows));
	binaryImg.convertTo(tempImg, CV_32SC1);
	int *p = NULL;

	int max_label = 10000; //最大连通域数目
	//int max_label = Rows*Cols / 2; //最大连通域数目
	int *label_Table = new int[max_label]; //等价表格
	int *zuji1 = new int[max_label];
	int *zuji2 = new int[max_label];
	int *zuji3 = new int[max_label];
	int *zuji4 = new int[max_label];
	int curr_label;
	for (curr_label = 0; curr_label < max_label; curr_label++)
	{
		label_Table[curr_label] = curr_label;
	}
	curr_label = 0;

	int i, j;
	int i1, i2, i3, i4;
	int temp1_label, temp2_label, temp3_label, temp4_label;
	p = (int*)Expand_Img.data;
	int index_up;
	int temp_minlabel;
	int up, upleft, upright, left;
	int type;
	for (i = 1; i < ExRows - 1; i++)
	{
		p += ExCols;
		for (j = 1; j < ExCols - 1; j++)
		{
			index_up = j - ExCols;
			up = p[index_up];
			upleft = p[index_up - 1];
			upright = p[index_up + 1];
			left = p[j - 1];
			type = (!!upleft)*8+ (!!up) *4 + (!!upright)*2 + !!left;
			if (p[j])
			{
				switch (type)
				{
				case 0://无标记
					curr_label++;
					p[j] = curr_label;
					break;
					//只有一个标记
				case 1://只有左有标记
					p[j] = left;
					break;
				case 2://只有右上有标记
					p[j] = upright;
					break;
				case 4://只有上有标记
					p[j] = up;
					break;
				case 8://只有左上有标记
					p[j] = upleft;
					break;
					//有两个标记
				case 3://左、右上有标记
					if (left == upright)
					{
						p[j] = left;
						break;
					}
					temp1_label = left;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = upright;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;

					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
				case 5://左、上有标记
					if (left == up)
					{
						p[j] = left;
						break;
					}
					temp1_label = left;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = up;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;
					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
				case 6://上、右上有标记
					if (up == upright)
					{
						p[j] = up;
						break;
					}
					temp1_label = up;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = upright;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;
					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
				case 9://左上、左有标记
					if (left == upleft)
					{
						p[j] = left;
						break;
					}
					temp1_label = upleft;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = left;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;
					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
				case 10://左上、右上有标记
					if (upleft == upright)
					{
						p[j] = upleft;
						break;
					}
					temp1_label = upleft;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = upright;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;
					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
				case 12://左上、上有标记
					if (upleft == up)
					{
						p[j] = up;
						break;
					}
					temp1_label = upleft;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = up;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;
					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
					//有三个标记
				case 7://上、右上、左有标记
					if ((up == upright) && (up == left))
					{
						p[j] = left;
						break;
					}
					temp1_label = up;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = upright;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp3_label = left;
					zuji3[0] = temp3_label;
					for (i3 = 1; label_Table[temp3_label] != temp3_label; i3++)
					{
						temp3_label = label_Table[temp3_label];
						zuji3[i3] = temp3_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;
					temp_minlabel = temp_minlabel < temp3_label ? temp_minlabel : temp3_label;
					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					for (i3--; i3 >= 0; i3--)
					{
						label_Table[zuji3[i3]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
				case 11://左上、右上、左有标记
					if ((upleft == upright) && (upleft == left))
					{
						p[j] = left;
						break;
					}
					temp1_label = upleft;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = upright;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp3_label = left;
					zuji3[0] = temp3_label;
					for (i3 = 1; label_Table[temp3_label] != temp3_label; i3++)
					{
						temp3_label = label_Table[temp3_label];
						zuji3[i3] = temp3_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;
					temp_minlabel = temp_minlabel < temp3_label ? temp_minlabel : temp3_label;
					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					for (i3--; i3 >= 0; i3--)
					{
						label_Table[zuji3[i3]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
				case 13://左上、上、左有标记
					if ((up == upleft) && (up == left))
					{
						p[j] = left;
						break;
					}
					temp1_label = upleft;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = up;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp3_label = left;
					zuji3[0] = temp3_label;
					for (i3 = 1; label_Table[temp3_label] != temp3_label; i3++)
					{
						temp3_label = label_Table[temp3_label];
						zuji3[i3] = temp3_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;
					temp_minlabel = temp_minlabel < temp3_label ? temp_minlabel : temp3_label;
					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					for (i3--; i3 >= 0; i3--)
					{
						label_Table[zuji3[i3]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
				case 14://左上、上、右上有标记
					if ((up == upright) && (up == upleft))
					{
						p[j] = upleft;
						break;
					}
					temp1_label = upleft;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = up;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp3_label = upright;
					zuji3[0] = temp3_label;
					for (i3 = 1; label_Table[temp3_label] != temp3_label; i3++)
					{
						temp3_label = label_Table[temp3_label];
						zuji3[i3] = temp3_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;
					temp_minlabel = temp_minlabel < temp3_label ? temp_minlabel : temp3_label;
					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					for (i3--; i3 >= 0; i3--)
					{
						label_Table[zuji3[i3]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
					//有四个标记
				case 15://四个均有标记
					if ((up == upright) && (up == left) && (up == upleft))
					{
						p[j] = left;
						break;
					}
					temp1_label = upleft;
					zuji1[0] = temp1_label;
					for (i1 = 1; label_Table[temp1_label] != temp1_label; i1++)
					{
						temp1_label = label_Table[temp1_label];
						zuji1[i1] = temp1_label;
					}
					temp2_label = up;
					zuji2[0] = temp2_label;
					for (i2 = 1; label_Table[temp2_label] != temp2_label; i2++)
					{
						temp2_label = label_Table[temp2_label];
						zuji2[i2] = temp2_label;
					}
					temp3_label = upright;
					zuji3[0] = temp3_label;
					for (i3 = 1; label_Table[temp3_label] != temp3_label; i3++)
					{
						temp3_label = label_Table[temp3_label];
						zuji3[i3] = temp3_label;
					}
					temp4_label = left;
					zuji4[0] = temp4_label;
					for (i4 = 1; label_Table[temp4_label] != temp4_label; i4++)
					{
						temp4_label = label_Table[temp4_label];
						zuji4[i4] = temp4_label;
					}
					temp_minlabel = temp1_label < temp2_label ? temp1_label : temp2_label;
					temp_minlabel = temp_minlabel < temp3_label ? temp_minlabel : temp3_label;
					temp_minlabel = temp_minlabel < temp4_label ? temp_minlabel : temp4_label;
					for (i1--; i1 >= 0; i1--)
					{
						label_Table[zuji1[i1]] = temp_minlabel;
					}
					for (i2--; i2 >= 0; i2--)
					{
						label_Table[zuji2[i2]] = temp_minlabel;
					}
					for (i3--; i3 >= 0; i3--)
					{
						label_Table[zuji3[i3]] = temp_minlabel;
					}
					for (i4--; i4 >= 0; i4--)
					{
						label_Table[zuji4[i4]] = temp_minlabel;
					}
					p[j] = temp_minlabel;
					break;
				default:cout << "未知错误！" << endl;
					return MY_FAIL;
				}
			}

		}
	}
	for (i = 2; i <= curr_label; i++)
	{
		label_Table[i] = label_Table[label_Table[i]];
	}
	
 	p = (int*)Expand_Img.data;
	for (i = 1; i < ExRows - 1; i++)
	{
		p += ExCols;
		for (j = 1; j < ExCols - 1; j++)
		{
			p[j] = label_Table[p[j]];
		}
	}
	tempImg.copyTo(labelImg);

	delete[]label_Table;
	return MY_OK;
}

