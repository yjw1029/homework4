#include "FindContours.h"

#define VERSION 6
#define ARRAY_MAX_SIZE 10000000

static void show_result(const cv::Mat &labelImg, const cv::Mat &srcImg) {
	Mat color_result(labelImg.rows, labelImg.cols, CV_16UC3);

	double max_label;
	minMaxLoc(labelImg, nullptr, &max_label);

	assert(max_label <= 65536 * 3);

	int *color_b = new int[max_label];
	int *color_g = new int[max_label];
	int *color_r = new int[max_label];
	color_b[0] = color_r[0] = color_g[0] = 65535;   //background white to clear
	srand(1);
	for (int i = 1; i<max_label; ) {
		//        srand(i*clock());
		int b = (rand() % 65536);
		int r = (rand() % 65536);
		int g = (rand() % 65536);
		int flag = 1;

		for (int j = 0; j<i; j++) {
			if (color_b[j] == b&&color_g[j] == g&&color_r[j] == r) {
				flag = 0;
				break;
			}
		}

		if (flag) {
			color_b[i] = b;
			color_r[i] = r;
			color_g[i] = g;
			i++;
		}
	}

	int pixiv = srcImg.rows*srcImg.cols;

	uint16_t *pc = (uint16_t *)color_result.data;
	int *pl = (int *)labelImg.data;

	for (int i = 0; i<pixiv; i++) {
		pc[3 * i + 0] = color_b[pl[i]];
		pc[3 * i + 1] = color_g[pl[i]];
		pc[3 * i + 2] = color_r[pl[i]];
	}

	delete[] color_b;
	delete[] color_g;
	delete[] color_r;

	namedWindow("result", WINDOW_FREERATIO);
	imshow("result", color_result);
	waitKey();
}

#if VERSION <= 5
/* Private subfunctions ------------------------------------------------------*/
static int compare(const void* a, const void* b)
{
	return *(int*)a - *(int*)b;
}

static inline void merge_operation(int *rnext, int *rtable, int *tail, int u, int v) {
	int i = v;
	while (i != -1) {
		rtable[i] = u;
		i = rnext[i];
	}
	rnext[tail[u]] = v;
	tail[u] = tail[v];
}

static void resolve(int c1, int c4, int *rtable, int *rnext, int *tail) {
	int u = rtable[c1], v = rtable[c4];
	if (u < v)
		merge_operation(rnext, rtable, tail, u, v);
	else if (u > v)
		merge_operation(rnext, rtable, tail, v, u);
}

static void epoch1(cv::Mat &labelImg, int *rtable, int *rnext, int *tail, int &label_value) {

	const int rows = labelImg.rows;
	const int cols = labelImg.cols;
	const int Vb = 0;

	int rows_index, cols_index;
	int c1, c2, c3, c4;
	int *now1, *now2, *now3, *now4;

	for (rows_index = 1; rows_index < rows - 1; rows_index++) {
		int *pl = labelImg.ptr<int>(rows_index); //present prosition
		int *pt = labelImg.ptr<int>(rows_index - 1);//last_row prosition
		cols_index = 1;
		//extend_loop
		for (; cols_index < cols - 1 - 4; cols_index += 4) {
			//first
			if (*(now1 = (pl + cols_index))) {
				c3 = pt[cols_index];

				if (c3 != Vb)
					*now1 = c3;
				else {
					c4 = pt[cols_index + 1];
					c1 = pl[cols_index - 1];
					if (c1) {
						*now1 = c1;
						if (c4) {
							resolve(c1, c4, rtable, rnext, tail);
						}
					}
					else {
						c2 = pt[cols_index - 1];
						if (c2) {
							*now1 = c2;
							if (c4) {
								resolve(c2, c4, rtable, rnext, tail);
							}
						}
						else if (c4)
							*now1 = c4;
						else {
							*now1 = label_value;
							rtable[label_value] = label_value;
							rnext[label_value] = -1;
							tail[label_value] = label_value;
							label_value++;
						}
					}
				}
			}
			//second
			if (*(now2 = (pl + cols_index + 1))) {
				c3 = pt[cols_index + 1];

				if (c3 != Vb)
					*now2 = c3;
				else {
					c4 = pt[cols_index + 2];
					c1 = pl[cols_index];
					if (c1) {
						*now2 = c1;
						if (c4) {
							resolve(c1, c4, rtable, rnext, tail);
						}
					}
					else {
						c2 = pt[cols_index];
						if (c2) {
							*now2 = c2;
							if (c4) {
								resolve(c2, c4, rtable, rnext, tail);
							}
						}
						else if (c4)
							*now2 = c4;
						else {
							*now2 = label_value;
							rtable[label_value] = label_value;
							rnext[label_value] = -1;
							tail[label_value] = label_value;
							label_value++;
						}
					}
				}
			}
			//third
			if (*(now3 = (pl + cols_index + 2))) {
				c3 = pt[cols_index + 2];

				if (c3 != Vb)
					*now3 = c3;
				else {
					c4 = pt[cols_index + 3];
					c1 = pl[cols_index + 1];
					if (c1) {
						*now3 = c1;
						if (c4) {
							resolve(c1, c4, rtable, rnext, tail);
						}
					}
					else {
						c2 = pt[cols_index + 1];
						if (c2) {
							*now3 = c2;
							if (c4) {
								resolve(c2, c4, rtable, rnext, tail);
							}
						}
						else if (c4)
							*now3 = c4;
						else {
							*now3 = label_value;
							rtable[label_value] = label_value;
							rnext[label_value] = -1;
							tail[label_value] = label_value;
							label_value++;
						}
					}
				}
			}
			//fourth
			if (*(now4 = (pl + cols_index + 3))) {
				c3 = pt[cols_index + 3];

				if (c3 != Vb)
					*now4 = c3;
				else {
					c4 = pt[cols_index + 4];
					c1 = pl[cols_index + 2];
					if (c1) {
						*now4 = c1;
						if (c4) {
							resolve(c1, c4, rtable, rnext, tail);
						}
					}
					else {
						c2 = pt[cols_index + 2];
						if (c2) {
							*now4 = c2;
							if (c4) {
								resolve(c2, c4, rtable, rnext, tail);
							}
						}
						else if (c4)
							*now4 = c4;
						else {
							*now4 = label_value;
							rtable[label_value] = label_value;
							rnext[label_value] = -1;
							tail[label_value] = label_value;
							label_value++;
						}
					}
				}
			}
		}
		//rest
		for (; cols_index < cols - 1; cols_index++) {

			if (*(now1 = (pl + cols_index))) {

				c3 = pt[cols_index];

				if (c3 != Vb)
					*now1 = c3;
				else {
					c4 = pt[cols_index + 1];
					c1 = pl[cols_index - 1];
					if (c1) {
						*now1 = c1;
						if (c4) {
							resolve(c1, c4, rtable, rnext, tail);
						}
					}
					else {
						c2 = pt[cols_index - 1];
						if (c2) {
							*now1 = c2;
							if (c4) {
								resolve(c2, c4, rtable, rnext, tail);
							}
						}
						else if (c4)
							*now1 = c4;
						else {
							*now1 = label_value;
							rtable[label_value] = label_value;
							rnext[label_value] = -1;
							tail[label_value] = label_value;
							label_value++;
						}
					}
				}
			}
		}
		//end_of_cols
	}
}

static void epoch2(cv::Mat &labelImg_local, int *rtable, int label_value) {
	const int img_size = labelImg_local.rows * labelImg_local.cols;

	int i = 0;
	int *pl = (int *)labelImg_local.data;
	rtable[0] = 0;
	//extend_loop
	for (; i < img_size - 8; i += 8) {
		pl[i] = rtable[pl[i]];
		pl[i + 1] = rtable[pl[i + 1]];
		pl[i + 2] = rtable[pl[i + 2]];
		pl[i + 3] = rtable[pl[i + 3]];
		pl[i + 4] = rtable[pl[i + 4]];
		pl[i + 5] = rtable[pl[i + 5]];
		pl[i + 6] = rtable[pl[i + 6]];
		pl[i + 7] = rtable[pl[i + 7]];
	}
	//rest
	for (; i < img_size; i++) {
		pl[i] = rtable[pl[i]];
	}
}

/* Private functions ---------------------------------------------------------*/
int ustc_ConnectedComponentLabeling(Mat thresholdImg, Mat &labelImg, int& labelMax) 
{
	//Initialize
	Mat labelImg_local;
	copyMakeBorder(thresholdImg, thresholdImg, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));  //Add one round

	const int rows = thresholdImg.rows;
	const int cols = thresholdImg.cols;
	const int labels_size = rows * cols / 4;

	int *rtable = new int[labels_size + 1];
	int *rnext = new int[labels_size + 1];
	int *tail = new int[labels_size + 1];

	thresholdImg.convertTo(labelImg_local, CV_32SC1);

	int label_value = 1;
	//Start
	epoch1(labelImg_local, rtable, rnext, tail, label_value);

	//Final
	epoch2(labelImg_local, rtable, label_value);

	labelMax = label_value;

	labelImg = labelImg_local;

	delete[] rtable;
	delete[] rnext;
	delete[] tail;
	return MY_OK;
}

int ustc_ConnectedComponentLabeling(Mat thresholdImg, Mat &labelImg, int& labelMax, int& labelNumber)
{
	//Initialize
	Mat labelImg_local;
	copyMakeBorder(thresholdImg, thresholdImg, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));  //Add one round

	const int rows = thresholdImg.rows;
	const int cols = thresholdImg.cols;
	const int labels_size = rows * cols / 4;

	int *rtable = new int[labels_size + 1];
	int *rnext = new int[labels_size + 1];
	int *tail = new int[labels_size + 1];

	thresholdImg.convertTo(labelImg_local, CV_32SC1);

	int label_value = 1;
	//Start
	epoch1(labelImg_local, rtable, rnext, tail, label_value);

	//Final
	epoch2(labelImg_local, rtable, label_value);

	//Count Connected Component 
	qsort((void*)rtable, label_value, sizeof(int), compare);
	int temp = 0, number = 0;
	for (int i = 1; i < label_value; i++)
	{
		if (temp < rtable[i])
		{
			temp = rtable[i];
			number++;
		}
	}
	labelMax = label_value;
	labelNumber = number;
	labelImg = labelImg_local;

	delete[] rtable;
	delete[] rnext;
	delete[] tail;
	return MY_OK;
}
#endif

#if VERSION > 5
int rtable[ARRAY_MAX_SIZE];
int rnext[ARRAY_MAX_SIZE];
int tail[ARRAY_MAX_SIZE];

/* Private subfunctions ------------------------------------------------------*/
static int compare(const void* a, const void* b)
{
	return *(int*)a - *(int*)b;
}

static inline void merge_operation(int u, int v) {
	int i = v;
	while (i != -1) {
		rtable[i] = u;
		i = rnext[i];
	}
	rnext[tail[u]] = v;
	tail[u] = tail[v];
}

static void resolve(int c1, int c4) {
	int u = rtable[c1], v = rtable[c4];
	if (u < v)
		merge_operation(u, v);
	else if (u > v)
		merge_operation(v, u);
}

static void epoch1(cv::Mat &labelImg, int &label_value) {

	const int rows = labelImg.rows;
	const int cols = labelImg.cols;
	const int Vb = 0;

	int rows_index, cols_index;
	int c1, c2, c3, c4;
	int *now1, *now2, *now3, *now4;

	for (rows_index = 1; rows_index < rows - 1; rows_index++) {
		int *pl = labelImg.ptr<int>(rows_index); //present prosition
		int *pt = labelImg.ptr<int>(rows_index - 1);//last_row prosition
		cols_index = 1;
		//extend_loop
		for (; cols_index < cols - 1 - 4; cols_index += 4) {
			//first
			if (*(now1 = (pl + cols_index))) {
				c3 = pt[cols_index];

				if (c3 != Vb)
					*now1 = c3;
				else {
					c4 = pt[cols_index + 1];
					c1 = pl[cols_index - 1];
					if (c1) {
						*now1 = c1;
						if (c4) {
							resolve(c1, c4);
						}
					}
					else {
						c2 = pt[cols_index - 1];
						if (c2) {
							*now1 = c2;
							if (c4) {
								resolve(c2, c4);
							}
						}
						else if (c4)
							*now1 = c4;
						else {
							*now1 = label_value;
							rtable[label_value] = label_value;
							rnext[label_value] = -1;
							tail[label_value] = label_value;
							label_value++;
						}
					}
				}
			}
			//second
			if (*(now2 = (pl + cols_index + 1))) {
				c3 = pt[cols_index + 1];

				if (c3 != Vb)
					*now2 = c3;
				else {
					c4 = pt[cols_index + 2];
					c1 = pl[cols_index];
					if (c1) {
						*now2 = c1;
						if (c4) {
							resolve(c1, c4);
						}
					}
					else {
						c2 = pt[cols_index];
						if (c2) {
							*now2 = c2;
							if (c4) {
								resolve(c2, c4);
							}
						}
						else if (c4)
							*now2 = c4;
						else {
							*now2 = label_value;
							rtable[label_value] = label_value;
							rnext[label_value] = -1;
							tail[label_value] = label_value;
							label_value++;
						}
					}
				}
			}
			//third
			if (*(now3 = (pl + cols_index + 2))) {
				c3 = pt[cols_index + 2];

				if (c3 != Vb)
					*now3 = c3;
				else {
					c4 = pt[cols_index + 3];
					c1 = pl[cols_index + 1];
					if (c1) {
						*now3 = c1;
						if (c4) {
							resolve(c1, c4);
						}
					}
					else {
						c2 = pt[cols_index + 1];
						if (c2) {
							*now3 = c2;
							if (c4) {
								resolve(c2, c4);
							}
						}
						else if (c4)
							*now3 = c4;
						else {
							*now3 = label_value;
							rtable[label_value] = label_value;
							rnext[label_value] = -1;
							tail[label_value] = label_value;
							label_value++;
						}
					}
				}
			}
			//fourth
			if (*(now4 = (pl + cols_index + 3))) {
				c3 = pt[cols_index + 3];

				if (c3 != Vb)
					*now4 = c3;
				else {
					c4 = pt[cols_index + 4];
					c1 = pl[cols_index + 2];
					if (c1) {
						*now4 = c1;
						if (c4) {
							resolve(c1, c4);
						}
					}
					else {
						c2 = pt[cols_index + 2];
						if (c2) {
							*now4 = c2;
							if (c4) {
								resolve(c2, c4);
							}
						}
						else if (c4)
							*now4 = c4;
						else {
							*now4 = label_value;
							rtable[label_value] = label_value;
							rnext[label_value] = -1;
							tail[label_value] = label_value;
							label_value++;
						}
					}
				}
			}
		}
		//rest
		for (; cols_index < cols - 1; cols_index++) {

			if (*(now1 = (pl + cols_index))) {

				c3 = pt[cols_index];

				if (c3 != Vb)
					*now1 = c3;
				else {
					c4 = pt[cols_index + 1];
					c1 = pl[cols_index - 1];
					if (c1) {
						*now1 = c1;
						if (c4) {
							resolve(c1, c4);
						}
					}
					else {
						c2 = pt[cols_index - 1];
						if (c2) {
							*now1 = c2;
							if (c4) {
								resolve(c2, c4);
							}
						}
						else if (c4)
							*now1 = c4;
						else {
							*now1 = label_value;
							rtable[label_value] = label_value;
							rnext[label_value] = -1;
							tail[label_value] = label_value;
							label_value++;
						}
					}
				}
			}
		}
		//end_of_cols
	}
}

static void epoch2(cv::Mat &labelImg_local, int label_value) {
	const int img_size = labelImg_local.rows * labelImg_local.cols;

	int i = 0;
	int *pl = (int *)labelImg_local.data;
	rtable[0] = 0;
	//extend_loop
	for (; i < img_size - 8; i += 8) {
		pl[i] = rtable[pl[i]];
		pl[i + 1] = rtable[pl[i + 1]];
		pl[i + 2] = rtable[pl[i + 2]];
		pl[i + 3] = rtable[pl[i + 3]];
		pl[i + 4] = rtable[pl[i + 4]];
		pl[i + 5] = rtable[pl[i + 5]];
		pl[i + 6] = rtable[pl[i + 6]];
		pl[i + 7] = rtable[pl[i + 7]];
	}
	//rest
	for (; i < img_size; i++) {
		pl[i] = rtable[pl[i]];
	}
}

/* Private functions ---------------------------------------------------------*/
int ustc_ConnectedComponentLabeling(Mat thresholdImg, Mat &labelImg, int& labelMax, int& labelNumber)
{
	//Initialize
	Mat labelImg_local;
	copyMakeBorder(thresholdImg, thresholdImg, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));  //Add one round

	const int rows = thresholdImg.rows;
	const int cols = thresholdImg.cols;
	const int labels_size = rows * cols / 4;

	thresholdImg.convertTo(labelImg_local, CV_32SC1);

	int label_value = 1;
	//Start
	epoch1(labelImg_local, label_value);

	//Final
	epoch2(labelImg_local, label_value);

	//Count Connected Component 
	qsort((void*)rtable, label_value, sizeof(int), compare);
	int temp = 0, number = 0;
	for (int i = 1; i < label_value; i++)
	{
		if (temp < rtable[i])
		{
			temp = rtable[i];
			number++;
		}
	}
	labelMax = label_value;
	labelNumber = number;
	labelImg = labelImg_local;

	return MY_OK;
}
#endif

const int POS_X[16] = { -1,0,1,1,1,0,-1,-1,-1,0,1,1,1,0,-1,-1 };
const int POS_Y[16] = { -1,-1,-1,0,1,1,1,0,-1,-1,-1,0,1,1,1,0 };

#if VERSION == 6
/*
* VERSION_6.0
*/
cv::Point pointList[1000000];

int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours)
{
	//Protect
	if (binaryImg.data == NULL) return MY_FAIL;
	if (binaryImg.type() != CV_8UC1) return MY_FAIL;
	//contours.clear();

	//Preprocess
	Mat labelImg;
	int laberMax, labelNumber;
	ustc_ConnectedComponentLabeling(binaryImg, labelImg, laberMax, labelNumber);
	//show_result(labelImg, binaryImg);

	int label1, label2, label3, label4;
	uchar* isFound = new uchar[laberMax];
	memset(isFound, 0, sizeof(uchar)*laberMax);

	//Initialize
	const int rows = labelImg.rows;
	const int cols = labelImg.cols;
	int* data = (int*)labelImg.data;

	vector<cv::Point> pointVector;
	int pointListPos = 0;

	contours.resize(labelNumber);
	int contourPos = 0;

	//Find_Cotours
	for (int i = 0; i < rows; i++)
	{
		int j = 0;

		for (; j < cols - 4; j += 4)
		{
			label1 = *data;
			label2 = *(data + 1);
			label3 = *(data + 2);
			label4 = *(data + 3);

			if (label1 != 0)
			{
				if (isFound[label1] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label1)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label1)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols - 1) == label1)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label1)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contours[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label1] = 1;
				}//end if
			}

			if (label2 != 0)
			{
				if (isFound[label2] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 1;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 1 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label2)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label2)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols) == label2)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label2)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contours[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label2] = 1;
				}//end if
			}

			if (label3 != 0)
			{
				if (isFound[label3] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 2;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 2 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label3)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label3)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols + 1) == label3)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label3)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contours[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label3] = 1;
				}//end if
			}

			if (label4 != 0)
			{
				if (isFound[label4] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 3;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 3 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label4)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label4)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols + 2) == label4)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label4)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contours[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label4] = 1;
				}//end if
			}

			data += 4;
		}

		for (; j < cols; j++)
		{
			label1 = *data;

			if (label1 != 0)
			{
				if (isFound[label1] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label1)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label1)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols - 1) == label1)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label1)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contours[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label1] = 1;
				}//end if
			}

			data++;
		}
	}

	delete[] isFound;
	return MY_OK;
}
#endif // VERSION_6

#if VERSION == 5
/*
* VERSION_5.0
*/
cv::Point pointList[1000000];
int USTC_Find_Cotours(Mat binaryImg, vector<vector<cv::Point>>& contour)
{
	//Protect
	if (binaryImg.data == NULL) return MY_FAIL;
	if (binaryImg.type() != CV_8UC1) return MY_FAIL;
	contour.clear();

	//Preprocess
	Mat labelImg;
	int laberMax, labelNumber;
	ustc_ConnectedComponentLabeling(binaryImg, labelImg, laberMax, labelNumber);
	//show_result(labelImg, binaryImg);

	int label1, label2, label3, label4;
	uchar* isFound = new uchar[laberMax];
	memset(isFound, 0, sizeof(uchar)*laberMax);

	//Initialize
	const int rows = labelImg.rows;
	const int cols = labelImg.cols;
	int* data = (int*)labelImg.data;

	vector<cv::Point> pointVector;
	int pointListPos = 0;

	contour.resize(labelNumber);
	int contourPos = 0;

	//Find_Cotours
	for (int i = 0; i < rows; i++)
	{
		int j = 0;

		for (; j < cols - 4; j += 4)
		{
			label1 = *data;
			label2 = *(data + 1);
			label3 = *(data + 2);
			label4 = *(data + 3);

			if (label1 != 0)
			{
				if (isFound[label1] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label1)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label1)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols - 1) == label1)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label1)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label1] = 1;
				}//end if
			}

			if (label2 != 0)
			{
				if (isFound[label2] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 1;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 1 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label2)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label2)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols) == label2)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label2)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label2] = 1;
				}//end if
			}

			if (label3 != 0)
			{
				if (isFound[label3] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 2;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 2 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label3)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label3)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols + 1) == label3)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label3)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label3] = 1;
				}//end if
			}

			if (label4 != 0)
			{
				if (isFound[label4] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 3;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 3 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label4)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label4)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols + 2) == label4)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label4)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label4] = 1;
				}//end if
			}

			data += 4;
		}

		for (; j < cols; j++)
		{
			label1 = *data;

			if (label1 != 0)
			{
				if (isFound[label1] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label1)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label1)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols - 1) == label1)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label1)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label1] = 1;
				}//end if
			}

			data++;
		}
	}
		
	delete[] isFound;
	return MY_OK;
}
#endif // VERSION_5

#if VERSION == 4
/*
* VERSION_4.0
*/
cv::Point pointList[1000000];
int USTC_Find_Cotours(Mat binaryImg, vector<vector<cv::Point>>& contour)
{
	//Protect
	if (binaryImg.data == NULL) return MY_FAIL;
	if (binaryImg.type() != CV_8UC1) return MY_FAIL;
	contour.clear();

	//Preprocess
	Mat labelImg;
	int laberMax;
	ustc_ConnectedComponentLabeling(binaryImg, labelImg, laberMax);
	//show_result(labelImg, binaryImg);

	int label1, label2, label3, label4;
	uchar* isFound = new uchar[laberMax];
	memset(isFound, 0, sizeof(uchar)*laberMax);

	//Initialize
	const int rows = labelImg.rows;
	const int cols = labelImg.cols;
	int* data = (int*)labelImg.data;

	vector<cv::Point> pointVector;
	int pointListPos = 0;

	contour.resize(laberMax);
	int contourPos = 0;

	//Find_Cotours
	for (int i = 0; i < rows; i++)
	{
		int j = 0;

		for (; j < cols - 4; j+=4)
		{
			label1 = *data;
			label2 = *(data + 1);
			label3 = *(data + 2);
			label4 = *(data + 3);

			if (label1 != 0)
			{
				if (isFound[label1] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label1)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label1)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols - 1) == label1)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label1)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label1] = 1;
				}//end if
			}

			if (label2 != 0)
			{
				if (isFound[label2] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 1;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 1 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label2)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label2)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data+ cols) == label2)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label2)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label2] = 1;
				}//end if
			}

			if (label3 != 0)
			{
				if (isFound[label3] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 2;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 2 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label3)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label3)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols + 1) == label3)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label3)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label3] = 1;
				}//end if
			}

			if (label4 != 0)
			{
				if (isFound[label4] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 3;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 3 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label4)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label4)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols + 2) == label4)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label4)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label4] = 1;
				}//end if
			}

			data += 4;
		}

		for (; j < cols; j++)
		{
			label1 = *data;

			if (label1 != 0)
			{
				if (isFound[label1] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label1)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label1)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols - 1) == label1)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label1)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label1] = 1;
				}//end if
			}

			data++;
		}

	}

	contour.resize(contourPos);

	delete[] isFound;
	return MY_OK;
}
#endif // VERSION_4

#if VERSION == 3
/*
* VERSION_3.0
*/
cv::Point pointList[1000000];
int USTC_Find_Cotours(Mat binaryImg, vector<vector<cv::Point>>& contour)
{
	//Protect
	if (binaryImg.data == NULL) return MY_FAIL;
	if (binaryImg.type() != CV_8UC1) return MY_FAIL;
	contour.clear();

	//Preprocess
	Mat labelImg;
	int laberMax;
	ustc_ConnectedComponentLabeling(binaryImg, labelImg, laberMax);
	//show_result(labelImg, binaryImg);

	int label;
	uchar* isFound = new uchar[laberMax];
	memset(isFound, 0, sizeof(uchar)*laberMax);

	//Initialize
	const int rows = labelImg.rows;
	const int cols = labelImg.cols;
	int* data = (int*)labelImg.data;

	vector<cv::Point> pointVector;
	int pointListPos = 0;

	contour.resize(laberMax);
	int contourPos = 0;

	//Find_Cotours
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			label = *data;
			data++;
			if (label == 0) continue;

			if (isFound[label] == 0)
			{
				pointListPos = 0;

				cv::Point pointStart, pointNow;
				pointStart = cv::Point(j - 1, i - 1);
				pointNow = pointStart;

				pointList[pointListPos] = pointNow;
				pointListPos++;

				int type = 3;
				int* warm = data - 1;
				for (int k = type; k < type + 5; k++)
				{
					warm = data + POS_X[k] + POS_Y[k] * cols - 1;
					if (*warm == label)
					{
						pointNow.x = pointNow.x + POS_X[k];
						pointNow.y = pointNow.y + POS_Y[k];
						type = k - 2;
						break;
					}
				}

				while (pointNow != pointStart)
				{
					int* warm_head;
					pointList[pointListPos] = pointNow;
					pointListPos++;
					for (int k = type; k < type + 7; k++)
					{
						warm_head = warm + POS_X[k] + POS_Y[k] * cols;
						if (*warm_head == label)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = (k + 6) % 8;
							warm = warm_head;
							break;
						}
					}
				}

				if (type == 6 || type == 5)
				{
					if (*(data + cols - 2) == label)
					{
						pointNow.x = pointStart.x - 1;
						pointNow.y = pointStart.y + 1;
						type = 4;
						warm = warm + cols - 1;

						while (pointNow != pointStart)
						{
							int* warm_head;
							pointList[pointListPos] = pointNow;
							pointListPos++;
							for (int k = type; k < type + 7; k++)
							{
								warm_head = warm + POS_X[k] + POS_Y[k] * cols;
								if (*warm_head == label)
								{
									pointNow.x = pointNow.x + POS_X[k];
									pointNow.y = pointNow.y + POS_Y[k];
									type = (k + 6) % 8;
									warm = warm_head;
									break;
								}
							}
						}
					}
				}

				contour[contourPos].assign(pointList, pointList + pointListPos);
				contourPos++;

				isFound[label] = 1;
			}//end if
		}
	}

	contour.resize(contourPos);

	delete[] isFound;
	return MY_OK;
}
#endif // VERSION_3

#if VERSION == 2
/*
* VERSION_2.0
*/
int USTC_Find_Cotours(Mat binaryImg, vector<vector<cv::Point>>& contour)
{
	//Protect
	if (binaryImg.data == NULL) return MY_FAIL;
	if (binaryImg.type() != CV_8UC1) return MY_FAIL;
	contour.clear();

	//Preprocess
	Mat labelImg;
	int laberMax;
	ustc_ConnectedComponentLabeling(binaryImg, labelImg, laberMax);
	//show_result(labelImg, binaryImg);

	int label;
	uchar* isFound = new uchar[laberMax];
	memset(isFound, 0, sizeof(uchar)*laberMax);

	//Initialize
	const int rows = labelImg.rows;
	const int cols = labelImg.cols;
	int* data = (int*)labelImg.data;

	vector<Point> pointVector;
	Point* pointList = new Point[(rows + cols) * 100];
	int pointListPos = 0;

	//Find_Cotours
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			//cout << i << "       " << j << endl;
			label = *data;
			data++;
			if (label == 0) continue;

			if (isFound[label] == 0)
			{
				pointListPos = 0;

				Point pointStart, pointNow;
				pointStart = Point(j - 1, i - 1);
				pointNow = pointStart;

				pointList[pointListPos] = pointNow;
				pointListPos++;

				int type = 3;
				int* warm = data - 1;
				for (int k = type; k < type + 5; k++)
				{
					warm = data + POS_X[k] + POS_Y[k] * cols - 1;
					if (*warm == label)
					{
						pointNow.x = pointNow.x + POS_X[k];
						pointNow.y = pointNow.y + POS_Y[k];
						type = k - 2;
						break;
					}
				}

				while (pointNow != pointStart)
				{
					int* warm_head;
					pointList[pointListPos] = pointNow;
					pointListPos++;
					for (int k = type; k < type + 7; k++)
					{
						warm_head = warm + POS_X[k] + POS_Y[k] * cols;
						if (*warm_head == label)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = (k + 6) % 8;
							warm = warm_head;
							break;
						}
					}
				}

				if (type == 6 || type == 5)
				{
					if (*(data + cols - 2) == label)
					{
						pointNow.x = pointStart.x - 1;
						pointNow.y = pointStart.y + 1;
						type = 4;
						warm = warm + cols - 1;

						while (pointNow != pointStart)
						{
							int* warm_head;
							pointList[pointListPos] = pointNow;
							pointListPos++;
							for (int k = type; k < type + 7; k++)
							{
								warm_head = warm + POS_X[k] + POS_Y[k] * cols;
								if (*warm_head == label)
								{
									pointNow.x = pointNow.x + POS_X[k];
									pointNow.y = pointNow.y + POS_Y[k];
									type = (k + 6) % 8;
									warm = warm_head;
									break;
								}
							}
						}
					}
				}

				pointVector.clear();
				pointVector.reserve(pointListPos);
				pointVector.assign(pointList, pointList + pointListPos);
				contour.push_back(pointVector);

				isFound[label] = 1;
			}//end if
		}
	}

	delete[] pointList;
	delete[] isFound;
	return MY_OK;
}
#endif // VERSION_2

#if VERSION == 1
/*
* VERSION_1.0
*/
int USTC_Find_Cotours(Mat binaryImg, vector<vector<cv::Point>>& contour)
{
	if (binaryImg.data == NULL) return MY_FAIL;
	if (binaryImg.channels() != 1) return MY_FAIL;

	contour.clear();

	Mat labelImg;
	int laberMax;
	ustc_ConnectedComponentLabeling(binaryImg, labelImg, laberMax);

	//show_result(labelImg, binaryImg);

	const int rows = labelImg.rows;
	const int cols = labelImg.cols;
	int* data = (int*)labelImg.data;
	uchar* isFound = new uchar[laberMax];
	memset(isFound, 0, sizeof(uchar)*laberMax);
	int label = 0;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			//cout << i << "       " << j << endl;
			label = *data;
			data++;
			if (label == 0) continue;

			if (isFound[label] == 0)
			{
				Point pointStart, pointNow;
				pointStart = Point(j - 1, i - 1);
				pointNow = pointStart;

				vector<Point> contourNow;
				contourNow.push_back(pointNow);

				int type = 3;
				int* warm = data - 1;
				for (int k = type; k < type + 5; k++)
				{
					warm = data + POS_X[k] + POS_Y[k] * cols - 1;
					if (*warm == label)
					{
						pointNow.x = pointNow.x + POS_X[k];
						pointNow.y = pointNow.y + POS_Y[k];
						type = k - 2;
						break;
					}
				}

				while (pointNow != pointStart)
				{
					int* warm_head;
					contourNow.push_back(pointNow);
					for (int k = type; k < type + 7; k++)
					{
						warm_head = warm + POS_X[k] + POS_Y[k] * cols;
						if (*warm_head == label)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = (k + 6) % 8;
							warm = warm_head;
							break;
						}
					}
					
				}
				contour.push_back(contourNow);
				isFound[label] = 1;
			}//end if
		}
	}

	delete[] isFound;
	return MY_OK;
}
#endif // VERSION_1

#if VERSION == 0
/*
* VERSION_0
*/
cv::Point pointList[1000000];
int USTC_Find_Cotours(Mat binaryImg, vector<vector<cv::Point>>& contour)
{
	//Protect
	if (binaryImg.data == NULL) return MY_FAIL;
	if (binaryImg.type() != CV_8UC1) return MY_FAIL;
	contour.clear();

	//Preprocess
	Mat labelImg;
	int labelNumber;
	copyMakeBorder(binaryImg, binaryImg, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));
	labelNumber = connectedComponents(binaryImg, labelImg);

	//show_result(labelImg, binaryImg);

	int label1, label2, label3, label4;
	uchar* isFound = new uchar[labelNumber];
	memset(isFound, 0, sizeof(uchar)*labelNumber);

	//Initialize
	const int rows = labelImg.rows;
	const int cols = labelImg.cols;
	int* data = (int*)labelImg.data;

	vector<cv::Point> pointVector;
	int pointListPos = 0;

	contour.resize(labelNumber);
	int contourPos = 0;

	//Find_Cotours
	for (int i = 0; i < rows; i++)
	{
		int j = 0;

		for (; j < cols - 4; j += 4)
		{
			label1 = *data;
			label2 = *(data + 1);
			label3 = *(data + 2);
			label4 = *(data + 3);

			if (label1 != 0)
			{
				if (isFound[label1] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label1)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label1)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols - 1) == label1)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label1)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label1] = 1;
				}//end if
			}

			if (label2 != 0)
			{
				if (isFound[label2] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 1;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 1 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label2)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label2)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols) == label2)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label2)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label2] = 1;
				}//end if
			}

			if (label3 != 0)
			{
				if (isFound[label3] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 2;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 2 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label3)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label3)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols + 1) == label3)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label3)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label3] = 1;
				}//end if
			}

			if (label4 != 0)
			{
				if (isFound[label4] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data + 3;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + 3 + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label4)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label4)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols + 2) == label4)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label4)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label4] = 1;
				}//end if
			}

			data += 4;
		}

		for (; j < cols; j++)
		{
			label1 = *data;

			if (label1 != 0)
			{
				if (isFound[label1] == 0)
				{
					pointListPos = 0;

					cv::Point pointStart, pointNow;
					pointStart = cv::Point(j - 1, i - 1);
					pointNow = pointStart;

					pointList[pointListPos] = pointNow;
					pointListPos++;

					int type = 3;
					int* warm = data;
					for (int k = type; k < type + 5; k++)
					{
						warm = data + POS_X[k] + POS_Y[k] * cols;
						if (*warm == label1)
						{
							pointNow.x = pointNow.x + POS_X[k];
							pointNow.y = pointNow.y + POS_Y[k];
							type = k - 2;
							break;
						}
					}

					while (pointNow != pointStart)
					{
						int* warm_head;
						pointList[pointListPos] = pointNow;
						pointListPos++;
						for (int k = type; k < type + 7; k++)
						{
							warm_head = warm + POS_X[k] + POS_Y[k] * cols;
							if (*warm_head == label1)
							{
								pointNow.x = pointNow.x + POS_X[k];
								pointNow.y = pointNow.y + POS_Y[k];
								type = (k + 6) % 8;
								warm = warm_head;
								break;
							}
						}
					}

					if (type == 6 || type == 5)
					{
						if (*(data + cols - 1) == label1)
						{
							pointNow.x = pointStart.x - 1;
							pointNow.y = pointStart.y + 1;
							type = 4;
							warm = warm + cols - 1;

							while (pointNow != pointStart)
							{
								int* warm_head;
								pointList[pointListPos] = pointNow;
								pointListPos++;
								for (int k = type; k < type + 7; k++)
								{
									warm_head = warm + POS_X[k] + POS_Y[k] * cols;
									if (*warm_head == label1)
									{
										pointNow.x = pointNow.x + POS_X[k];
										pointNow.y = pointNow.y + POS_Y[k];
										type = (k + 6) % 8;
										warm = warm_head;
										break;
									}
								}
							}
						}
					}

					contour[contourPos].assign(pointList, pointList + pointListPos);
					contourPos++;

					isFound[label1] = 1;
				}//end if
			}

			data++;
		}
	}

	delete[] isFound;
	return MY_OK;
}
#endif // VERSION_0
