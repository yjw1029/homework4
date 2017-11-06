#include "FindContours.h"

int USTC_Find_Contours(Mat binaryImg, vector < vector < cv::Point >>& contours) {
	if (NULL == binaryImg.data)
	{
		cout << "image is NULL." << endl;
		return MY_FAIL;
	}
	Mat labelImg(binaryImg.rows, binaryImg.cols, CV_32SC1);
	uchar* binaryPtr = binaryImg.data;
	int* labelPtr = (int*)labelImg.data;
	int *labelC1, *labelC2, *labelC3, *labelC4;
	const int binImgCols = binaryImg.cols;
	const int binImgRows = binaryImg.rows;
	const int binImgSize = binImgCols*binImgRows;
	memset(labelImg.data, 0, binImgSize * 4);
	int *connectedLabel = new int[binImgSize / 4 + 1]{ 0 };
	int *nextLabel = new int[binImgSize / 4 + 1]{ 0 };
	int *tailLabel = new int[binImgSize / 4 + 1]{ 0 };
	int labelingIndex = 1;
	int rowsIndex, colsIndex;
	int cmpFlag;
	int nextIndex;
	int get1Label, get2Label;
	int maxLabel, minLabel;

	labelC3 = labelPtr;
	labelC4 = labelPtr + 1;
	//Special judge on the first pixel
	labelingIndex += ((*binaryPtr) ? (*labelPtr = labelingIndex, connectedLabel[labelingIndex] = labelingIndex, tailLabel[labelingIndex] = labelingIndex, 1) : 0);
	binaryPtr++;
	labelPtr++;

	//Special judge on the first row
	for (colsIndex = binImgCols - 1; colsIndex; colsIndex--, binaryPtr++, labelPtr++) {
		if (!*binaryPtr) continue;
		*labelPtr = ((*(labelPtr - 1)) ? (*(labelPtr - 1)) : (connectedLabel[labelingIndex] = labelingIndex, tailLabel[labelingIndex] = labelingIndex, labelingIndex++));
	}

	//Ordinary judge on other rows
	for (rowsIndex = binImgRows - 1; rowsIndex; rowsIndex--) {
		*labelPtr = ((*binaryPtr) ? ((*labelC3) ? (*labelC3) : ((*labelC4) ? (*labelC4) : (connectedLabel[labelingIndex] = labelingIndex, tailLabel[labelingIndex] = labelingIndex, labelingIndex++))) : 0);
		binaryPtr++;
		labelPtr++;
		labelC2 = labelC3;
		labelC3++;
		labelC4++;
		labelC1 = labelPtr - 1;

		//Ordinary judge on the other pixels of every row
		for (colsIndex = binImgCols - 2; colsIndex; colsIndex--, binaryPtr++, labelPtr++, labelC1++, labelC2++, labelC3++, labelC4++) {
			//labelC4++;
			if (!*binaryPtr) continue;
			cmpFlag = ((*labelC4) ? ((*labelC2) ? ((connectedLabel[*labelC2] == connectedLabel[*labelC4]) ? (4) : (6)) : ((*labelC1) ? ((connectedLabel[*labelC4] == connectedLabel[*labelC1]) ? (4) : (5)) : (4))) : ((*labelC3) ? (3) : ((*labelC2) ? (2) : ((*labelC1) ? (1) : (0)))));
			*labelPtr = cmpFlag ? ((--cmpFlag) ? ((--cmpFlag) ? ((--cmpFlag) ? ((cmpFlag--, (*labelC4))) : (*labelC3)) : (*labelC2)) : (*labelC1)) : (connectedLabel[labelingIndex] = labelingIndex, tailLabel[labelingIndex] = labelingIndex, labelingIndex++);
			if (cmpFlag) {
				get1Label = connectedLabel[*labelC4];
				get2Label = (cmpFlag & 1)*(connectedLabel[*labelC1]) + ((cmpFlag & 2) >> 1)*(connectedLabel[*labelC2]);
				maxLabel = get1Label > get2Label ? (minLabel = get2Label, get1Label) : (minLabel = get1Label, get2Label);
				nextIndex = maxLabel;
				while (nextIndex) {
					//cout << nextIndex << ' ';
					connectedLabel[nextIndex] = minLabel;
					nextIndex = nextLabel[nextIndex];
				}
				nextLabel[tailLabel[minLabel]] = maxLabel;
				tailLabel[minLabel] = tailLabel[maxLabel];
			}
		}
		*labelPtr = (*binaryPtr) ? ((*labelC3) ? (*labelC3) : ((*labelC2) ? (*labelC2) : ((*labelC1) ? (*labelC1) : (connectedLabel[labelingIndex] = labelingIndex, tailLabel[labelingIndex] = labelingIndex, labelingIndex++)))) : 0;
		labelC3++;
		labelC4++;
		binaryPtr++;
		labelPtr++;
	}
	labelPtr = (int*)labelImg.data;
	for (int i = binImgSize; i; i--) {
		*labelPtr = connectedLabel[*labelPtr];
		labelPtr++;
	}


	Mat padImg(binImgRows + 2, binImgCols + 2, CV_32SC1);
	memset(padImg.data, 0, padImg.cols*padImg.rows * 4);
	Mat area = padImg.rowRange(1, 1 + binImgRows).colRange(1, 1 + binImgCols);
	labelImg.copyTo(area);
	area.release();
	const int padImgCols = binImgCols + 2;
	int keyNum = 0;
	int numX, numY;
	int integralX, integralY;
	int *padPtr,*loopPtr;
	int order[16] = { 0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7 };
	int nextorder[8] = { 6,0,0,2,2,4,4,6 };
	int nextPtr[8] = { padImgCols - 1,-padImgCols - 1,-padImgCols - 1,-padImgCols + 1,-padImgCols + 1,padImgCols + 1,padImgCols + 1,padImgCols - 1 };
	int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
	int dy[8] = { -1, -1, -1, 0, 1, 1, 1, 0 };
	int dPtr[8] = { 1,1,padImgCols,padImgCols,-1,-1,-padImgCols,-padImgCols };
	int n;
	contours.clear();
	vector<cv::Point> pointArray;
	Point keyPoint;
	padPtr = ((int*)padImg.data) + padImgCols + 1;
	numX = 0;
	numY = 0;
	for (numY = 0; numY < binImgRows; numY++) {
		for (numX = 0; numX < binImgCols; numX++, padPtr++) {
			if (!((keyNum - *padPtr) >> 31))continue;
			loopPtr = padPtr + 1;
			for (n = 3; n < 7; n++) {
				if (*loopPtr)break;
				loopPtr += dPtr[n];
			}
			if (n == 7)continue;
			keyPoint.x = numX;
			keyPoint.y = numY;
			pointArray.push_back(keyPoint);
			integralX = dx[n];
			integralY = dy[n];
			keyNum = *padPtr;
			for (; (integralX) || (integralY);) {
				padPtr = loopPtr;
				loopPtr += nextPtr[n];
				keyPoint.x = numX + integralX;
				keyPoint.y = numY + integralY;
				pointArray.push_back(keyPoint);
				n = nextorder[n];
				while (!(*loopPtr)) {
					loopPtr += dPtr[order[n]];
					n++;
				}
				n = order[n];
				integralX += dx[n];
				integralY += dy[n];
			}
			padPtr = loopPtr;
			keyPoint.x = numX;
			keyPoint.y = numY;
			pointArray.push_back(keyPoint);
			loopPtr += nextPtr[n];
			n = nextorder[n];
			while (!(*loopPtr)) {
				loopPtr += dPtr[order[n]];
				n++;
			}
			n = order[n];
			if (numX + dx[n] != pointArray[1].x || numY + dy[n] != pointArray[1].y) {
				integralX += dx[n];
				integralY += dy[n];
				for (; (integralX) || (integralY);) {
					padPtr = loopPtr;
					loopPtr += nextPtr[n];
					keyPoint.x = numX + integralX;
					keyPoint.y = numY + integralY;
					pointArray.push_back(keyPoint);
					n = nextorder[n];
					while (!(*loopPtr)) {
						loopPtr += dPtr[order[n]];
						n++;
					}
					n = order[n];
					integralX += dx[n];
					integralY += dy[n];
				}
				padPtr = loopPtr;
				keyPoint.x = numX;
				keyPoint.y = numY;
				pointArray.push_back(keyPoint);
			}
			contours.push_back(pointArray);
			pointArray.clear();
		}
		padPtr += 2;
	}
}
