#include "FindContours.h"
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#define VERYBIG 65535

int update_result(int*result, int oldone, int newone, int m) {
	for (int i = 0; i <= m; i++) {
		if (result[i] == oldone) {
			result[i] = newone;
		}
	}
	return MY_OK;
}
int pick_min2(int a, int b) {
	int min;
	if (a > b) {
		min = b;
	}
	else {
		min = a;
	}
	return min;
}
int pick_min3(int a, int b, int c) {
	int min1;
	int min;
	if (a > b) {
		min1 = b;
	}
	else {
		min1 = a;
	}
	if (min1>c) {
		min = c;
	}
	else {
		min = min1;
	}
	return min;
}
int pick_min4(int a, int b, int c, int d) {
	int min1;
	int min2;
	int min;
	if (a > b) {
		min1 = b;
	}
	else {
		min1 = a;
	}
	if (c > d) {
		min2 = d;
	}
	else {
		min2 = c;
	}
	if (min1 > min2) {
		min = min2;
	}
	else {
		min = min1;
	}
	return min;
}

int ustc_ConnectedComponentLabeling(Mat grayImg, Mat& labelImg, int bin_th) {
	if (NULL == grayImg.data || NULL == labelImg.data || bin_th>255 || bin_th<0) {
		cout << "wrong." << endl;
		return MY_FAIL;
	}
	int cols = grayImg.cols;
	int rows = grayImg.rows;
	int lenth = cols*rows / 4;
	int *label = new int[lenth];
	int *result = new int[lenth];
	memset(label, -1, lenth);
	memset(result, -1, lenth);

	//标记两个数组中的位置
	int k = 0;
	int w = 0;

	Mat threImg(rows, cols, CV_8UC1);
	threshold(grayImg, threImg, bin_th, 255, THRESH_BINARY);
	int new_rows = rows + 2;
	int new_cols = cols + 2;
	Mat testImg(new_rows, new_cols, CV_32SC1);
	testImg.setTo(0);
	int rr = rows + 1;
	int cc = cols + 1;
	int*t_data = (int*)testImg.data;
	uchar*data = threImg.data;
	for (int i = 1; i < rr; i++) {
		int flag1 = i * new_cols;
		int flag1_s = (i - 1)*cols;
		for (int j = 1; j < cc; j++) {
			int flag2 = flag1 + j;
			int flag2_s = flag1_s + j - 1;
			t_data[flag2] = (int)data[flag2_s];
		}
	}
	int curr_max_label = 0;
	for (int i = 1; i < rr; i++) {
		int flag = (i - 1)*new_cols;
		for (int j = 1; j < cc; j++) {
			int loc_a = flag + j - 1;
			int loc_e = loc_a + new_cols + 1;
			int a = t_data[loc_a];
			int b = t_data[loc_a + 1];
			int c = t_data[loc_a + 2];
			int d = t_data[loc_e - 1];
			int e = t_data[loc_e];
			switch (e) {
			case 0:break;
			default:
				int bianma = (a & 1) * 8 + (b & 1) * 4 + (c & 1) * 2 + (d & 1);
				int flagg = 0;
				int mydefine = 0;
				switch (bianma) {
				case 0:curr_max_label++;
					label[w++] = curr_max_label;
					result[k++] = curr_max_label;
					t_data[loc_e] = curr_max_label;
					break;
				case 1:t_data[loc_e] = d;
					break;
				case 2:t_data[loc_e] = c;
					break;
				case 4:t_data[loc_e] = b;
					break;
				case 8:t_data[loc_e] = a;
					break;
				case 3:flagg = pick_min2(d, c);
					t_data[loc_e] = flagg;
					mydefine = ((d - flagg) || 0)*d + ((c - flagg) || 0)*c;
					update_result(result, mydefine, flagg, k);
					break;
				case 5: flagg = pick_min2(b, d);
					t_data[loc_e] = flagg;
					mydefine = ((b - flagg) || 0)*b + ((d - flagg) || 0)*d;
					update_result(result, mydefine, flagg, k);
					break;
				case 12: flagg = pick_min2(a, b);
					t_data[loc_e] = flagg;
					mydefine = ((a - flagg) || 0)*a + ((b - flagg) || 0)*b;
					update_result(result, mydefine, flagg, k);
					break;
				case 6:flagg = pick_min2(b, c);
					t_data[loc_e] = flagg;
					mydefine = ((b - flagg) || 0)*b + ((c - flagg) || 0)*c;
					//printf("%d %d\n", flagg, mydefine);
					update_result(result, mydefine, flagg, k);
					break;
				case 9:flagg = pick_min2(a, d);
					t_data[loc_e] = flagg;
					mydefine = ((a - flagg) || 0)*a + ((d - flagg) || 0)*d;
					//printf("%d %d\n", flagg, mydefine);
					update_result(result, mydefine, flagg, k);
					break;
				case 10: flagg = pick_min2(a, c);
					t_data[loc_e] = flagg;
					mydefine = ((a - flagg) || 0)*a + ((c - flagg) || 0)*c;
					//printf("%d %d\n", flagg, mydefine);
					update_result(result, mydefine, flagg, k);
					break;
				case 7: flagg = pick_min3(b, c, d);
					t_data[loc_e] = flagg;
					//不是最小值的result更新
					if (b == flagg) {
						update_result(result, c, flagg, k);
						update_result(result, d, flagg, k);
					}
					else if (c == flagg) {
						update_result(result, b, flagg, k);
						update_result(result, d, flagg, k);
					}
					else if (d == flagg) {
						update_result(result, b, flagg, k);
						update_result(result, c, flagg, k);
					}
					break;
				case 11: flagg = pick_min3(a, c, d);
					t_data[loc_e] = flagg;
					//不是最小值的result更新
					if (a == flagg) {
						update_result(result, c, flagg, k);
						update_result(result, d, flagg, k);
					}
					else if (c == flagg) {
						update_result(result, a, flagg, k);
						update_result(result, d, flagg, k);
					}
					else if (d == flagg) {
						update_result(result, a, flagg, k);
						update_result(result, c, flagg, k);
					}
					break;
				case 13:flagg = pick_min3(b, a, d);
					t_data[loc_e] = flagg;
					//不是最小值的result更新
					if (b == flagg) {
						update_result(result, a, flagg, k);
						update_result(result, d, flagg, k);
					}
					else if (a == flagg) {
						update_result(result, b, flagg, k);
						update_result(result, d, flagg, k);
					}
					else if (d == flagg) {
						update_result(result, b, flagg, k);
						update_result(result, a, flagg, k);
					}
					break;
				case 14:flagg = pick_min3(b, c, a);
					t_data[loc_e] = flagg;
					//不是最小值的result更新
					if (b == flagg) {
						update_result(result, c, flagg, k);
						update_result(result, a, flagg, k);
					}
					else if (c == flagg) {
						update_result(result, b, flagg, k);
						update_result(result, a, flagg, k);
					}
					else if (a == flagg) {
						update_result(result, b, flagg, k);
						update_result(result, c, flagg, k);
					}
					break;
				case 15: flagg = pick_min4(a, b, c, d);
					t_data[loc_e] = flagg;
					//不是最小值的result更新
					if (b == flagg) {
						update_result(result, c, flagg, k);
						update_result(result, d, flagg, k);
						update_result(result, a, flagg, k);
					}
					else if (c == flagg) {
						update_result(result, b, flagg, k);
						update_result(result, d, flagg, k);
						update_result(result, a, flagg, k);
					}
					else if (d == flagg) {
						update_result(result, b, flagg, k);
						update_result(result, c, flagg, k);
						update_result(result, a, flagg, k);
					}
					else if (a == flagg) {
						update_result(result, b, flagg, k);
						update_result(result, c, flagg, k);
						update_result(result, d, flagg, k);
					}
					break;
				}
			}
		}
	}
	for (int i = 0; i < k; i++) {
		int dd = result[i];
		int kk = result[dd - 1];
		if (dd != kk) {
			result[i] = kk;
		}
	}
	//再次遍历图像实现正确赋值，连通域建成
	for (int i = 1; i < rr; i++) {
		int flag = i*new_cols;
		for (int j = 1; j < cc; j++) {
			for (int f = 0; f < w; f++) {
				int dat = t_data[flag + j];
				int lab = label[f];
				int res = result[f];
				if (dat == lab) {
					t_data[flag + j] = res;
				}
			}
		}
	}
	int*label_data = (int*)labelImg.data;
	for (int i = 0; i < rows; i++) {
		int flag_a1 = i*cols;
		int flag_a2 = (i + 1)*new_cols;
		for (int j = 0; j < cols; j++) {
			int flag_b1 = flag_a1 + j;
			int flag_b2 = flag_a2 + j + 1;
			//uchar dataa = (uchar)((float)t_data[flag_b2]/VERYBIG*255);
			int dataa = t_data[flag_b2];
			label_data[flag_b1] = dataa;
		}
	}
	delete label;
	delete result;
	return MY_OK;
}
//liantongyu function end

void find_linyu(Mat BiglabelImg,int linyu[8],Point now) {
	int*t_data = (int*)BiglabelImg.data;
	int x = now.x;
	int y = now.y;
	int height = BiglabelImg.rows;
	int width = BiglabelImg.cols;
	int bpos = (y - 1)*width + x;
	int pos = y*width + x;
	int fpos = (y + 1)*width + x;
	linyu[0] = t_data[bpos - 1];
	linyu[1] = t_data[bpos];
	linyu[2] = t_data[bpos + 1];
	linyu[3] = t_data[pos+1];
	linyu[7] = t_data[pos - 1];
	linyu[4] = t_data[fpos + 1];
	linyu[5] = t_data[fpos];
	linyu[6] = t_data[fpos - 1];
}

void find_pos(int lin,Point hh,int &xx,int &yy) {
	int i = hh.y;
	int j = hh.x;
	switch (lin) {
	  case 0: {
			xx = j - 1;
			yy = i - 1;
			break;
		}
	  case 1: {
		  xx = j;
		  yy = i - 1;
		  break;
	  }
	  case 2: {
		  xx = j + 1;
		  yy = i - 1;
		  break;
	  }
	  case 3: {
		  xx = j + 1;
		  yy = i;
		  break;
	  }
	  case 4: {
		  xx = j + 1;
		  yy = i + 1;
		  break;
	  }
	  case 5: {
		  xx = j;
		  yy = i+1;
		  break;
	  }
	  case 6: {
		  xx = j - 1;
		  yy = i + 1;
		  break;
	  }
	  case 7: {
		  xx = j - 1;
		  yy = i;
		  break;
	  }
	}
}

int find_last(Mat BiglabelImg,Point last,Point now) {
	int last_i = last.y;
	int last_j = last.x;
	int now_i = now.y;
	int now_j = now.x;
	int dx = last_i - now_i;
	int dy = last_j - now_j;
	switch (dx) {
	   case -1:{
			if (dy == -1) return 0;
			else if (dy == 0) return 1;
			else return 2;
		}
	   case 0: {
		   if (dy == -1) return 7;
		   else return 3;
	   }
	   case 1: {
		   if (dy == -1) return 6;
		   else if (dy == 0) return 5;
		   else return 4;
	   }
	}
}

int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours) {
   if (NULL == binaryImg.data) {
		cout << "wrong." << endl;
		return MY_FAIL;
	}
	int height = binaryImg.rows;
	int width = binaryImg.cols;
	Mat labelImg(height, width, CV_32SC1);
	int bin_th = 100;
	ustc_ConnectedComponentLabeling(binaryImg, labelImg, bin_th);
	contours.clear();

	int new_height = height + 2;
	int new_width = width + 2;
	Mat BiglabelImg(new_height, new_width, CV_32SC1);
	BiglabelImg.setTo(0);
	int rr = height + 1;
	int cc = width + 1;
	int*t_data = (int*)BiglabelImg.data;
	int*data = (int *)labelImg.data;
	for (int i = 1; i < rr; i++) {
		int flag1 = i * new_width;
		int flag1_s = (i - 1)*width;
		for (int j = 1; j < cc; j++) {
			int flag2 = flag1 + j;
			int flag2_s = flag1_s + j - 1;
			t_data[flag2] = data[flag2_s];
		}
	}
	int label[VERYBIG] = { 0 };
	label[0] = 1;//qiangxingpaichu
	int linyu[8] = { 0 };
	vector<cv::Point> curr_contours;
	curr_contours.clear();
	Point head;
	//int num = 0;
	//int len = -1;
	int flag1 = 0;
	int sub_height = new_height - 1;
	int sub_width = new_width - 1;
	//start ergodic,find first point
	for (int i = 1; i <sub_height; i++) {
		int pos = i*new_width;
		for (int j = 1; j < sub_width; j++) {
			int position = pos + j;
			int la = t_data[position];
			if (label[la] != 0);
			else {//find a new label
				label[la] = 1;
				head.x = j;
				head.y = i;
				curr_contours.push_back(head);

				//find second point
				int fpos = position + new_width;
				int bpos = position - new_width;
				int flag = 0;
				Point sec;
				find_linyu(BiglabelImg, linyu, head);
				for (int lin = 0; lin < 8; lin++) {
					if (linyu[lin] == la) {
						int xx = 0;
						int yy = 0;
						find_pos(lin, head, xx, yy);
						sec.x = xx;
						sec.y = yy;
						curr_contours.push_back(sec);
						//cout << curr_contours[1] << endl;
						flag = 1;
						break;
					}
				}
				switch (flag) {
				case 0: {
					curr_contours.pop_back();
					for (int i_in = 1; i_in < sub_height; i_in++) {
						int pos = i_in*new_width;
						for (int j_in = 1; j_in < sub_width; j_in++) {
							if (t_data[pos + j_in] == la) {
								t_data[pos + j_in] = 0;
							}
						}
					}
					break;
				}
				case 1: {
					//xunhuan zhao qitadian
					Point last = head;
					Point now = sec;
					//num = 2;
					while (1) {
						find_linyu(BiglabelImg, linyu, now);
						int lastpos = find_last(BiglabelImg, last, now);
						for (int lin = 0; lin < 8; lin++) {
							int weizhi = (lastpos + lin + 1) % 8;
							if (linyu[weizhi] == la) {
								int xx = 0;
								int yy = 0;
								find_pos(weizhi, now, xx, yy);
								last.x = now.x;
								last.y = now.y;
								now.x = xx;
								now.y = yy;
								curr_contours.push_back(now);
								int inpos = (yy - 1)*width + xx - 1;
								data[inpos] = 255.0;
								//num++;
								//cout << curr_contours[num - 1] << endl;
								break;
							}
						}
						if ((now.x == head.x) && (now.y == head.y)) {
							//len++;
							flag1 = 1;
							break;
						}
					}
					break;
				}
				}
						if (flag1 == 1) {
							contours.push_back(curr_contours);
							//num = 0;
							flag1 = 0;
						}
						curr_contours.clear();
				}
			}
		}
}
