#include "FindContours.h"
#define TWO_MIN(X,Y)  (Y ^ (( X ^ Y ) & - ( X <Y )))


void transLabel(int* label, int min, int max, int labelMax)
{
	int i = label[min];
	int j = label[max];
	if (i == j)
	{
		return;
	}
	
	int minNumber = TWO_MIN(i, j);
	int maxNumber = i + j - minNumber;
	label[maxNumber] = minNumber;
	for (int x = maxNumber + 1; x < labelMax; x++)
	{
		int temp = (((label[x] - maxNumber) >> 31) + ((maxNumber - label[x]) >> 31) + 1) * minNumber;
		label[x] = (-((temp - 1) >> 31)) * label[x] + temp;
	}
}

void transLabel2(int* label, int num1, int num2, int num3, int labelMax)
{
	int i = label[num1];
	int j = label[num2];
	int k = label[num3];
	if ((i == j) && (j == k))
	{
		return;
	}

	int minNumber1 = TWO_MIN(i, j);
	int maxNumber1 = i + j - minNumber1;
	int minNumber = TWO_MIN(k, minNumber1);
	int maxNumber2 = k + minNumber1 - minNumber;
	minNumber1 = TWO_MIN(maxNumber1, maxNumber2);
	label[minNumber1] = minNumber;
	for (int x = minNumber1 + 1; x < labelMax; x++)
	{
		int temp = (((label[x] - maxNumber1) >> 31) + ((maxNumber1 - label[x]) >> 31) + 1) * minNumber;
		label[x] = (-((temp - 1) >> 31)) * label[x] + temp;
		temp = (((label[x] - maxNumber2) >> 31) + ((maxNumber2 - label[x]) >> 31) + 1) * minNumber;
		label[x] = (-((temp - 1) >> 31)) * label[x] + temp;
	}

}


void transLabel3(int* label, int num1, int num2, int num3, int num4, int labelMax)
{

	int i = label[num1];
	int j = label[num2];
	int k = label[num3];
	int z = label[num4];
	if ((i == j) && (j == k) && (k == z))
	{
		return;
	}
	int minNumber1 = TWO_MIN(i, j);
	int maxNumber1 = i + j - minNumber1;
	int minNumber2 = TWO_MIN(k, minNumber1);
	int maxNumber2 = k + minNumber1 - minNumber2;
	int minNumber = TWO_MIN(z, minNumber2);
	int maxNumber3 = z + minNumber2 - minNumber;
	minNumber1 = TWO_MIN(maxNumber1, maxNumber2);
	minNumber1 = TWO_MIN(minNumber1, maxNumber3);
	
	label[minNumber1] = minNumber;
	for (int x = minNumber1 + 1; x < labelMax; x++)
	{
		int temp = (((label[x] - maxNumber1) >> 31) + ((maxNumber1 - label[x]) >> 31) + 1) * minNumber;
		label[x] = (-((temp - 1) >> 31)) * label[x] + temp;
		temp = (((label[x] - maxNumber2) >> 31) + ((maxNumber2 - label[x]) >> 31) + 1) * minNumber;
		label[x] = (-((temp - 1) >> 31)) * label[x] + temp;
		temp = (((label[x] - maxNumber3) >> 31) + ((maxNumber3 - label[x]) >> 31) + 1) * minNumber;
		label[x] = (-((temp - 1) >> 31)) * label[x] + temp;
	}


}
int ustc_ConnectedComponentLabeling(Mat grayImg, Mat& labelImg, int bin_th)
{
	if (NULL == grayImg.data || NULL == labelImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}
	int height = grayImg.rows + 2;
	int width = grayImg.cols + 2;
	Mat tempImg;
	Mat binaryImg(height, width, CV_8UC1);
	//tempImg鐎涙ê鐨rayImg娑撳﹣绗呭锕€褰搁崥鍕⒖娑撯偓閹烘帒鎮楅惃鍕禈
	copyMakeBorder(grayImg, tempImg, 1, 1, 1, 1, 0, 0);
	//binaryImg鐎涙ê鐨empImg娴滃苯鈧厧瀵查崥搴ｆ畱閸?
	threshold(tempImg, binaryImg, bin_th, 1, THRESH_BINARY);
	//connection鐎涙绻涢柅姘厵缂佹挻鐏夐敍宀€鏁?鐞涖劎銇氶懗灞炬珯
	Mat connectImg(height, width, CV_32SC1);
	connectImg.setTo(0);
	int label[10000];
	label[0] = 0;
	int labelMax = 1;
	int *connect = (int *)connectImg.data;
	for (int i = 1; i < height - 1; ++i)
	{
		for (int j = 1; j < width - 1; ++j)
		{
			int xy = i * width + j;
			int value = binaryImg.data[xy];
			//婵″倹鐏夋稉楦垮剹閺?value娴兼艾褰夋稉? 16 閿涘矁瀚㈡稉宥嗘Ц閼冲本娅欓敍灞艰礋0
			value = (value - 1) << 4;
			int col_i = i - 1;
			int xy2 = col_i * width + j;
			//閼活櫦alue = -16 閿涘esult 缂佹挻鐏夎箛鍛暰鐏忓繋绨?閿涘苯婀猻witch娑?娴兼俺绻橀崗?default 娑擃叏绱濋懟銉ょ瑝閺勵垵鍎楅弲顖ょ礉閸掓瑤绗夋导姘閸濆吵esult閻ㄥ嫬鈧?
			int result = (binaryImg.data[xy2 - 1] << 3) + (binaryImg.data[xy2] << 2) +
				(binaryImg.data[xy2 + 1] << 1) + (binaryImg.data[xy - 1]) + value;
			switch (result)
			{
			case 0:
			{
				connect[xy] = labelMax;
				label[labelMax] = labelMax;
				labelMax++;
				break;
			}
			case 1:
			{
				connect[xy] = connect[xy - 1];
				break;
			}
			case 2:
			{
				connect[xy] = connect[xy2 + 1];
				break;
			}
			case 3:
			{
				int num1 = connect[xy - 1];
				int num2 = connect[xy2 + 1];
				int minNumber = TWO_MIN(num1, num2);
				connect[xy] = minNumber;
				transLabel(label, num1, num2, labelMax);
				break;
			}
			case 4:
			{
				connect[xy] = connect[xy2];
				break;
			}
			case 5:
			{
				int num1 = connect[xy - 1];
				int num2 = connect[xy2];
				int minNumber = TWO_MIN(num1, num2);
				connect[xy] = minNumber;
				transLabel(label, num1, num2, labelMax);
				break;
			}
			case 6:
			{
				int num1 = connect[xy2 + 1];
				int num2 = connect[xy2];
				int minNumber = TWO_MIN(num1, num2);
				connect[xy] = minNumber;
				transLabel(label, num1, num2, labelMax);
				break;
			}
			case 7:
			{
				int num1 = connect[xy2 + 1];
				int num2 = connect[xy2];
				int num3 = connect[xy - 1];
				int minNumber1 = TWO_MIN(num1, num2);
				int minNumber2 = TWO_MIN(num3, minNumber1);
				connect[xy] = minNumber2;
				transLabel2(label, num1, num2, num3, labelMax);
				break;
			}
			case 8:
			{
				connect[xy] = connect[xy2 - 1];
				break;
			}
			case 9:
			{
				int num1 = connect[xy2 - 1];
				int num2 = connect[xy - 1];
				int minNumber = TWO_MIN(num1, num2);
				connect[xy] = minNumber;
				transLabel(label, num1, num2, labelMax);
				break;
			}
			case 10:
			{
				int num1 = connect[xy2 - 1];
				int num2 = connect[xy2 + 1];
				int minNumber = TWO_MIN(num1, num2);
				connect[xy] = minNumber;
				transLabel(label, num1, num2, labelMax);
				break;
			}
			case 11:
			{
				int num1 = connect[xy2 - 1];
				int num2 = connect[xy2 + 1];
				int num3 = connect[xy - 1];
				int minNumber1 = TWO_MIN(num1, num2);
				int minNumber2 = TWO_MIN(num3, minNumber1);
				connect[xy] = minNumber2;
				transLabel2(label, num1, num2, num3, labelMax);
				break;
			}
			case 12:
			{
				int num1 = connect[xy2 - 1];
				int num2 = connect[xy2];
				int minNumber = TWO_MIN(num1, num2);
				connect[xy] = minNumber;
				transLabel(label, num1, num2, labelMax);
				break;
			}
			case 13:
			{
				int num1 = connect[xy2 - 1];
				int num2 = connect[xy2];
				int num3 = connect[xy - 1];
				int minNumber1 = TWO_MIN(num1, num2);
				int minNumber2 = TWO_MIN(num3, minNumber1);
				connect[xy] = minNumber2;
				transLabel2(label, num1, num2, num3, labelMax);
				break;
			}
			case 14:
			{
				int num1 = connect[xy2 - 1];
				int num2 = connect[xy2];
				int num3 = connect[xy2 + 1];
				int minNumber1 = TWO_MIN(num1, num2);
				int minNumber2 = TWO_MIN(num3, minNumber1);
				connect[xy] = minNumber2;
				transLabel2(label, num1, num2, num3, labelMax);
				break;
			}
			case 15:
			{
				int num1 = connect[xy2 - 1];
				int num2 = connect[xy2];
				int num3 = connect[xy2 + 1];
				int num4 = connect[xy - 1];
				int minNumber1 = TWO_MIN(num1, num2);
				int minNumber2 = TWO_MIN(num3, minNumber1);
				int minNumber = TWO_MIN(num4, minNumber2);
				connect[xy] = minNumber;
				transLabel3(label, num1, num2, num3, num4, labelMax);
				break;
			}
			default:
				break;
			}
		}
	}

	for (int i = 1; i< height - 1; ++i)
		for (int j = 1; j < width - 1; ++j)
		{
			int num = connect[i * width + j];
			connect[i * width + j] = label[num];
		}

	labelImg = connectImg.clone();
}

void next_point1(int *point_temp, int width, vector <cv::Point> &p_one, int* point_i, int* point_j, int* point_sign)
{

	int i = *point_i;
	int j = *point_j;
	int num = point_temp[i * width + j];

	Point next;
	int result = ((point_temp[(i + 1) * width + j] / num) << 2) + ((point_temp[(i + 1) * width + j - 1] / num) << 1) + (point_temp[i * width + j - 1] / num);
	switch (result)
	{
	case 7:
	case 6:
	case 5:
	case 4:
	{
		*point_i = i + 1;
		*point_j = j;
		next.x = i + 1;
		next.y = j;
		p_one.push_back(next);
		*point_sign = 6;
		return;
	}
	case 3:
	case 2:
	{
		*point_i = i + 1;
		*point_j = j - 1;
		next.x = i + 1;
		next.y = j - 1;
		p_one.push_back(next);
		*point_sign = 7;
		return;
	}
	case 1:
	{
		*point_i = i;
		*point_j = j - 1;
		next.x = i;
		next.y = j - 1;
		p_one.push_back(next);
		*point_sign = 8;
		return;
	}
	default:
	{
		int result2 = ((point_temp[(i - 1) * width + j - 1] / num) << 1) + ((point_temp[(i - 1) * width + j] / num));
		switch (result2)
		{

		case 3:case 2:
		{
			*point_i = i - 1;
			*point_j = j - 1;
			next.x = i - 1;
			next.y = j - 1;
			p_one.push_back(next);
			*point_sign = 1;
			return;
		}
		case 1:
		{
			*point_i = i - 1;
			*point_j = j;
			next.x = i - 1;
			next.y = j;
			p_one.push_back(next);
			*point_sign = 2;
			return;
		}
		default:
		{
			result2 = ((point_temp[(i - 1) * width + j + 1] / num) << 1) + point_temp[i * width + j + 1] / num;
			switch (result2)
			{
			case 3:
			case 2:
			{
				*point_i = i - 1;
				*point_j = j + 1;
				next.x = i - 1;
				next.y = j + 1;
				p_one.push_back(next);
				*point_sign = 3;
				return;
			}
			case 1:
			{
				*point_i = i;
				*point_j = j + 1;
				next.x = i;
				next.y = j + 1;
				p_one.push_back(next);
				*point_sign = 4;
				return;
			}
			default:
			{
				*point_sign = 0;
				return;
			}
			}
		}
		}
	}
	}
}

void next_point2(int *point_temp, int width, vector <cv::Point> &p_one, int* point_i, int* point_j, int* point_sign)
{

	int i = *point_i;
	int j = *point_j;
	int num = point_temp[i * width + j];

	Point next;
	int result = ((point_temp[(i + 1) * width + j - 1] / num) << 2) + ((point_temp[i * width + j - 1] / num) << 1) + (point_temp[(i - 1) * width + j - 1] / num);
	switch (result)
	{
	case 7:
	case 6:
	case 5:
	case 4:
	{
		*point_i = i + 1;
		*point_j = j - 1;
		next.x = i + 1;
		next.y = j - 1;
		p_one.push_back(next);
		*point_sign = 7;
		return;
	}
	case 3:
	case 2:
	{
		*point_i = i;
		*point_j = j - 1;
		next.x = i;
		next.y = j - 1;
		p_one.push_back(next);
		*point_sign = 8;
		return;
	}
	case 1:
	{
		*point_i = i - 1;
		*point_j = j - 1;
		next.x = i - 1;
		next.y = j - 1;
		p_one.push_back(next);
		*point_sign = 1;
		return;
	}
	default:
	{
		int result2 = ((point_temp[(i - 1) * width + j] / num) << 1) + ((point_temp[(i - 1) * width + j + 1] / num));
		switch (result2)
		{

		case 3:case 2:
		{
			*point_i = i - 1;
			*point_j = j;
			next.x = i - 1;
			next.y = j;
			p_one.push_back(next);
			*point_sign = 2;
			return;
		}
		case 1:
		{
			*point_i = i - 1;
			*point_j = j + 1;
			next.x = i - 1;
			next.y = j + 1;
			p_one.push_back(next);
			*point_sign = 3;
			return;
		}
		default:
		{
			result2 = ((point_temp[i * width + j + 1] / num) << 1) + point_temp[(i + 1) * width + j + 1] / num;
			switch (result2)
			{
			case 3:
			case 2:
			{
				*point_i = i;
				*point_j = j + 1;
				next.x = i;
				next.y = j + 1;
				p_one.push_back(next);
				*point_sign = 4;
				return;
			}
			case 1:
			{
				*point_i = i + 1;
				*point_j = j + 1;
				next.x = i + 1;
				next.y = j + 1;
				p_one.push_back(next);
				*point_sign = 5;
				return;
			}
			default:
			{
				*point_sign = 0;
				return;
			}
			}
		}
		}
	}
	}
}
void next_point3(int *point_temp, int width, vector <cv::Point> &p_one, int* point_i, int* point_j, int* point_sign)
{

	int i = *point_i;
	int j = *point_j;
	int num = point_temp[i * width + j];

	Point next;
	int result = ((point_temp[i * width + j - 1] / num) << 2) + ((point_temp[(i - 1) * width + j - 1] / num) << 1) + (point_temp[(i - 1) * width + j] / num);
	switch (result)
	{
	case 7:
	case 6:
	case 5:
	case 4:
	{
		*point_i = i;
		*point_j = j - 1;
		next.x = i;
		next.y = j - 1;
		p_one.push_back(next);
		*point_sign = 8;
		return;
	}
	case 3:
	case 2:
	{
		*point_i = i - 1;
		*point_j = j - 1;
		next.x = i - 1;
		next.y = j - 1;
		p_one.push_back(next);
		*point_sign = 1;
		return;
	}
	case 1:
	{
		*point_i = i - 1;
		*point_j = j;
		next.x = i - 1;
		next.y = j;
		p_one.push_back(next);
		*point_sign = 2;
		return;
	}
	default:
	{
		int result2 = ((point_temp[(i - 1) * width + j + 1] / num) << 1) + ((point_temp[i * width + j + 1] / num));
		switch (result2)
		{

		case 3:case 2:
		{
			*point_i = i - 1;
			*point_j = j + 1;
			next.x = i - 1;
			next.y = j + 1;
			p_one.push_back(next);
			*point_sign = 3;
			return;
		}
		case 1:
		{
			*point_i = i;
			*point_j = j + 1;
			next.x = i;
			next.y = j + 1;
			p_one.push_back(next);
			*point_sign = 4;
			return;
		}
		default:
		{
			result2 = ((point_temp[(i + 1) * width + j + 1] / num) << 1) + point_temp[(i + 1) * width + j] / num;
			switch (result2)
			{
			case 3:
			case 2:
			{
				*point_i = i + 1;
				*point_j = j + 1;
				next.x = i + 1;
				next.y = j + 1;
				p_one.push_back(next);
				*point_sign = 5;
				return;
			}
			case 1:
			{
				*point_i = i + 1;
				*point_j = j;
				next.x = i + 1;
				next.y = j;
				p_one.push_back(next);
				*point_sign = 6;
				return;
			}
			default:
			{
				*point_sign = 0;
				return;
			}
			}
		}
		}
	}
	}
}
void next_point4(int *point_temp, int width, vector <cv::Point> &p_one, int* point_i, int* point_j, int* point_sign)
{

	int i = *point_i;
	int j = *point_j;
	int num = point_temp[i * width + j];

	Point next;
	int result = ((point_temp[(i - 1) * width + j - 1] / num) << 2) + ((point_temp[(i - 1) * width + j] / num) << 1) + (point_temp[(i - 1) * width + j + 1] / num);
	switch (result)
	{
	case 7:
	case 6:
	case 5:
	case 4:
	{
		*point_i = i - 1;
		*point_j = j - 1;
		next.x = i - 1;
		next.y = j - 1;
		p_one.push_back(next);
		*point_sign = 1;
		return;
	}
	case 3:
	case 2:
	{
		*point_i = i - 1;
		*point_j = j;
		next.x = i - 1;
		next.y = j;
		p_one.push_back(next);
		*point_sign = 2;
		return;
	}
	case 1:
	{
		*point_i = i - 1;
		*point_j = j + 1;
		next.x = i - 1;
		next.y = j + 1;
		p_one.push_back(next);
		*point_sign = 3;
		return;
	}
	default:
	{
		int result2 = ((point_temp[i * width + j + 1] / num) << 1) + ((point_temp[(i + 1) * width + j + 1] / num));
		switch (result2)
		{

		case 3:case 2:
		{
			*point_i = i;
			*point_j = j + 1;
			next.x = i;
			next.y = j + 1;
			p_one.push_back(next);
			*point_sign = 4;
			return;
		}
		case 1:
		{
			*point_i = i + 1;
			*point_j = j + 1;
			next.x = i + 1;
			next.y = j + 1;
			p_one.push_back(next);
			*point_sign = 5;
			return;
		}
		default:
		{
			result2 = +((point_temp[(i + 1) * width + j] / num) << 1) + point_temp[(i + 1) * width + j - 1] / num;
			switch (result2)
			{
			case 3:
			case 2:
			{
				*point_i = i + 1;
				*point_j = j;
				next.x = i + 1;
				next.y = j;
				p_one.push_back(next);
				*point_sign = 6;
				return;
			}
			case 1:
			{
				*point_i = i + 1;
				*point_j = j - 1;
				next.x = i + 1;
				next.y = j - 1;
				p_one.push_back(next);
				*point_sign = 7;
				return;
			}
			default:
			{
				*point_sign = 0;
				return;
			}
			}
		}
		}
	}
	}
}

void next_point5(int *point_temp, int width, vector <cv::Point> &p_one, int* point_i, int* point_j, int* point_sign)
{

	int i = *point_i;
	int j = *point_j;
	int num = point_temp[i * width + j];

	Point next;
	int result = ((point_temp[(i - 1) * width + j] / num) << 2) + ((point_temp[(i - 1) * width + j + 1] / num) << 1) + (point_temp[i * width + j + 1] / num);
	switch (result)
	{
	case 7:
	case 6:
	case 5:
	case 4:
	{
		*point_i = i - 1;
		*point_j = j;
		next.x = i - 1;
		next.y = j;
		p_one.push_back(next);
		*point_sign = 2;
		return;
	}
	case 3:
	case 2:
	{
		*point_i = i - 1;
		*point_j = j + 1;
		next.x = i - 1;
		next.y = j + 1;
		p_one.push_back(next);
		*point_sign = 3;
		return;
	}
	case 1:
	{
		*point_i = i;
		*point_j = j + 1;
		next.x = i;
		next.y = j + 1;
		p_one.push_back(next);
		*point_sign = 4;
		return;
	}
	default:
	{
		int result2 = ((point_temp[(i + 1) * width + j + 1] / num) << 1) + ((point_temp[(i + 1) * width + j] / num));
		switch (result2)
		{

		case 3:case 2:
		{
			*point_i = i + 1;
			*point_j = j + 1;
			next.x = i + 1;
			next.y = j + 1;
			p_one.push_back(next);
			*point_sign = 5;
			return;
		}
		case 1:
		{
			*point_i = i + 1;
			*point_j = j;
			next.x = i + 1;
			next.y = j;
			p_one.push_back(next);
			*point_sign = 6;
			return;
		}
		default:
		{
			result2 = +((point_temp[(i + 1) * width + j - 1] / num) << 1) + point_temp[i * width + j - 1] / num;
			switch (result2)
			{
			case 3:
			case 2:
			{
				*point_i = i + 1;
				*point_j = j - 1;
				next.x = i + 1;
				next.y = j - 1;
				p_one.push_back(next);
				*point_sign = 7;
				return;
			}
			case 1:
			{
				*point_i = i;
				*point_j = j - 1;
				next.x = i;
				next.y = j - 1;
				p_one.push_back(next);
				*point_sign = 8;
				return;
			}
			default:
			{
				*point_sign = 0;
				return;
			}

			}
		}
		}
	}
	}
}

void next_point6(int *point_temp, int width, vector <cv::Point> &p_one, int* point_i, int* point_j, int* point_sign)
{

	int i = *point_i;
	int j = *point_j;
	int num = point_temp[i * width + j];

	Point next;
	int result = ((point_temp[(i - 1) * width + j + 1] / num) << 2) + ((point_temp[i * width + j + 1] / num) << 1) + (point_temp[(i + 1) * width + j + 1] / num);
	switch (result)
	{
	case 7:
	case 6:
	case 5:
	case 4:
	{
		*point_i = i - 1;
		*point_j = j + 1;
		next.x = i - 1;
		next.y = j + 1;
		p_one.push_back(next);
		*point_sign = 3;
		return;
	}
	case 3:
	case 2:
	{
		*point_i = i;
		*point_j = j + 1;
		next.x = i;
		next.y = j + 1;
		p_one.push_back(next);
		*point_sign = 4;
		return;
	}
	case 1:
	{
		*point_i = i + 1;
		*point_j = j + 1;
		next.x = i + 1;
		next.y = j + 1;
		p_one.push_back(next);
		*point_sign = 5;
		return;
	}
	default:
	{
		int result2 = ((point_temp[(i + 1) * width + j] / num) << 1) + ((point_temp[(i + 1) * width + j - 1] / num));
		switch (result2)
		{

		case 3:case 2:
		{
			*point_i = i + 1;
			*point_j = j;
			next.x = i + 1;
			next.y = j;
			p_one.push_back(next);
			*point_sign = 6;
			return;
		}
		case 1:
		{
			*point_i = i + 1;
			*point_j = j - 1;
			next.x = i + 1;
			next.y = j - 1;
			p_one.push_back(next);
			*point_sign = 7;
			return;
		}
		default:
		{
			result2 = ((point_temp[i * width + j - 1] / num) << 1) + point_temp[(i - 1) * width + j - 1] / num;
			switch (result2)
			{
			case 3:
			case 2:
			{
				*point_i = i;
				*point_j = j - 1;
				next.x = i;
				next.y = j - 1;
				p_one.push_back(next);
				*point_sign = 8;
				return;
			}
			case 1:
			{
				*point_i = i - 1;
				*point_j = j - 1;
				next.x = i - 1;
				next.y = j - 1;
				p_one.push_back(next);
				*point_sign = 1;
				return;
			}
			default:
			{
				*point_sign = 0;
				return;
			}
			}
		}
		}
	}
	}
}
void next_point7(int *point_temp, int width, vector <cv::Point> &p_one, int* point_i, int* point_j, int* point_sign)
{

	int i = *point_i;
	int j = *point_j;
	int num = point_temp[i * width + j];

	Point next;
	int result = ((point_temp[i * width + j + 1] / num) << 2) + ((point_temp[(i + 1) * width + j + 1] / num) << 1) + (point_temp[(i + 1) * width + j] / num);
	switch (result)
	{
	case 7:
	case 6:
	case 5:
	case 4:
	{
		*point_i = i;
		*point_j = j + 1;
		next.x = i;
		next.y = j + 1;
		p_one.push_back(next);
		*point_sign = 4;
		return;
	}
	case 3:
	case 2:
	{
		*point_i = i + 1;
		*point_j = j + 1;
		next.x = i + 1;
		next.y = j + 1;
		p_one.push_back(next);
		*point_sign = 5;
		return;
	}
	case 1:
	{
		*point_i = i + 1;
		*point_j = j;
		next.x = i + 1;
		next.y = j;
		p_one.push_back(next);
		*point_sign = 6;
		return;
	}
	default:
	{
		int result2 = ((point_temp[(i + 1) * width + j - 1] / num) << 1) + ((point_temp[i * width + j - 1] / num));
		switch (result2)
		{

		case 3:case 2:
		{
			*point_i = i + 1;
			*point_j = j - 1;
			next.x = i + 1;
			next.y = j - 1;
			p_one.push_back(next);
			*point_sign = 7;
			return;
		}
		case 1:
		{
			*point_i = i;
			*point_j = j - 1;
			next.x = i;
			next.y = j - 1;
			p_one.push_back(next);
			*point_sign = 8;
			return;
		}
		default:
		{
			result2 = +((point_temp[(i - 1) * width + j - 1] / num) << 1) + point_temp[(i - 1) * width + j] / num;
			switch (result2)
			{
			case 3:
			case 2:
			{
				*point_i = i - 1;
				*point_j = j - 1;
				next.x = i - 1;
				next.y = j - 1;
				p_one.push_back(next);
				*point_sign = 1;
				return;
			}
			case 1:
			{
				*point_i = i - 1;
				*point_j = j;
				next.x = i - 1;
				next.y = j;
				p_one.push_back(next);
				*point_sign = 2;
				return;
			}
			default:
			{
				*point_sign = 0;
				return;
			}
			}
		}
		}
	}
	}
}
void next_point8(int *point_temp, int width, vector <cv::Point> &p_one, int* point_i, int* point_j, int* point_sign)
{

	int i = *point_i;
	int j = *point_j;
	int num = point_temp[i * width + j];

	Point next;
	int result = ((point_temp[(i + 1) * width + j + 1] / num) << 2) + ((point_temp[(i + 1) * width + j] / num) << 1) + (point_temp[(i + 1) * width + j - 1] / num);
	switch (result)
	{
	case 7:
	case 6:
	case 5:
	case 4:
	{
		*point_i = i + 1;
		*point_j = j + 1;
		next.x = i + 1;
		next.y = j + 1;
		p_one.push_back(next);
		*point_sign = 5;
		return;
	}
	case 3:
	case 2:
	{
		*point_i = i + 1;
		*point_j = j;
		next.x = i + 1;
		next.y = j;
		p_one.push_back(next);
		*point_sign = 6;
		return;
	}
	case 1:
	{
		*point_i = i + 1;
		*point_j = j - 1;
		next.x = i + 1;
		next.y = j - 1;
		p_one.push_back(next);
		*point_sign = 7;
		return;
	}
	default:
	{
		int result2 = ((point_temp[i * width + j - 1] / num) << 1) + ((point_temp[(i - 1) * width + j - 1] / num));
		switch (result2)
		{

		case 3:case 2:
		{
			*point_i = i;
			*point_j = j - 1;
			next.x = i;
			next.y = j - 1;
			p_one.push_back(next);
			*point_sign = 8;
			return;
		}
		case 1:
		{
			*point_i = i - 1;
			*point_j = j - 1;
			next.x = i - 1;
			next.y = j - 1;
			p_one.push_back(next);
			*point_sign = 1;
			return;
		}
		default:
		{
			result2 = +((point_temp[(i - 1) * width + j] / num) << 1) + point_temp[(i - 1) * width + j + 1] / num;
			switch (result2)
			{
			case 3:
			case 2:
			{
				*point_i = i - 1;
				*point_j = j;
				next.x = i - 1;
				next.y = j;
				p_one.push_back(next);
				*point_sign = 2;
				return;
			}
			case 1:
			{
				*point_i = i - 1;
				*point_j = j + 1;
				next.x = i - 1;
				next.y = j + 1;
				p_one.push_back(next);
				*point_sign = 3;
				return;
			}
			default:
			{
				*point_sign = 0;
				return;
			}
			}
		}
		}
	}
	}
}

void signout(int * point_temp, int width, int height, int num)
{
	for (int i = 1; i < height - 1; ++i)
	{
		int pos = i * width;
		for (int j = 1; j < width - 1; ++j)
		{
			pos++;
			int value = point_temp[pos];
			if (value == num)
				point_temp[pos] = 0;
		}
	}

}

int USTC_Find_Contours(Mat binaryImg, vector <vector<cv::Point>> &p)
{
	int height = binaryImg.rows + 2;
	int width = binaryImg.cols + 2;
	p.clear();
	Mat labelImg(height, width, CV_32SC1);
	int bin_th = 100;
	ustc_ConnectedComponentLabeling(binaryImg, labelImg, bin_th);
	Mat tempImg(height, width, CV_32SC1);
	tempImg = labelImg.clone();
	int *point_temp = (int *)tempImg.data;
	vector <cv::Point> p_one;
	p_one.clear();
	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			int num = point_temp[i * width + j];
			if (0 == num)
			{
				continue;
			}
			int result = ((point_temp[i * width + j + 1] / num) << 3) + ((point_temp[(i + 1) * width + j + 1] / num) << 2) +
				((point_temp[(i + 1) * width + j] / num) << 1) + ((point_temp[(i + 1) * width + j - 1] / num));
			int xi = i;
			int xj = j;
			int sign = 0;
			int* point_i = &xi;
			int* point_j = &xj;
			int* point_sign = &sign;
			Point coord;
			coord.x = i;
			coord.y = j;
			p_one.push_back(coord);
			switch (result)
			{
			case 15: case 14: case 13: case 12: case 11: case 10: case 9:
			case 8:
			{
				*point_j = j + 1;
				next_point4(point_temp, width, p_one, point_i, point_j, point_sign);
				while (*point_sign != 0)
				{

					switch (*point_sign)
					{
					case 1:
					{
						next_point1(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 2:
					{
						next_point2(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 3:
					{
						next_point3(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 4:
					{
						next_point4(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 5:
					{
						next_point5(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 6:
					{
						next_point6(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 7:
					{
						next_point7(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 8:
					{
						next_point8(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					}
					if ((*point_i == i) && (*point_j == j))
						*point_sign = 0;
				}
				break;
			}
			case 7:
			case 6:
			case 5:
			case 4:
			{
				*point_i = i + 1;
				*point_j = j + 1;
				next_point5(point_temp, width, p_one, point_i, point_j, point_sign);
				while (*point_sign != 0)
				{
					switch (*point_sign)
					{
					case 1:
					{
						next_point1(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 2:
					{
						next_point2(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 3:
					{
						next_point3(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 4:
					{
						next_point4(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 5:
					{
						next_point5(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 6:
					{
						next_point6(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 7:
					{
						next_point7(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 8:
					{
						next_point8(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					}
					if ((*point_i == i) && (*point_j == j))
						*point_sign = 0;
				}
				break;
			}
			case 3:
			case 2:
			{

				*point_i = i + 1;
				next_point6(point_temp, width, p_one, point_i, point_j, point_sign);
				while (*point_sign != 0)
				{
					switch (*point_sign)
					{
					case 1:
					{
						next_point1(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 2:
					{
						next_point2(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 3:
					{
						next_point3(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 4:
					{
						next_point4(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 5:
					{
						next_point5(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 6:
					{
						next_point6(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 7:
					{
						next_point7(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 8:
					{
						next_point8(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					}
					if ((*point_i == i) && (*point_j == j))
						*point_sign = 0;
				}
				break;
			}
			case 1:
			{

				*point_i = i + 1;
				*point_j = j - 1;
				next_point7(point_temp, width, p_one, point_i, point_j, point_sign);
				while (*point_sign != 0)
				{
					switch (*point_sign)
					{
					case 1:
					{
						next_point1(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 2:
					{
						next_point2(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 3:
					{
						next_point3(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 4:
					{
						next_point4(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 5:
					{
						next_point5(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 6:
					{
						next_point6(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 7:
					{
						next_point7(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					case 8:
					{
						next_point8(point_temp, width, p_one, point_i, point_j, point_sign);
						break;
					}
					}
					if ((*point_i == i) && (*point_j == j))
						*point_sign = 0;
				}
				break;
			}
			default:
			{
				break;
			}
			}
			int flag = 1;
			do
			{

				int  number = p_one.size();
				int col_i = p_one[number - 1].x;
				int col_j = p_one[number - 1].y;
				if ((col_i != i) || (col_j != j))
				{

					number--;
					int lasti = p_one[number - 1].x;
					int lastj = p_one[number - 1].y;
					*point_i = lasti;
					*point_j = lastj;
					if (lasti - col_i == 1)
					{
						if (lastj - col_j == 1)
						{
							*point_sign = 5;

						}
						else if (lastj - col_j == 0)
						{
							*point_sign = 6;

						}
						else if (lastj - col_j == -1)
						{
							*point_sign = 7;

						}
					}
					else if (lasti - col_i == 0)
					{
						if (lastj - col_j == 1)
						{
							*point_sign = 4;

						}
						if (lastj - col_j == -1)
						{
							*point_sign = 8;

						}
					}
					else if (lasti - col_i == -1)
					{
						if (lastj - col_j == 1)
						{
							*point_sign = 3;

						}
						if (lastj - col_j == 0)
						{
							*point_sign = 2;

						}
						if (lastj - col_j == -1)
						{
							*point_sign = 1;

						}
					}
					while (*point_sign != 0)
					{
						switch (*point_sign)
						{
						case 1:
						{
							next_point1(point_temp, width, p_one, point_i, point_j, point_sign);
							break;
						}
						case 2:
						{
							next_point2(point_temp, width, p_one, point_i, point_j, point_sign);
							break;
						}
						case 3:
						{
							next_point3(point_temp, width, p_one, point_i, point_j, point_sign);
							break;
						}
						case 4:
						{
							next_point4(point_temp, width, p_one, point_i, point_j, point_sign);
							break;
						}
						case 5:
						{
							next_point5(point_temp, width, p_one, point_i, point_j, point_sign);
							break;
						}
						case 6:
						{
							next_point6(point_temp, width, p_one, point_i, point_j, point_sign);
							break;
						}
						case 7:
						{
							next_point7(point_temp, width, p_one, point_i, point_j, point_sign);
							break;
						}
						case 8:
						{
							next_point8(point_temp, width, p_one, point_i, point_j, point_sign);
							break;
						}
						}
						if ((*point_i == i) && (*point_j == j))
						{
							*point_sign = 0;
							flag = 0;
						}
					}

				}
				else
				{
					flag = 0;
				}
			} while (flag);
			if (p_one.size() > 1)
			{
				p.push_back(p_one);
				signout(point_temp, width, height, num);
				p_one.clear();
			}
		}
	}
	return 1;
}

