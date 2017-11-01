#include "FindContours.h"
#define MAX_NUMBER 100000

int ustc_ConnectedComponentLabeling(
	Mat binaryImg,
	Mat& labelImg)
{

	if (NULL == binaryImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}
	int width = binaryImg.cols;
	int height = binaryImg.rows;


	Mat testImg(height + 2, width + 2, CV_8UC1, Scalar(0));
	Mat imageROI = testImg(Rect(1, 1, width, height));
	binaryImg.copyTo(imageROI);

	Mat labelImg_copy(height + 2, width + 2, CV_32SC1, Scalar(0));
	int label[MAX_NUMBER] = { 0 };
	int label_count = 0;
	int result[MAX_NUMBER] = { 0 };
	for (int row_i = 1; row_i <= height; row_i++)
	{
		int temp0 = row_i*width;
		for (int col_j = 1; col_j <= width; col_j++)
		{
			int temp1 = temp0 + col_j;
			int light = testImg.data[temp1];
			if (0 == light)
			{
				continue;
			}
			int a = ((int *)labelImg_copy.data)[temp1 - width - 1];
			int b = ((int *)labelImg_copy.data)[temp1 - width];
			int c = ((int *)labelImg_copy.data)[temp1 - width + 1];
			int d = ((int *)labelImg_copy.data)[temp1 - 1];
			int e = ((int *)labelImg_copy.data)[temp1];
			int a_judge = (a > 0) ? 1 : 0;
			int b_judge = (b > 0) ? 1 : 0;
			int c_judge = (c > 0) ? 1 : 0;
			int d_judge = (d > 0) ? 1 : 0;
			int type = a_judge * 8 + b_judge * 4 + c_judge * 2 + d_judge;
			switch (type)
			{
			case 0:
				label_count++;
				label[label_count] = label_count;
				((int *)labelImg_copy.data)[temp1] = label_count;
				break;

			case 1:
				((int *)labelImg_copy.data)[temp1] = d;
				break;
			case 2:
				((int *)labelImg_copy.data)[temp1] = c;
				break;
			case 4:
				((int *)labelImg_copy.data)[temp1] = b;
				break;
			case 8:
				((int *)labelImg_copy.data)[temp1] = a;
				break;

			default:
				int maxer = a;
				int miner = a;
				if (a < b)
				{
					maxer = b;
				}
				if (maxer < c)
				{
					maxer = c;
				}
				if (maxer < d)
				{
					maxer = d;
				}

				if ((miner == 0 || a > b) && b != 0)
				{
					miner = b;
				}
				if ((miner == 0 || miner > c) && c != 0)
				{
					miner = c;
				}
				if ((miner == 0 || miner > d) && d != 0)
				{
					miner = d;
				}
				if (maxer == miner)
				{
					((int *)labelImg_copy.data)[temp1] = miner;
				}
				else if (miner != maxer)
				{
					((int *)labelImg_copy.data)[temp1] = miner;
					if (result[maxer] == 0)
					{
						result[maxer] = (result[miner] == 0) ? miner : result[miner];
						for (int temp_k = maxer + 1; temp_k <= label_count; temp_k++)
						{
							if (result[temp_k] == maxer)
							{
								result[temp_k] = result[maxer];
							}
						}

					}
					else if (result[maxer] != 0 && result[maxer] != miner)
					{
						int minest = result[maxer];
						if (miner < minest)
						{
							int temp = miner;
							miner = minest;
							minest = temp;
						}

						result[miner] = (result[minest] == 0) ? minest : result[minest];
						for (int temp_k = miner + 1; temp_k <= label_count; temp_k++)
						{
							if (result[temp_k] == miner)
							{
								result[temp_k] = result[miner];
							}

						}
					}
				}
				break;

			}
		}
	}

	for (int row_i = 1; row_i <= height; row_i++)
	{
		int temp0 = row_i*width;

		for (int col_j = 1; col_j <= width; col_j++)
		{
			int temp1 = temp0 + col_j;
			int currlabel = ((int *)labelImg_copy.data)[temp1];
			int newlabel = result[currlabel];
			((int *)labelImg_copy.data)[temp1] = newlabel ? newlabel : currlabel;
		}
	}

	Mat finalimageROI = labelImg_copy(Rect(1, 1, width, height));
	finalimageROI.copyTo(labelImg);

	return MY_OK;
}
int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours)
{
	if (NULL == binaryImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}
	contours.clear();
	int width = binaryImg.cols;
	int height = binaryImg.rows;
	Mat labelImg(height, width, CV_32SC1);
	int flag1 = ustc_ConnectedComponentLabeling(binaryImg, labelImg);
	if (flag1 == MY_FAIL)
	{
		cout << "Labeling is failing." << endl;
		return MY_FAIL;
	}

	Mat labelImg_copy2(height + 2, width + 2, CV_32SC1, Scalar(0));
	for (int i = 1; i <= height; i++)
	{
		for (int j = 1; j <= width; j++)
		{
			((int *)labelImg_copy2.data)[i*width + j] = ((int *)labelImg.data)[(i - 1)*width + j - 1];
		}
	}

	for (int row_i = 1; row_i <= height; row_i++)
	{
		int temp0 = row_i*width;
		for (int col_j = 1; col_j <= width; col_j++)
		{
			int temp1 = temp0 + col_j;
			int currlabel = ((int *)labelImg_copy2.data)[temp1];
			if (0 != currlabel)
			{
				vector<cv::Point> curr_contour;
				curr_contour.push_back(cv::Point(col_j - 1, row_i - 1));
				cv::Point start_pt = curr_contour[0];
				cv::Point last_pt(col_j - 2, row_i - 2);
				cv::Point curr_pt = start_pt;
				cv::Point next_pt(-1, -1);
				int start_neigh_x[4] = { start_pt.x - 1, start_pt.x, start_pt.x + 1, start_pt.x + 1 };
				int start_neigh_y[4] = { start_pt.y + 1, start_pt.y + 1, start_pt.y + 1, start_pt.y };
				int flag_next = 1;
				while (flag_next)
				{
					int judge = (curr_pt.x - last_pt.x) * 10 + (curr_pt.y - last_pt.y);
					int flag = 0;
					int px = curr_pt.x;
					int py = curr_pt.y;
					int next_x[5] = { 0 };
					int next_y[5] = { 0 };
					switch (judge)
					{
					case 10:

						next_x[0] = px;
						next_x[1] = px + 1;
						next_x[2] = px + 1;
						next_x[3] = px + 1;
						next_x[4] = px;
						next_y[0] = py + 1;
						next_y[1] = py + 1;
						next_y[2] = py;
						next_y[3] = py - 1;
						next_y[4] = py - 1;

						break;
					case 9:

						next_x[0] = px + 1;
						next_x[1] = px + 1;
						next_x[2] = px + 1;
						next_x[3] = px;
						next_x[4] = px - 1;

						next_y[0] = py + 1;
						next_y[1] = py;
						next_y[2] = py - 1;
						next_y[3] = py - 1;
						next_y[4] = py - 1;

						break;
					case -1:

						next_x[0] = px + 1;
						next_x[1] = px + 1;
						next_x[2] = px;
						next_x[3] = px - 1;
						next_x[4] = px - 1;

						next_y[0] = py;
						next_y[1] = py - 1;
						next_y[2] = py - 1;
						next_y[3] = py - 1;
						next_y[4] = py;

						break;
					case -11:
						next_x[0] = px + 1;
						next_x[1] = px;
						next_x[2] = px - 1;
						next_x[3] = px - 1;
						next_x[4] = px - 1;

						next_y[0] = py - 1;
						next_y[1] = py - 1;
						next_y[2] = py - 1;
						next_y[3] = py;
						next_y[4] = py + 1;

						break;
					case -10:
						next_x[0] = px;
						next_x[1] = px - 1;
						next_x[2] = px - 1;
						next_x[3] = px - 1;
						next_x[4] = px;

						next_y[0] = py - 1;
						next_y[1] = py - 1;
						next_y[2] = py;
						next_y[3] = py + 1;
						next_y[4] = py + 1;


						break;
					case -9:


						next_x[0] = px - 1;
						next_x[1] = px - 1;
						next_x[2] = px - 1;
						next_x[3] = px;
						next_x[4] = px + 1;

						next_y[0] = py - 1;
						next_y[1] = py;
						next_y[2] = py + 1;
						next_y[3] = py + 1;
						next_y[4] = py + 1;


						break;
					case 1:
						next_x[0] = px - 1;
						next_x[1] = px - 1;
						next_x[2] = px;
						next_x[3] = px + 1;
						next_x[4] = px + 1;

						next_y[0] = py;
						next_y[1] = py + 1;
						next_y[2] = py + 1;
						next_y[3] = py + 1;
						next_y[4] = py;

						break;
					case 11:

						next_x[0] = px - 1;
						next_x[1] = px;
						next_x[2] = px + 1;
						next_x[3] = px + 1;
						next_x[4] = px + 1;

						next_y[4] = py - 1;
						next_y[0] = py + 1;
						next_y[1] = py + 1;
						next_y[2] = py + 1;
						next_y[3] = py;

						break;
					}

					for (int i = 0; i < 5; i++){
						next_pt.x = next_x[i];
						next_pt.y = next_y[i];
						int temp = (next_pt.y + 1)*width + next_pt.x + 1;
						int templabel = ((int *)labelImg_copy2.data)[temp];
						if (templabel)
						{
							flag = 1;
							break;
						}
					}
					if (flag == 0)
					{
						next_pt.x = -1;
						next_pt.y = -1;
						if (start_pt == curr_pt)
						{
							break;
						}
						else if (start_pt != curr_pt)
						{
							next_pt = last_pt;
						}
					}

					last_pt = curr_pt;
					curr_pt = next_pt;
					curr_contour.push_back(cv::Point(curr_pt.x, curr_pt.y));
					if (start_pt == next_pt)
					{
						int temp_count;
						int temp_next;
						for (temp_count = 0; temp_count < 4; temp_count++)
						{
							if (last_pt.x == start_neigh_x[temp_count] && last_pt.y == start_neigh_y[temp_count])
							{
								break;
							}

						}
						for (temp_next = temp_count + 1; temp_next < 4; temp_next++)
						{
							int temp_neigh = start_neigh_x[temp_next] + 1 + (start_neigh_y[temp_next] + 1)*width;
							int neigh_label = ((int *)labelImg_copy2.data)[temp_neigh];
							if (neigh_label)
							{
								last_pt = curr_pt;
								curr_pt.x = start_neigh_x[temp_next];
								curr_pt.y = start_neigh_y[temp_next];
								curr_contour.push_back(cv::Point(curr_pt.x, curr_pt.y));
								break;
							}
						}

						if (temp_next == 4)
						{
							flag_next = 0;
						}


					}
				}
				contours.push_back(curr_contour);
				for (int row_i0 = 1; row_i0 <= height; row_i0++)
				{
					int temp00 = row_i0*width;
					for (int col_j0 = 1; col_j0 <= width; col_j0++)
					{
						int temp10 = temp00 + col_j0;
						int currlabel0 = ((int *)labelImg_copy2.data)[temp10];
						if (currlabel0 == currlabel)
						{
							((int *)labelImg_copy2.data)[temp10] = 0;
						}
					}
				}
			}
		}
	}
	return MY_OK;

}
