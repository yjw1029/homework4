
#include"cv.h"
#include"FindContours.h"


#define BinMax 255
#define BinMid 100
#define BinMin 0
#define Defence 50


using namespace cv;
using namespace std;

Point pointBuff[10000000];

int USTC_Find_Contours(Mat binImg, vector<vector<cv::Point>>& countors) {
	
	int width = binImg.cols;
	int height = binImg.rows;
	Mat binImg_loc = Mat::zeros(height+2, width+2, CV_8UC1);

	binImg.copyTo(binImg_loc(Range(1, 1 + height), Range(1, 1 + width)));
	Mat graph = binImg_loc.clone();
	binImg_loc.convertTo(binImg_loc, CV_16UC1);
	unsigned short*p = (unsigned short*)binImg_loc.data;
	width+=2;
	height+=2;
	int size = width*height;
	uchar inflag = 0;
	int sideI[8] = { -width - 1,-width ,-width + 1,1,width + 1,width ,width - 1,-1 };
	int sideY[8] = { -1,-1,-1,0,1,1,1,0 };
	int sideX[8] = { -1,0,1,1,1,0,-1,-1 };
	int searchI[8][8];
	int searchX[8][8];
	int searchY[8][8];
	int directRefresh[8][8];
	for (int a = 0; a < 8; a++) {
		for (int b = 0; b < 8; b++) {
			int c = (a + 8 - 2 + b) % 8;
			searchI[a][b] = sideI[c];
			searchX[a][b] = sideX[c];
			searchY[a][b] = sideY[c];
			directRefresh[a][b] = c;
		}
	}

	Point OneOne;
	OneOne.x = 1;
	OneOne.y = 1;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = x + width*y;
			
			if ((p[i] == BinMax)&& p[i-1]== BinMin) {
				
				//begin countor finding...
				//first point
				Point first;
				Point now;
				Point pre;
				first.x = x;
				first.y = y;
				
				vector<Point> ctr;
				ctr.clear();
				
				int buff_count = 0;
				int times = 0;
				pointBuff[buff_count++] = first;
				
				p[i] = BinMid;
				int direct;
				for (direct = 0; direct < 8&&p[i + sideI[direct]]==BinMin; direct++);
				//direct==8:noise single point
				if (direct == 8) {
					continue;
				}
				//direct==0-7:next countor point
				
				now.x = x + sideX[direct];
				now.y = y + sideY[direct];
				while (now != first) {
					
					pre = now - OneOne;
					pointBuff[buff_count++] = pre;
					
					int current = now.x + now.y*width;
					p[current] = BinMid;
					int j;
					for (j = 0;j < 8 && p[current + searchI[direct][j]]== BinMin; j++);
									
					direct = directRefresh[direct][j];
					now.x += sideX[direct];
					now.y += sideY[direct];

				}
				ctr.assign(pointBuff, pointBuff + buff_count);
				
				//insert this countor
				countors.push_back(ctr);
			}
		}
			
	}

	
	
	return MY_OK;
}
