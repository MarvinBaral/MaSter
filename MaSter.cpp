#include "MaSter.h"
#include <iostream>
#include <string>
#include <math.h>

vector<Point*>* MaSter::getStarCentersByAdvancedROI(Mat &pic, int ROI, int threshold)
{
	vector<Point*>* stars = new vector<Point*>();

	for (int y = 0; y < pic.rows; y++) {
		for (int x = 0; x < pic.cols; x++) {
			if(getByteBW(pic,x,y) >= threshold) {
				Point* center = calcCenterAdvancedROI(pic, Point(x,y), ROI);
				stars->push_back(center);
			}
		}
	}
	return stars;
}

//Point* MaSter::calcCenterSimpleROI(Mat& pic, Point start, int ROI) {
//	int totalIntesity = 0;
//	int x_weigthedIntensity = 0;
//	int y_weightedIntensity = 0;
//	int intensity;

//	for (int y = max(0, start.y - ROI); y <= min(pic.rows, start.y + ROI); y++) {
//		for(int x = max(0, start.x - ROI); x <= min(pic.cols, start.x + ROI); x++) {
//			intensity = getByteBW(pic, x, y);
//			totalIntesity += intensity;
//			x_weigthedIntensity += x * intensity;
//			y_weightedIntensity += y * intensity;
//		}
//	}
//	if (totalIntesity == 0) {
//		totalIntesity = 1;
//	}
//	Point* center = new Point(x_weigthedIntensity/totalIntesity, y_weightedIntensity/totalIntesity); //like floor(), not round() !!!
//	cout << "diff Point x:" << center->x - start.x << " y:" << center->y - start.y << endl;
//	return center;
//}

Point* MaSter::calcCenterAdvancedROI(Mat& pic, Point start, int ROI) {
	Point maxIntenseOne = start;
	for (int y = max(0, start.y - ROI); y <= min(pic.rows, start.y + ROI); y++) {
		for(int x = max(0, start.x - ROI); x <= min(pic.cols, start.x + ROI); x++) {
			if (getByteBW(pic, maxIntenseOne.x, maxIntenseOne.y) < getByteBW(pic, x, y)) {
				maxIntenseOne.x = x;
				maxIntenseOne.y = y;
			}
		}
	}

	int totalIntesity = 0;
	int x_weigthedIntensity = 0;
	int y_weightedIntensity = 0;
	int intensity;

	for (int y = max(0, maxIntenseOne.y - ROI); y <= min(pic.rows, maxIntenseOne.y + ROI); y++) {
		for(int x = max(0, maxIntenseOne.x - ROI); x <= min(pic.cols, maxIntenseOne.x + ROI); x++) {
			intensity = getByteBW(pic, x, y);
			totalIntesity += intensity;
			x_weigthedIntensity += x * intensity;
			y_weightedIntensity += y * intensity;
		}
	}
	if (totalIntesity == 0) {
		totalIntesity = 1;
	}
	Point* center = new Point(nearbyint(x_weigthedIntensity*1.f/totalIntesity*1.f), nearbyint(y_weightedIntensity*1.f/totalIntesity*1.f));
//	cout << "diff Point x:" << center->x - maxIntenseOne.x << " y:" << center->y - maxIntenseOne.y << endl;
	return center;
}

double MaSter::distanceBetweenPoints(Point p1, Point p2)
{
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

double MaSter::calcAlphaInDeg(Mat& pic, Point p1, Point p2)
{
	Point center = Point(pic.cols/2, pic.rows/2);
	double d1 = distanceBetweenPoints(center, p1) * cam->pixelSize;
	double d2 = distanceBetweenPoints(center, p2) * cam->pixelSize;
	double f = cam->f;
	double b = sqrt(f*f + d1*d1);
	double a = sqrt(f*f + d2*d2);
	double c = distanceBetweenPoints(p1, p2) * cam->pixelSize; //a, b, c now span the wanted triangle
	double alpha = acos(-1*(c*c - a*a - b*b) / (2*a*b)); //Kosinussatz

	return alpha / M_PI * 180;
}

double MaSter::getPositions(Mat &pic, tab2 dbMatch, Point M, Point alpha1, Point alpha2) //Part4
{
	struct tab1 ECI_M = db->getTab1ById(dbMatch.ID1);
	struct tab1 ECI_A1 = db->getTab1ById(dbMatch.ID2);

	double* OM;
	double* OA1;
	cout << "star vectors, normed:" << endl;
	OM = getNormalVektorToPointSensorFrame(pic, M, true);
	OA1 = getNormalVektorToPointSensorFrame(pic, alpha1, true);
	cout << "OM: [" << OM[0] << ", " << OM[1] << ", " << OM[2] << "]" << endl;
	cout << "OA1: [" << OA1[0] << ", " << OA1[1] << ", " << OA1[2] << "]" << endl;

	//TODO:
	// --> Rotationmatrix
	// --> Quaternions

}

double* MaSter::getNormalVektorToPointSensorFrame(Mat& pic, Point point, bool normed)
{
	double* normal = new double[3] ();
	Point center = Point(pic.cols/2, pic.rows/2);

	normal[2] = cam->f;
	normal[0] = (point.y - center.y) * cam->pixelSize; //Erklärung siehe Zeichnung
	normal[1] = (center.x - point.x) * cam->pixelSize;

	double length = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);

	if (normed) {
		normal[0] /= length;
		normal[1] /= length;
		normal[2] /= length;
	}

	return normal;
}

vector<Point*>* MaSter::removeDuplicates(vector<Point*>* stars, int ROI) //just removes all occuring duplicates, better would be some averaging
{
	int newx, newy, existingX, existingY;
	bool isDuplicate;
	vector<Point*>* filteredStars = new vector<Point*>;
	for (int i = 0; i < stars->size(); i++) {
		newx = stars->at(i)->x;
		newy = stars->at(i)->y;
		isDuplicate = false;
		for(int j = 0; j < filteredStars->size(); j++) {
			existingX = filteredStars->at(j)->x;
			existingY = filteredStars->at(j)->y;
			if (abs(existingX - newx) <= ROI && abs(existingY -newy) <= ROI) {
				isDuplicate = true;
			}
		}
		if (!isDuplicate) {
			filteredStars->push_back(stars->at(i));
		}
	}
	return filteredStars;
}

void MaSter::drawRectsAroundPoints(Mat& pic, vector<Point*>* points, int ROI)
{
	Scalar grey = Scalar(155);
	Scalar white = Scalar(255);
	for (int i = 0; i < points->size(); i++) {
		Point* center = points->at(i);
		rectangle(pic, Point(center->x-ROI, center->y-ROI), Point(center->x+ROI,center->y+ROI), grey, 1);
	}

	//unneeded cool feature
	int numStars = points->size();
	string num = to_string(numStars);
	putText(pic, "Stars found: " + num, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, white, 1);
}

void MaSter::crossPoint(Mat& pic, Point point, int ROI)
{
	Scalar color = Scalar(255);
	line(pic, Point(point.x - ROI, point.y - ROI), Point(point.x + ROI, point.y + ROI), color);
	line(pic, Point(point.x - ROI, point.y + ROI), Point(point.x + ROI, point.y - ROI), color);
}

void MaSter::identStarsByTriangles(Mat &pic, vector<Point*>* stars, int ROI)
{
	Scalar color = Scalar(255);
	Point center = Point(pic.cols/2, pic.rows/2);
	int idxCentralStar = getNearestStar(pic, center, stars);
	Point centralStar = *stars->at(idxCentralStar);
	crossPoint(pic, centralStar, ROI);
	crossPoint(pic, center, ROI/2);
	Point* near = get2nearestStarsToCentralStar(pic, idxCentralStar, stars); //0 = nearest, 1 = 2nd nearest
	line(pic, centralStar, near[0], color);
	line(pic, centralStar, near[1], color);
	cout << "Central Star M: " << centralStar << " px" << endl;
	cout << "neighbour 1: " << near[0] << " px" << endl;
	cout << "neighbour 2: " << near[1] << " px" << endl;

	double alpha1 = calcAlphaInDeg(pic, centralStar, near[0]);
	double alpha2 = calcAlphaInDeg(pic, centralStar, near[1]);
	cout << "alpha 1: " << alpha1 << " deg" << endl;
	cout << "alpha 2: " << alpha2 << " deg" << endl;

	double a = distanceBetweenPoints(near[1], centralStar);
	double b = distanceBetweenPoints(near[0], centralStar);
	double c = distanceBetweenPoints(near[0], near[1]);
	double rBeta = acos(-1 * ((c*c - a*a - b*b) / (2*a*b))); //Kosinussatz
	double dBeta = rBeta / M_PI * 180;
	cout << "beta: " << dBeta << " deg" << endl;

	cout << "finding matches..." << endl;
	struct tab2 match = db->findBestMatch(dBeta, alpha1, alpha2);
	cout << "Match:" << endl;
	cout << " ID M: " << match.ID1 << endl;
	cout << " ID A1: " << match.ID2 << endl;
	cout << " ID A2: " << match.ID3 << endl;
	cout << endl;
	putText(pic, to_string(match.ID1), Point(centralStar.x-20, centralStar.y-8), FONT_HERSHEY_PLAIN, 0.8, color);
	putText(pic, to_string(match.ID2), Point(near[0].x-20, near[0].y-8), FONT_HERSHEY_PLAIN, 0.8, color);
	putText(pic, to_string(match.ID3), Point(near[1].x-20, near[1].y-8), FONT_HERSHEY_PLAIN, 0.8, color);

	struct fileLine ctrlStar = db->getById(match.ID1);
	putText(pic, "central star:", Point(0, 45), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255), 1);
	putText(pic, "RA: " + to_string(ctrlStar.ra), Point(0, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255), 1);
	putText(pic, "DEC: " + to_string(ctrlStar.dec), Point(0, 75), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255), 1);

	//Part 4
	cout << "calculating position..." << endl;
	getPositions(pic, match, centralStar, near[0], near[1]);
}

int MaSter::getNearestStar(Mat &pic, Point base, vector<Point*>* stars)
{
	int nearestStar = 0;
	double lowestAngle = INFINITY;
	for (int i = 0; i < stars->size(); i++) {
		double angle = calcAlphaInDeg(pic, *stars->at(i), base);
		if (angle < lowestAngle) {
			nearestStar = i;
			lowestAngle = angle;
		}
	}
	return nearestStar;
}

Point* MaSter::get2nearestStarsToCentralStar(Mat &pic, int centralStar, vector<Point*>* stars)
{	/*
	  * remove central Star (would be obviously the nearest to itself)
	  * getNearestStar()
	  * remove nearest star
	  * getNearestStar()
	  * add the removed elements back to the vector
*/
	Point* twoNearestStars = new Point[2] ();

	Point* centralStarP = stars->at(centralStar);
	stars->erase(stars->begin() + centralStar);

	int nearest = getNearestStar(pic, *centralStarP, stars);
	Point* nearestP = stars->at(nearest);
	twoNearestStars[0] = *stars->at(nearest);
	stars->erase(stars->begin() + nearest);

	int scndNearest = getNearestStar(pic, *centralStarP, stars);
	twoNearestStars[1] = *stars->at(scndNearest);

	stars->push_back(centralStarP);
	stars->push_back(nearestP);

	return twoNearestStars;
}

MaSter::MaSter(Camera* camera, Database* database)
{
	cam = camera;
	db = database;
}

int MaSter::getByteBW(Mat frame, int x, int y) {
	return *(frame.data + frame.step[0] * y + frame.step[1] * x); //http://docs.opencv.org/2.4/modules/core/doc/basic_structures.html#mat
}

void MaSter::writeByteBW(Mat frame, int x, int y, int value) {
	*(frame.data + frame.step[0] * y + frame.step[1] * x) = value;
}
