/*
 * MaSter = Marvins Sternensensor
 * OpenCV only used to get raw image data, every algorithm implemented completely self.
 * Intern calculations use SI Base-Units where possible
 *
 * Grundlagen der Raumflugmechanik
 * Marvin Baral
 * Christian Miethaner
 * Marco Brieden
 * Jakob Tröschel
*/
#include <iostream>
#include "opencv2/opencv.hpp"
#include "Camera.h"
#include "MaSter.h"
#include "Database.h"

using namespace std;
using namespace cv;

const int ROI = 5;
const double MAG = 6.5;

int main(int argc, char *argv[])
{
	int THRS = 25;


	if (argc == 3) {
		Camera* cam = new Camera(25, 5.8, MAG);

		//Part 2
		Database* db = new Database(argv[1], MAG);

		MaSter maSter(cam, db);

		Mat pic;

		int keyPressed;
		do {
			//Part 1
			pic = imread(argv[2], IMREAD_GRAYSCALE); //Sterne.BMP = 640x864 px
			cout << "detecting stars in image..." << endl;
			vector<Point*>* starlist = maSter.getStarCentersByAdvancedROI(pic, ROI, THRS);
			cout << starlist->size() << " stars found" << endl;
			vector<Point*>* filteredStarlist = maSter.removeDuplicates(starlist, ROI);
			cout << filteredStarlist->size() << " stars after removing duplicates" << endl;
			maSter.drawRectsAroundPoints(pic, filteredStarlist, ROI);
			putText(pic, "Threshold: " + to_string(THRS), Point(0, 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255), 1);
			cout << endl;

			//Part 3
			cout << "identifying stars..." << endl;
			maSter.identStarsByTriangles(pic, filteredStarlist, ROI);

			namedWindow("MaSter", WINDOW_KEEPRATIO);
			imshow("MaSter", pic);
			cout << endl;

			keyPressed = waitKey();
			switch(keyPressed) {
				case 81: //left
					break;
				case 83: //right
					break;
				case 82: //up
					if (THRS < 255) {
						THRS++;
					}
					break;
				case 84: //down
					if (THRS > 1) {
						THRS--;
					}
					break;
				case 13: //enter
					break;
				case 32: //space
					break;
				default:
					cout << "key pressed: " << keyPressed << endl;
					break;
			}

		} while (keyPressed != 27);

		imwrite(string(argv[2]) + ".result.bmp", pic);

	} else {
		cout << "Wrong number of arguments. Usage: MaSter <hipparcos file> <image file>" << endl;
	}
	return 0;
}

