#include "FindContours.h"

//#define NUMSHOW

//找连通域  grayImg: 二值化图  labelImg: 扩充的标签图
int ustc_ConnectedComponentLabeling(Mat grayImg, Mat& labelImg);
//二值图扩充
int gray_expand2_binary(const Mat &grayImg, Mat &binaryImg);


int USTC_Find_Contours(Mat binaryImg, vector< vector< cv::Point > > &contours)
{
	int width = binaryImg.cols;
	int height = binaryImg.rows;
	
	//建0,1二值图
	Mat binaryImg_01(height, width, CV_8UC1);
	uchar *binaryImg_data = binaryImg.data;
	for (int row_i = 0; row_i < height; row_i++)
	{
		int temp0 = row_i * width;
		for (int col_j = 0; col_j < width; col_j += 1)
		{
			int temp1 = temp0 + col_j;
			binaryImg_01.data[temp1] = binaryImg_data[temp1] >> 7;
		}
	}
	//binaryImg_01 = binaryImg / 255;
	
	//建标签图
	cv::Mat labelImg = cv::Mat::zeros(height + 2, width + 2, CV_32SC1);
	int *labelImg_data = (int*)labelImg.data;
	//产生标签图
	ustc_ConnectedComponentLabeling(binaryImg_01, labelImg);
#ifdef NUMSHOW
	cout << "\n\n";
	cout << "---------------------------------------------------------------------------------------------------------------\n";
	for (int i = 0; i < labelImg.rows; i++)
	{
		for (int j = 0; j < labelImg.cols; j++)
		{
			cout.width(2);
			cout << ((int*)labelImg.data)[i * labelImg.cols + j];
		}
		cout << endl;
	}
#endif


	width = labelImg.cols;
	height = labelImg.rows;
	contours.clear();
	//存当前轮廓
	vector< cv::Point > Now_contour;
	Now_contour.clear();
	int label_th = 0;
	int search_vec_x[8] = { 1,  1,  0, -1, -1, -1,  0,  1 };
	int search_vec_y[8] = { 0, -1, -1, -1,  0,  1,  1,  1 };
	
	int trans_pst[8] = { 4, 5, 6, 7, 
		                 0, 1, 2, 3 };

	int index_limit = width * height - width;
	for (int index = width; index < index_limit; index++)
	{
		//找到轮廓的第一个点
		if (labelImg_data[index] > label_th)
		{
			Now_contour.clear();
			int y = index / width;
			int x = index % width;
			cv::Point Now_point(0, 0);
			int now_pst = 7;
			int next_pst = -1;
			int search_pst;

			//建周围点的指针数组
			int* center_pointer = labelImg_data + index;
			int* index_pointer_1[4] = { center_pointer - 1 + width, center_pointer + width, center_pointer + 1 + width,
			                            center_pointer + 1 };
			//找第二个点
			for (int i = 0; i < 4; i++)
			{
				if (*(index_pointer_1[i]) > label_th)
				{
					next_pst = (i + 5) % 8;
					break;
				}
			}
			//如果没找到第二个点
			if (-1 == next_pst)
			{
				label_th = labelImg_data[index];
			}

			//找到第二个点，即这个轮廓成立
			else if (next_pst != -1)
			{
				//判断起始位置要经过几次
				int startpoint_visit_times = 1;
				int q = 1;
				int mark_sum = 0;
				for (int i = 3; i >= 0; i--)
				{
					if (*(index_pointer_1[i]) > label_th)
					{
						mark_sum += q;
					}
					q = q << 1;
				}
				if (mark_sum >= 9 && mark_sum <= 11)
				{
					startpoint_visit_times = 2;
				}

				//当前标签值
				int now_label = labelImg_data[index];
				//起始点压入当前轮廓
				cv::Point Head_point(x - 1, y - 1);
				Now_contour.push_back(Head_point);
				//第二个点压入当前轮廓
				y = y + search_vec_y[next_pst];
				x = x + search_vec_x[next_pst];
				Now_point.x = x - 1;
				Now_point.y = y - 1;
				Now_contour.push_back(Now_point);

				now_pst = next_pst;
				next_pst = -1;
				while (1)
				{
					//建周围点的指针数组
					int* center_pointer = labelImg_data + y * width + x;
					int* index_pointer[8] = { center_pointer + 1,
						                      center_pointer + 1 - width, center_pointer - width, center_pointer - 1 - width,
						                      center_pointer - 1,
						                      center_pointer - 1 + width, center_pointer + width, center_pointer + 1 + width };
					for (int i = 0; i < 5; i++)
					{
						search_pst = (now_pst + 6 + i) % 8;
						if (*(index_pointer[search_pst]) > label_th)
						{
							next_pst = search_pst;
							break;
						}
					}
			        //如果到轮廓尖部，则掉头
					if (-1 == next_pst)
					{
						now_pst = trans_pst[now_pst];
						for (int i = 0; i < 5; i++)
						{
							search_pst = (now_pst + 6 + i) % 8;
							if (*(index_pointer[search_pst]) > label_th)
							{
								next_pst = search_pst;
								break;
							}
						}
					}
		
					y = y + search_vec_y[next_pst];
					x = x + search_vec_x[next_pst];
					Now_point.x = x - 1;
					Now_point.y = y - 1;
					//如果回到起始点
					if (Now_point == Head_point) 
					{
						startpoint_visit_times--;
						if (0 == startpoint_visit_times)
						{
							label_th = now_label;
							break;
						}
					}
					//当前点压入轮廓
					Now_contour.push_back(Now_point);
					//准备下轮循环
					now_pst = next_pst;
					next_pst = -1;
				}
				contours.push_back(Now_contour);
			}
		}
	}

	return MY_OK;
}

//将二值化图转化为标签图
int ustc_ConnectedComponentLabeling(Mat grayImg, Mat& labelImg)
{
	if (NULL == grayImg.data || NULL == labelImg.data)
	{
		std::cout << "exist null image!\n";
		return MY_FAIL;
	}
	if (grayImg.rows + 2 != labelImg.rows || grayImg.cols + 2 != labelImg.cols)
	{
		std::cout << "sizes of grayImg and labelImg don't match!\n";
		return MY_FAIL;
	}

	int *data_label = (int*)labelImg.data;

	Mat binaryImg(grayImg.rows + 2, grayImg.cols + 2, CV_8UC1);//建立扩充的二值图
	gray_expand2_binary(grayImg, binaryImg);//二值图扩充
	

	int i_local = 0;
	int i_row = 0, j_col = 0;
	int row_scan = binaryImg.rows - 1;
	int col_scan = binaryImg.cols - 1;
	int bi_row = binaryImg.rows;
	int bi_col = binaryImg.cols;
	//********************************** 1_st time scan **********************************
	//scanner
	//bcd
	//a
	int length = binaryImg.rows * binaryImg.cols / 4;//标签最大可能个数
	int *mark = new int[length];
	int *link = new int[length];
	memset(mark, 0, length * sizeof(int));
	memset(link, 0, length * sizeof(int));
	int max_mark = 0;
	int scanner = 0;
	int b = 0, c = 0, d = 0;
	int a = 0, e = 0;

	int b_index = 0, c_index = 0, d_index = 0;
	int a_index = 0, e_index = 0;
	int b_mark = 0, c_mark = 0, d_mark = 0;
	int a_mark = 0, e_mark = 0;

	int link_min = 0, link_max = 0, link_tmp = 0;

	int tmp_curt_row = 0;
	int tmp_pre_row = 0;
	for (i_row = 1; i_row < row_scan; i_row++)
	{
		tmp_curt_row = i_row * binaryImg.cols;
		tmp_pre_row = (i_row - 1) * binaryImg.cols;
		for (j_col = 1; j_col < col_scan; j_col++)
		{
			e_index = tmp_curt_row + j_col;
			e = binaryImg.data[e_index];
			if (!e) //background
			{
				data_label[e_index] = 0;
			}
			else  //forthground
			{
				a_index = tmp_curt_row + j_col - 1;
				b_index = tmp_pre_row + j_col - 1;
				c_index = tmp_pre_row + j_col;
				d_index = tmp_pre_row + j_col + 1;

				a = binaryImg.data[a_index];
				b = binaryImg.data[b_index];
				c = binaryImg.data[c_index];
				d = binaryImg.data[d_index];
				scanner = (a << 3) + (b << 2) + (c << 1) + d;
				switch (scanner)
				{
				case 0://0000
					max_mark++;
					data_label[e_index] = max_mark;
					mark[max_mark] = max_mark;
					link[max_mark] = max_mark;
					break;
				case 1://0001  d   //e = d
					d_mark = data_label[d_index];
					data_label[e_index] = d_mark;
					break;
				case 2://0010  c   //e = c
					c_mark = data_label[c_index];
					data_label[e_index] = c_mark;
					break;
				case 3://0011  cd  d <= c   //e = d
					d_mark = data_label[d_index];
					data_label[e_index] = d_mark;
					break;
				case 4://0100  b   //e = b
					b_mark = data_label[b_index];
					data_label[e_index] = b_mark;
					break;
				case 5://0101  bd   //connect b & d
					b_mark = data_label[b_index];
					d_mark = data_label[d_index];
					if (b_mark == d_mark)
					{
						data_label[e_index] = b_mark;
					}
					else //b_mark != d_mark
					{
						if (b_mark < d_mark)
						{
							data_label[e_index] = b_mark;
						}
						else
						{
							data_label[e_index] = d_mark;
						}
						//connect b_link & d_link
						link_min = link[b_mark];
						link_max = link[d_mark];
						if (link_min == link_max)
						{
							//nothing to do
						}
						else if (link_max > link_min)
						{
							link[link_max] = link[link_min];
							for (i_local = link_max + 1; i_local <= max_mark; i_local++)
							{
								link[i_local] = link[link[i_local]];
							}
						}
						else //link_max < link_min 
						{
							link_tmp = link_min;
							link_min = link_max;
							link_max = link_tmp;
							link[link_max] = link[link_min];
							for (i_local = link_max + 1; i_local <= max_mark; i_local++)
							{
								link[i_local] = link[link[i_local]];
							}
						}
					}
					break;
				case 6://0110  bc  c <= b   //e = c
					c_mark = data_label[c_index];
					data_label[e_index] = c_mark;
					break;
				case 7://0111  bcd  d <= c <= b   //e = d
					d_mark = data_label[d_index];
					data_label[e_index] = d_mark;
					break;
				case 8://1000  a   //e = a
					a_mark = data_label[a_index];
					data_label[e_index] = a_mark;
					break;
				case 9://1001  ad  connect a & d
					a_mark = data_label[a_index];
					d_mark = data_label[d_index];
					if (a_mark == d_mark)
					{
						data_label[e_index] = a_mark;
					}
					else
					{
						if (a_mark < d_mark)
						{
							data_label[e_index] = a_mark;
						}
						else
						{
							data_label[e_index] = d_mark;
						}
						link_min = link[a_mark];
						link_max = link[d_mark];
						if (link_min == link_max)
						{
							//nothing to do
						}
						else if (link_max > link_min)
						{
							link[link_max] = link[link_min];
							for (i_local = link_max + 1; i_local <= max_mark; i_local++)
							{
								link[i_local] = link[link[i_local]];
							}
						}
						else //link_max < link_min
						{
							link_tmp = link_min;
							link_min = link_max;
							link_max = link_tmp;
							link[link_max] = link[link_min];
							for (i_local = link_max + 1; i_local <= max_mark; i_local++)
							{
								link[i_local] = link[link[i_local]];
							}
						}
					}
					break;
				case 10://1010  ac  a <= c   //e = a
					a_mark = data_label[a_index];
					data_label[e_index] = a_mark;
					break;
				case 11://1011  acd  a <= c, d <= c
					a_mark = data_label[a_index];
					d_mark = data_label[d_index];
					if (a_mark <= d_mark)
					{
						data_label[e_index] = a_mark;
					}
					else
					{
						data_label[e_index] = d_mark;
					}
					break;
				case 12://1100  ab  a <= b   //e = a
					a_mark = data_label[a_index];
					data_label[e_index] = a_mark;
					break;
				case 13://1101  abd  //a <= b   connect a & d
					a_mark = data_label[a_index];
					d_mark = data_label[d_index];
					if (a_mark == d_mark)
					{
						data_label[e_index] = a_mark;
					}
					else
					{
						if (a_mark < d_mark)
						{
							data_label[e_index] = a_mark;
						}
						else
						{
							data_label[e_index] = d_mark;
						}
						link_min = link[a_mark];
						link_max = link[d_mark];
						if (link_min == link_max)
						{
							//nothing to do
						}
						else if (link_max > link_min)
						{
							link[link_max] = link[link_min];
							for (i_local = link_max + 1; i_local <= max_mark; i_local++)
							{
								link[i_local] = link[link[i_local]];
							}
						}
						else //link_max < link_min
						{
							link_tmp = link_min;
							link_min = link_max;
							link_max = link_tmp;
							link[link_max] = link[link_min];
							for (i_local = link_max + 1; i_local <= max_mark; i_local++)
							{
								link[i_local] = link[link[i_local]];
							}
						}
					}
					break;
				case 14://1110  abc  a <= b, a <= c   //e = a
					a_mark = data_label[a_index];
					data_label[e_index] = a_mark;
					break;
				case 15://1111  abcd  //compare a d
					a_mark = data_label[a_index];
					d_mark = data_label[d_index];
					if (a_mark <= d_mark)
					{
						data_label[e_index] = a_mark;
					}
					else
					{
						data_label[e_index] = d_mark;
					}
					break;
				}
			}
		}
	}
	//*********************************** end of 1_st time scan ******************************

	//*********************************** 2_nd time scan *************************************
	for (i_row = row_scan - 1; i_row > 0; i_row--)
	{
		tmp_curt_row = i_row * binaryImg.cols;
		for (j_col = col_scan - 1; j_col > 0; j_col--)
		{
			e_index = tmp_curt_row + j_col;
			e_mark = data_label[e_index];
			data_label[e_index] = link[e_mark];
		}
	}
	//********************************** end of 2_nd time scan *******************************

	delete[] mark;
	delete[] link;
	return MY_OK;
}

//二值图扩充
int gray_expand2_binary(const Mat &grayImg, Mat &binaryImg)
{
	int i = 0, j = 0;
	int i_gray = 0, j_gray = 0;
	int i_binary = 0, j_binary = 0;
	int gray_row = grayImg.rows;
	int gray_col = grayImg.cols;
	int tmp_crt_row_gray = 0;
	int tmp_crt_row_bi = 0;

	int j_quotient = 0;
	int j_remainder = 0;
	for (i_gray = gray_row - 1, i_binary = gray_row; i_gray >= 0; i_gray--, i_binary--)
	{
		tmp_crt_row_bi = i_binary * binaryImg.cols;
		tmp_crt_row_gray = i_gray * gray_col;
		j_quotient = grayImg.cols >> 2;
		j_remainder = grayImg.cols - (j_quotient << 2);
		for (j_gray = gray_col - 1, j_binary = gray_col; j_quotient >= 0; j_quotient--)
		{
			binaryImg.data[tmp_crt_row_bi + j_binary] = grayImg.data[tmp_crt_row_gray + j_gray];  j_gray--; j_binary--;
			binaryImg.data[tmp_crt_row_bi + j_binary] = grayImg.data[tmp_crt_row_gray + j_gray];  j_gray--; j_binary--;
			binaryImg.data[tmp_crt_row_bi + j_binary] = grayImg.data[tmp_crt_row_gray + j_gray];  j_gray--; j_binary--;
			binaryImg.data[tmp_crt_row_bi + j_binary] = grayImg.data[tmp_crt_row_gray + j_gray];  j_gray--; j_binary--;
		}
		for (; j_remainder >= 0; j_remainder--)
		{
			binaryImg.data[tmp_crt_row_bi + j_binary] = grayImg.data[tmp_crt_row_gray + j_gray];  j_gray--; j_binary--;
		}
	}
	//二值化图头尾两行置零
	i = (binaryImg.rows - 1) * binaryImg.cols;
	for (j = binaryImg.cols - 1; j >= 0; j--)
	{
		binaryImg.data[j] = 0;
		binaryImg.data[i + j] = 0;
	}
	//二值化图头尾两列置零
	j = binaryImg.cols - 1;
	for (i = binaryImg.rows - 1; i >= 0; i--)
	{
		tmp_crt_row_bi = i * binaryImg.cols;
		binaryImg.data[tmp_crt_row_bi] = 0;
		binaryImg.data[tmp_crt_row_bi + j] = 0;
	}

	return MY_OK;
}
