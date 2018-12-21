#ifndef MASTER_H
#define MASTER_H
#include <opencv2/opencv.hpp>
#include "Database.h"
#include "Camera.h"

using namespace cv;
using namespace std;

class MaSter
{
	private:
		Camera* cam;
		Database* db;
		static inline int getByteBW(Mat frame, int x, int y);
		static inline void writeByteBW(Mat frame, int x, int y, int value);
//		static inline Point* calcCenterSimpleROI(Mat &pic, Point start, int ROI);
		static inline Point* calcCenterAdvancedROI(Mat& pic, Point start, int ROI);
		static inline double distanceBetweenPoints(Point p1, Point p2);
		double calcAlphaInDeg(Mat& pic, Point p1, Point p2);
		double* getNormalVektorToPointSensorFrame(Mat &pic, Point point, bool normed);
		double getPositions(Mat& pic, struct tab2 dbMatch, Point M, Point alpha1, Point alpha2);
	public:
		vector<Point*>* getStarCentersByAdvancedROI(Mat &pic, int ROI, int threshold);
		vector<Point*>* removeDuplicates(vector<Point*>* stars, int ROI);
		void drawRectsAroundPoints(Mat &pic, vector<Point*>* points, int ROI);
		void crossPoint(Mat &pic, Point point, int ROI);
		void identStarsByTriangles(Mat& pic, vector<Point*>* stars, int ROI);
		int getNearestStar(Mat& pic, Point base, vector<Point*>* stars);
		Point* get2nearestStarsToCentralStar(Mat& pic, int centralStar, vector<Point*>* stars);

		MaSter(Camera* camera, Database* database);
};

#endif // MASTER_H

