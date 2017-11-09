#include<FindContours.h>

#define OK		1
#define FAIL	0
#define MAX_LABLE	100000

int *table;
int length;

int Connect_Update(int a, int b) {
	int con_a = table[a];
	int con_b = table[b];
	if (con_a > con_b) {
		int temp = con_a;
		con_a = con_b;
		con_b = temp;
	}
	for (int i = con_b; i <= length; i++)
	{
		if (table[i] == con_b) {
			table[i] = con_a;
		}
	}
	return min(a, b);
}

int First_Traverse(Mat BinaryImg, Mat &LabelImg) {
	if (NULL == BinaryImg.data || NULL == LabelImg.data)
	{
		cout << "image read failed." << endl;
		return FAIL;
	}


	int width = BinaryImg.cols;
	int height = BinaryImg.rows;
	int label_width = LabelImg.cols;
	int label_height = LabelImg.rows;
	if (height != label_height || width != label_width)
	{
		cout << "image size wrong." << endl;
		return FAIL;
	}

	length = 0;
	memset(table, 0, width*height * sizeof(int));
	LabelImg.setTo(0);

	uchar* binptr = BinaryImg.data;
	float* labelptr = (float*)LabelImg.data;
	//length += (*binptr & 1);
	//table[(*binptr & 1)] = (*binptr & 1);
	if (*binptr) //first pixel
	{
		table[1] = 1;
		length = 1;
		*labelptr = length;
	}
	binptr++; labelptr++;
	for (int j = 1; j < width; j++) //first row without first pixel
	{
		switch ((*binptr & 2) + (*(binptr - 1) & 1))
		{
		case 0:	break;
		case 1:	break;
		case 2:
		{
			length++;
			table[length] = length;
			*labelptr = length;
			break;
		}
		case 3: *labelptr = *(labelptr - 1);
		}
		binptr++; labelptr++;
	}

	for (int i = 1; i < height; i++)
	{
		switch ((*binptr & 4) + (*(binptr - width) & 2) + (*(binptr - width + 1) & 1))
			//	2	1
			//	4
		{
		case 0: break;
		case 1: break;
		case 2: break;
		case 3: break;
		case 4:
		{
			length++;
			table[length] = length;
			*labelptr = length;
			break;
		}
		case 5:
		{
			*labelptr = *(labelptr - width + 1);
			break;
		}
		case 6:
		case 7:
		{
			*labelptr = *(labelptr - width);
			break;
		}

		}
		labelptr++; binptr++;
		for (int j = 1; j < width - 1; j++)
		{
			switch ((*binptr & 16) + (*(binptr - 1) & 8) + (*(binptr - width - 1) & 4) + (*(binptr - width) & 2) + (*(binptr - width + 1) & 1))
				//	4	2	1	
				//	8	16
			{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15: break;
			case 16:
			{
				length++;
				table[length] = length;
				*labelptr = length;
				break;
			}
			case 17:
			{
				*labelptr = *(labelptr - width + 1);
				break;
			}
			case 18:
			case 19:
			{
				*labelptr = *(labelptr - width);
				break;
			}
			case 20:
			{
				*labelptr = *(labelptr - width - 1);
				break;
			}
			case 21:
			{
				*labelptr = Connect_Update(*(labelptr - width - 1), *(labelptr - width + 1));
				break;
			}
			case 22:
			case 23:
			{
				*labelptr = *(labelptr - width - 1);
				break;
			}
			case 24:
			{
				*labelptr = *(labelptr - 1);
				break;
			}
			case 25:
			{
				*labelptr = Connect_Update(*(labelptr - 1), *(labelptr - width + 1));
				break;
			}
			case 26:
			case 27:
			case 28:
			{
				*labelptr = *(labelptr - 1);
				break;
			}
			case 29:
			{
				*labelptr = *labelptr = Connect_Update(*(labelptr - 1), *(labelptr - width + 1));
				break;
			}
			case 30:
			case 31:
			{
				*labelptr = *(labelptr - 1);
				break;
			}
			}
			labelptr++; binptr++;
		}
		switch ((*binptr & 8) + (*(binptr - 1) & 4) + (*(binptr - width - 1) & 2) + (*(binptr - width) & 1))
			//	2	1
			//	4	8
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:break;
		case 8:
		{
			length++;
			table[length] = length;
			*labelptr = length;
			break;
		}
		case 9:
		{
			*labelptr = *(labelptr - width);
			break;
		}
		case 10:
		case 11:
		{
			*labelptr = *(labelptr - width - 1);
			break;
		}
		case 12:
		case 13:
		case 14:
		case 15:
		{
			*labelptr = *(labelptr - 1);
			break;
		}
		}
		labelptr++; binptr++;
	}


	return OK;
}

int Second_Traverse(Mat &LabelImg) {
	if (NULL == LabelImg.data)
	{
		cout << "image read failed." << endl;
		return FAIL;
	}
	float* ptr = (float*)LabelImg.data;
	int width = LabelImg.cols;
	int height = LabelImg.rows;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++) {
			*ptr = table[(int)*ptr];
			ptr++;
		}
	}

	return OK;
}

int ustc_ConnectedComponentLabeling(Mat grayImg, Mat& labelImg)
{
	if (NULL == grayImg.data || NULL == labelImg.data)
	{
		cout << "image read failed." << endl;
		return FAIL;
	}

	int width = grayImg.cols;
	int height = grayImg.rows;
	int label_width = labelImg.cols;
	int label_height = labelImg.rows;
	if (height != label_height || width != label_width)
	{
		cout << "image size wrong." << endl;
		return FAIL;
	}
	Mat BinaryImg(height, width, CV_8UC1);

	table = (int*)malloc(width*height * sizeof(int));

	length = 0;

	int flag = First_Traverse(grayImg, labelImg);

	flag = Second_Traverse(labelImg);

	return OK;
}

int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours)
{
	int width = binaryImg.cols;
	int height = binaryImg.rows;
	int TraversedLabel[MAX_LABLE] = { 0 };
	int Dirt_x[8] = { -1,-1,-1,0,1,1,1,0 };
	int Dirt_y[8] = { -1,0,1,1,1,0,-1,-1 };
	//  0  1  2
	//  7  x  3
	//	6  5  4
	Mat labelImg(height, width, CV_32FC1);
	ustc_ConnectedComponentLabeling(binaryImg, labelImg);	//build a image with label

	Mat newImg(height + 2, width + 2, CV_32FC1); //build a new image which has a zero edge
	memset(newImg.data, 0, (height + 2)*(width + 2) * sizeof(float));
	for (int i = 0;i < height;i++)
	{
		for (int j = 0;j < width;j++)
		{
			((float*)newImg.data)[(i + 1) * (width + 2) + j + 1] = ((float*)labelImg.data)[i * width + j];
			//cout << ((float*)newImg.data)[(i + 1) * (width + 2) + j + 1] << " ";
		}
		//cout << endl;
	}
	width += 2;
	height += 2;
	
	//Mat showImg(height - 2, width - 2, CV_8UC1);

	float *pointer = newImg.ptr<float>(0);
	for (int i = 0;i < width * height;i++)
	{
		int label = *pointer;
		if (label != 0 && TraversedLabel[label] == 0)		//first point
		{
			TraversedLabel[label] = 1;
			Point point = Point(i / width - 1, i % width - 1);
			vector<cv::Point> edge;
			edge.push_back(point);

			float *last = NULL;
			int lastDirt = -1;
			for (int j = 0;j < 8;j++)		//second point
			{
				float *next = pointer + Dirt_x[j] * width + Dirt_y[j];
				if (*next == label)
				{
					last = pointer;
					pointer = next;
					point.x += Dirt_x[j];
					point.y += Dirt_y[j];
					edge.push_back(point);
					lastDirt = (j + 4) % 8;
					break;
				}
			}
			if (lastDirt == -1)		//second point not exist
			{
				*pointer = 0;
				continue;
			}

			float *begin1 = last, *begin2 = pointer;
			while(1)		//other points
			{
				for (int j = lastDirt + 1;j < 8;j++)
				{
					float *next = pointer + Dirt_x[j] * width + Dirt_y[j];
					if (*next == label)
					{
						last = pointer;
						pointer = next;
						point.x += Dirt_x[j];
						point.y += Dirt_y[j];
						lastDirt = (j + 4) % 8;
						goto NEXT;
					}
				}

				for (int j = 0;j < lastDirt + 1;j++)
				{
					float *next = pointer + Dirt_x[j] * width + Dirt_y[j];
					if (*next == label)
					{
						last = pointer;
						pointer = next;
						point.x += Dirt_x[j];
						point.y += Dirt_y[j];
						lastDirt = (j + 4) % 8;
						goto NEXT;
					}
				}

			NEXT:
				if (begin1 == last && begin2 == pointer)
					break;
				else
					edge.push_back(point);
			}

			pointer = begin1;
			contours.push_back(edge);
			
			//for (int j = 0;j < edge.size();j++)
			//{
			//	showImg.data[edge[j].x * (width - 2) + edge[j].y] = 255;
			//}
		}
		pointer++;
	}
	//namedWindow("Edge", 0);
	//imshow("Edge", showImg);
	//waitKey(1);

	return MY_OK;
}
