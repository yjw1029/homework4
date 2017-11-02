#define MY_OK 1
#define MY_FAIL -1
#define PLACE0 now - width - 3
#define PLACE1 now - width - 2
#define PLACE2 now - width - 1
#define PLACE3 now + 1
#define PLACE4 now + width + 3
#define PLACE5 now + width + 2
#define PLACE6 now + width + 1
#define PLACE7 now - 1



//declaration part
int USTC_Find_Contours(Mat binaryImg, vector<vector<Point>> &contours);
int Find(int* first, int* second, int first_x, int first_y, int flag, vector<Point> &temp_contours, int width , int last_flag);
int Find_Contours(Mat labelImg, vector<vector<Point>> &contours);
int ustc_refresh(int num1, int num2, int len, int *point);
int ustc_ConnectedComponentLabeling(Mat binaryImg, Mat &labelImg, int bin_th);
//end



int USTC_Find_Contours(Mat binaryImg, vector<vector<Point>> &contours)
{
	int height = binaryImg.rows;
	int width = binaryImg.cols;
	int result;
	Mat labelImg(height, width, CV_32SC1);
	labelImg.setTo(0);
	result = ustc_ConnectedComponentLabeling(binaryImg, labelImg, 200);
	if (result == MY_FAIL)
	{
		return result;
	}
	result = Find_Contours(labelImg, contours);
	return result;
}


int Find(int* first, int* second, int first_x,int first_y,int flag,vector<Point> &temp_contours,int width,int last_flag)
{
	int *now = second;
	int *last = first;
	int *next = NULL;
	Point now_point;
	now_point.x = first_x;
	now_point.y = first_y;

	temp_contours.push_back(now_point);


	while ((now != first)|| ( (last_flag)&&(flag!=6) )   )
	{
		bool sign0,sign1, sign2, sign3, sign4, sign5,sign6,sign7;
		int binary;

		switch (flag)
		{
		case 0: now_point.x++; now_point.y++; break;
		case 1: now_point.y++; break;
		case 2: now_point.x--; now_point.y++; break;
		case 3: now_point.x--; break;
		case 4: now_point.x--; now_point.y--; break;
		case 5: now_point.y--; break;
		case 6: now_point.x++; now_point.y--; break;
		case 7: now_point.x++; break;
		}

		temp_contours.push_back(now_point);
		
		sign0 = *(PLACE0);
		sign1 = *(PLACE1);
		sign2 = *(PLACE2);
		sign3 = *(PLACE3);
		sign4 = *(PLACE4);
		sign5 = *(PLACE5);
		sign6 = *(PLACE6);
		sign7 = *(PLACE7);

		switch (flag)
		{
		case 0:
			if (sign1) { next = PLACE1; flag = 5; }
			else if (sign2) { next = PLACE2; flag = 6; }
			else if (sign3) { next = PLACE3; flag = 7; }
			else if (sign4) { next = PLACE4; flag = 0; }
			else if (sign5) { next = PLACE5; flag = 1; }
			else if (sign6) { next = PLACE6; flag = 2; }
			else if (sign7) { next = PLACE7; flag = 3; }
			else  { next = last; flag = 4; }
			
			break; // end case 0
		case 1:
			
			if (sign2) { next = PLACE2; flag = 6; }
			else if (sign3) { next = PLACE3; flag = 7; }
			else if (sign4) { next = PLACE4; flag = 0; }
			else if (sign5) { next = PLACE5; flag = 1; }
			else if (sign6) { next = PLACE6; flag = 2; }
			else if (sign7) { next = PLACE7; flag = 3; }
			else if (sign0) { next = PLACE0; flag = 4; }
			else { next = PLACE1; flag = 5; }

			break; // end case 1
		case 2:
			
			
			if (sign3) { next = PLACE3; flag = 7; }
			else if (sign4) { next = PLACE4; flag = 0; }
			else if (sign5) { next = PLACE5; flag = 1; }
			else if (sign6) { next = PLACE6; flag = 2; }
			else if (sign7) { next = PLACE7; flag = 3; }
			else if (sign0) { next = PLACE0; flag = 4; }
			else if (sign1){ next = PLACE1; flag = 5; }
			else{ next = PLACE2; flag = 6; }
			
			break; // end case 2
		case 3:
			
			if (sign4) { next = PLACE4; flag = 0; }
			else if (sign5) { next = PLACE5; flag = 1; }
			else if (sign6) { next = PLACE6; flag = 2; }
			else if (sign7) { next = PLACE7; flag = 3; }
			else if (sign0) { next = PLACE0; flag = 4; }
			else if (sign1) { next = PLACE1; flag = 5; }
			else if (sign2) { next = PLACE2; flag = 6; }
			else  { next = PLACE3; flag = 7; }
			break; // end case 3
		case 4:
			
		
			if (sign5) { next = PLACE5; flag = 1; }
			else if (sign6) { next = PLACE6; flag = 2; }
			else if (sign7) { next = PLACE7; flag = 3; }
			else if (sign0) { next = PLACE0; flag = 4; }
			else if (sign1) { next = PLACE1; flag = 5; }
			else if (sign2) { next = PLACE2; flag = 6; }
			else if (sign3) { next = PLACE3; flag = 7; }
			else { next = PLACE4; flag = 0; }
			break; // end case 4
		case 5:
			
		    if (sign6) { next = PLACE6; flag = 2; }
			else if (sign7) { next = PLACE7; flag = 3; }
			else if (sign0) { next = PLACE0; flag = 4; }
			else if (sign1) { next = PLACE1; flag = 5; }
			else if (sign2) { next = PLACE2; flag = 6; }
			else if (sign3) { next = PLACE3; flag = 7; }
			else if (sign4) { next = PLACE4; flag = 0; }
			else{ next = PLACE5; flag = 1; }
			break; // end case 5
		case 6:
			
			 if (sign7) { next = PLACE7; flag = 3; }
			else if (sign0) { next = PLACE0; flag = 4; }
			else if (sign1) { next = PLACE1; flag = 5; }
			else if (sign2) { next = PLACE2; flag = 6; }
			else if (sign3) { next = PLACE3; flag = 7; }
			else if (sign4) { next = PLACE4; flag = 0; }
			else if (sign5) { next = PLACE5; flag = 1; }
			else  { next = PLACE6; flag = 2; }
			break; // end case 6
		case 7:
			
			if (sign0) { next = PLACE0; flag = 4; }
			else if (sign1) { next = PLACE1; flag = 5; }
			else if (sign2) { next = PLACE2; flag = 6; }
			else if (sign3) { next = PLACE3; flag = 7; }
			else if (sign4) { next = PLACE4; flag = 0; }
			else if (sign5) { next = PLACE5; flag = 1; }
			else if (sign6) { next = PLACE6; flag = 2; }
			else { next = PLACE7; flag = 3; }
			break; // end case 7
		}//end switch
		
		last = now;
		now = next;
				
	}
	now_point.x = first_x;
	now_point.y = first_y;

	temp_contours.push_back(now_point);

	return MY_OK;
}


int Find_Contours(Mat labelImg, vector<vector<Point>> &contours)
{
	if (NULL == labelImg.data)
	{
		cout << "ERROR" << endl;
		return MY_FAIL;
	}
	if (1 != labelImg.channels())
	{
		cout << "wrong channels!" << endl;
		return MY_FAIL;
	}
	if (4 != labelImg.elemSize1())
	{
		cout << "Image elemsize error!" << endl;
		return MY_FAIL;
	}
	//defend finish.

	contours.clear();

	int height = labelImg.rows;
	int width = labelImg.cols;
	int label;

	Mat BigImg(height+2,width+2,CV_32SC1);
	BigImg.setTo(0);

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			((int*)BigImg.data)[(i + 1)*(width + 2) + j + 1] = ((int*)labelImg.data)[i*width + j];

	vector<Point> temp_contours;

	for(int row_i=1;row_i<height+1;row_i++)
		for (int col_j = 1; col_j < width + 1; col_j++)
		{
			int* place = &((int *)BigImg.data)[row_i * (width + 2) + col_j];
			if (0 != *place)
			{
			
				bool sign3 = *(place + 1);
				bool sign4 = *(place + width + 3);						
				bool sign5 = *(place + width + 2);
				bool sign6 = *(place + width + 1);

				int binary = 8 * sign3 + 4 * sign4 + 2 * sign5 + sign6;
				int last_flag = 0;
				switch (binary)
				{
				case 0: 
					*(place) = 0;
					break;
				case 1:
					Find(place, place + width + 1, col_j - 1, row_i - 1,2 ,temp_contours,width,last_flag); 
					contours.push_back(temp_contours); 
					temp_contours.clear();
					label = *place;
					for (int my_row_i = row_i; my_row_i<height + 1; my_row_i++)
						for (int my_col_j = 1; my_col_j < width + 1; my_col_j++)
						{
							int *my_place = &((int *)BigImg.data)[my_row_i*(width + 2) + my_col_j];
							if (label == *my_place)
							{
								*my_place = 0;
							}
						}
					break;
				case 2:
				case 3: 
					Find(place, place + width + 2, col_j - 1, row_i - 1,1, temp_contours,width, last_flag);
					contours.push_back(temp_contours); 
					temp_contours.clear();
					label = *place;
					for (int my_row_i = row_i; my_row_i<height + 1; my_row_i++)
						for (int my_col_j = 1; my_col_j < width + 1; my_col_j++)
						{
							int *my_place = &((int *)BigImg.data)[my_row_i*(width + 2) + my_col_j];
							if (label == *my_place)
							{
								*my_place = 0;
							}
						}
					break;
				case 5:
					last_flag = 1;
				case 4:
				case 6:
				case 7:  
					Find(place, place + width + 3, col_j - 1, row_i - 1,0, temp_contours,width, last_flag);
					contours.push_back(temp_contours); 
					temp_contours.clear();
					label = *place;
					for (int my_row_i = row_i; my_row_i<height + 1; my_row_i++)
						for (int my_col_j = 1; my_col_j < width + 1; my_col_j++)
						{
							int *my_place = &((int *)BigImg.data)[my_row_i*(width + 2) + my_col_j];
							if (label == *my_place)
							{
								*my_place = 0;
							}
						}
					break;
				case 9:
				case 13:
					last_flag = 1;
				default:  
					Find(place, place +1, col_j - 1, row_i - 1,7, temp_contours,width, last_flag);
					contours.push_back(temp_contours); 
					temp_contours.clear();
					label = *place;
					for (int my_row_i = row_i; my_row_i<height + 1; my_row_i++)
						for (int my_col_j = 1; my_col_j < width + 1; my_col_j++)
						{
							int *my_place = &((int *)BigImg.data)[my_row_i*(width + 2) + my_col_j];
							if (label == *my_place)
							{
								*my_place = 0;
							}
						}
					break;
				}
			}
		}
	return MY_OK;
}


int ustc_refresh(int num1, int num2, int len, int *point)//num1 min  mun2 max
{
	num1 = *(point + num1);
	num2 = *(point + num2);
	for (int i = 1; i <= len; i++)
	{
		if (point[i] == num2)
		{
			point[i] = num1;
		}
	}
	return MY_OK;
}




int ustc_ConnectedComponentLabeling(Mat binaryImg, Mat &labelImg, int bin_th)
{
	//defend start:
	if (NULL == binaryImg.data || NULL == labelImg.data)
	{
		cout << "Empty gray image!" << endl;
		return MY_FAIL;
	}
	if (labelImg.cols != binaryImg.cols || labelImg.rows != binaryImg.rows)
	{
		cout << "label and gray image not match!" << endl;
		return MY_FAIL;
	}
	if (1 != binaryImg.channels() || 1 != labelImg.channels())
	{
		cout << "wrong channels!" << endl;
		return MY_FAIL;
	}
	if (bin_th < 0 || bin_th>255)
	{
		cout << "bin_th error!" << endl;
		return MY_FAIL;
	}
	if (1 != binaryImg.elemSize1() || 4 != labelImg.elemSize1())
	{
		cout << "Image elemsize error!" << endl;
		return MY_FAIL;
	}
	//defend finish.
	int marknum[200000] = { 0 };
	int* listPoint = marknum;
	int height = binaryImg.rows, width = binaryImg.cols;
		
	Mat bigImg(height + 2, width + 2, CV_8UC1, Scalar(0));

	for (int rows_i = 1; rows_i <= height; rows_i++)
		for (int cols_j = 1; cols_j <= width; cols_j++)
		{
			bigImg.data[rows_i*(width + 2) + cols_j] = binaryImg.data[(rows_i - 1)*width + cols_j - 1];
		}

	int signNum = 0;
	for (int rows_i = 1; rows_i <= height; rows_i++)
		for (int cols_j = 1; cols_j <= width; cols_j++)
		{
			int bigplace = rows_i*(width + 2) + cols_j;
			int subplace = (rows_i - 1)*width + cols_j - 1;
			if (bigImg.data[bigplace])
			{
				int a = 0, b = 0, c = 0, d = 0;
				uchar *bigpoint = &bigImg.data[bigplace];
				int *subpoint = &((int*)labelImg.data)[subplace];
				d = *(--bigpoint);
				bigpoint -= (width + 2);
				a = *(bigpoint++);
				b = *(bigpoint++);
				c = *(bigpoint);
				int temp = (bool)a * 8 + (bool)b * 4 + (bool)c * 2 + (bool)d;
				a = 0;
				b = 0;
				c = 0;
				d = 0;
				int fb = 0;
				int fc = 0;
				int fd = 0;
				int fa = 0;
				switch (temp)
				{
				case 0:
					*(subpoint) = ++signNum;
					listPoint[signNum] = signNum;
					break;
				case 1:
					d = *(subpoint - 1);
					*(subpoint) = d;
					break;
				case 2:
					c = *(subpoint - width + 1);
					*(subpoint) = c;
					break;
				case 3:
					c = *(subpoint - width + 1);
					d = *(subpoint - 1);
					fc = listPoint[c];
					fd = listPoint[d];
					if (fc == fd)
						*(subpoint) = c;
					else if (fc < fd)
					{
						*(subpoint) = c;
						ustc_refresh(c, d, signNum, listPoint);
					}
					else if (fc > fd)
					{
						*(subpoint) = d;
						ustc_refresh(d, c, signNum, listPoint);
					}
					break;
				case 4:
					b = *(subpoint - width);
					*(subpoint) = b;
					break;
				case 5:
					b = *(subpoint - width);
					d = *(subpoint - 1);
					fb = listPoint[b];
					fd = listPoint[d];
					if (fb == fd)
						*(subpoint) = b;
					else if (fb < fd)
					{
						*(subpoint) = b;
						ustc_refresh(b, d, signNum, listPoint);
					}
					else if (fb> fd)
					{
						*(subpoint) = d;
						ustc_refresh(d, b, signNum, listPoint);
					}
					break;
				case 6:
					b = *(subpoint - width);
					c = *(subpoint - width + 1);
					fb = listPoint[b];
					fc = listPoint[c];
					if (fb == fc)
						*(subpoint) = b;
					else if (fb <fc)
					{
						*(subpoint) = b;
						ustc_refresh(b, c, signNum, listPoint);
					}
					else if (fb > fc)
					{
						*(subpoint) = c;
						ustc_refresh(c, b, signNum, listPoint);
					}
					break;
				case 7:
					b = *(subpoint - width);
					c = *(subpoint - width + 1);
					d = *(subpoint - 1);
					fb = listPoint[b];
					fc = listPoint[c];
					fd = listPoint[d];
					temp = (bool)(fc - fb) * 2 + (bool)(fd - fb);
					switch (temp)
					{
					case 0:
						*(subpoint) = b;
						break;
					case 1:
						if (fd > fb)
						{
							*(subpoint) = b;
							ustc_refresh(b, d, signNum, listPoint);
						}
						else if (fd < fb)
						{
							*(subpoint) = d;
							ustc_refresh(d, b, signNum, listPoint);
						}
						break;
					case 2:
					case 3:
						if (fc > fb)
						{
							*(subpoint) = b;
							ustc_refresh(b, c, signNum, listPoint);
						}
						else if (fc < fb)
						{
							*(subpoint) = c;
							ustc_refresh(c, b, signNum, listPoint);
						}
						break;
					}
					break;
				case 8:
					a = *(subpoint - width - 1);
					*(subpoint) = a;
					break;
				case 9:
					a = *(subpoint - width - 1);
					d = *(subpoint - 1);
					fd = listPoint[d];
					fa = listPoint[a];
					if (fa == fd)
						*(subpoint) = a;
					else if (fa < fd)
					{
						*(subpoint) = a;
						ustc_refresh(a, d, signNum, listPoint);
					}
					else if (fa > fd)
					{
						*(subpoint) = d;
						ustc_refresh(d, a, signNum, listPoint);
					}
					break;
				case 10:
					a = *(subpoint - width - 1);
					c = *(subpoint - width + 1);
					fc = listPoint[c];
					fa = listPoint[a];
					if (fa == fc)
						*(subpoint) = a;
					else if (fa < fc)
					{
						*(subpoint) = a;
						ustc_refresh(a, c, signNum, listPoint);
					}
					else if (fa> fc)
					{
						*(subpoint) = c;
						ustc_refresh(c, a, signNum, listPoint);
					}
					break;
				case 11:
					a = *(subpoint - width - 1);
					c = *(subpoint - width + 1);
					d = *(subpoint - 1);
					fc = listPoint[c];
					fd = listPoint[d];
					fa = listPoint[a];
					temp = (bool)(fc - fa) * 2 + (bool)(fd - fa);
					switch (temp)
					{
					case 0:
						*(subpoint) = a;
						break;
					case 1:
						if (fd > fa)
						{
							*(subpoint) = a;
							ustc_refresh(a, d, signNum, listPoint);
						}
						else if (fd < fa)
						{
							*(subpoint) = d;
							ustc_refresh(d, a, signNum, listPoint);
						}
						break;
					case 2:
					case 3:
						if (fc > fa)
						{
							*(subpoint) = a;
							ustc_refresh(a, c, signNum, listPoint);
						}
						else if (fc < fa)
						{
							*(subpoint) = c;
							ustc_refresh(c, a, signNum, listPoint);
						}
						break;
					}
					break;
				case 12:
					a = *(subpoint - width - 1);
					b = *(subpoint - width);
					fb = listPoint[b];
					fa = listPoint[a];
					if (fa == fb)
						*(subpoint) = a;
					else if (fa <fb)
					{
						*(subpoint) = a;
						ustc_refresh(a, b, signNum, listPoint);
					}
					else if (fa > fb)
					{
						*(subpoint) = b;
						ustc_refresh(b, a, signNum, listPoint);
					}
					break;
				case 13:
					a = *(subpoint - width - 1);
					b = *(subpoint - width);
					d = *(subpoint - 1);
					fb = listPoint[b];
					fd = listPoint[d];
					fa = listPoint[a];
					temp = (bool)(fb - fa) * 2 + (bool)(fd - fa);
					switch (temp)
					{
					case 0:
						*(subpoint) = a;
						break;
					case 1:
						if (fd > fa)
						{
							*(subpoint) = a;
							ustc_refresh(a, d, signNum, listPoint);
						}
						else if (fd < fa)
						{
							*(subpoint) = d;
							ustc_refresh(d, a, signNum, listPoint);
						}
						break;
					case 2:
					case 3:
						if (fb > fa)
						{
							*(subpoint) = a;
							ustc_refresh(a, b, signNum, listPoint);
						}
						else if (fb < fa)
						{
							*(subpoint) = b;
							ustc_refresh(b, a, signNum, listPoint);
						}
						break;
					}
					break;
				case 14:
					a = *(subpoint - width - 1);
					b = *(subpoint - width);
					c = *(subpoint - width + 1);
					fb = listPoint[b];
					fc = listPoint[c];
					fa = listPoint[a];
					temp = (bool)(fb - fa) * 2 + (bool)(fc - fa);
					switch (temp)
					{
					case 0:
						*(subpoint) = a;
						break;
					case 1:
						if (fc > fa)
						{
							*(subpoint) = a;
							ustc_refresh(a, c, signNum, listPoint);
						}
						else if (fc < fa)
						{
							*(subpoint) = c;
							ustc_refresh(c, a, signNum, listPoint);
						}
						break;
					case 2:
					case 3:
						if (fb > fa)
						{
							*(subpoint) = a;
							ustc_refresh(a, b, signNum, listPoint);
						}
						else if (fb < fa)
						{
							*(subpoint) = b;
							ustc_refresh(b, a, signNum, listPoint);
						}
						break;
					}
					break;
				case 15:
					a = *(subpoint - width - 1);
					b = *(subpoint - width);
					c = *(subpoint - width + 1);
					d = *(subpoint - 1);
					fb = listPoint[b];
					fc = listPoint[c];
					fd = listPoint[d];
					fa = listPoint[a];
					temp = (bool)(fb - fa) * 4 + (bool)(fc - fa) * 2 + (bool)(fd - fa);
					switch (temp)
					{
					case 0:
						*(subpoint) = a;
						break;
					case 1:
					case 3:
					case 5:
					case 7:
						if (fa<fd)
						{
							*(subpoint) = a;
							ustc_refresh(a, d, signNum, listPoint);
						}
						if (fa>fd)
						{
							*(subpoint) = d;
							ustc_refresh(d, a, signNum, listPoint);
						}
						break;
					case 2:
					case 6:
						if (fa<fc)
						{
							*(subpoint) = a;
							ustc_refresh(a, c, signNum, listPoint);
						}
						if (fa>fc)
						{
							*(subpoint) = c;
							ustc_refresh(c, a, signNum, listPoint);
						}
						break;
					case 4:
						if (fa<fb)
						{
							*(subpoint) = a;
							ustc_refresh(a, b, signNum, listPoint);
						}
						if (fa>fb)
						{
							*(subpoint) = b;
							ustc_refresh(b, a, signNum, listPoint);
						}
						break;
					}
					break;
				}
			}
		}

	for (int rows_i = 0; rows_i < height; rows_i++)
		for (int cols_j = 0; cols_j < width; cols_j++)
		{
			int val = ((int*)labelImg.data)[rows_i*width + cols_j];
			switch (val)
			{
			case 0:
				break;
			default:
				((int*)labelImg.data)[rows_i*width + cols_j] = *(listPoint + val);
			}
		}
	return MY_OK;
}//end ustc_ConnectedComponentLabeling
