#include "Database.h"
#include <iostream>
#include <fstream>
#include <math.h>

fileLine Database::parseLine(string line)
{
	struct fileLine parsedLine;
	int splitterLocations[3];
	int splitterIdx = 0;

	for (int i = 0; i < line.length(); i++) {
		if (line[i] == ' ') {
			if (splitterIdx == 3) {
				cout << "Error: Wrong file format" << endl;
				exit(1);
			}
			splitterLocations[splitterIdx] = i;
			splitterIdx++;
		}
	}
	string ID = line.substr(0, splitterLocations[0]);
	string Ra = line.substr(splitterLocations[0] + 1, splitterLocations[1] - splitterLocations[0] - 1);
	string Dec = line.substr(splitterLocations[1] + 1, splitterLocations[2] - splitterLocations[1] - 1);
	string Mag = line.substr(splitterLocations[2]); //to the end
	parsedLine.ID = stoi(ID);
	parsedLine.ra = stod(Ra) / 24 * 360; //Ra is given in hours, not degrees (wtf!?) https://community.dur.ac.uk/physics.astrolab/one_lab/pm_coord.html
	parsedLine.dec = stod(Dec);
	parsedLine.mag = stod(Mag);
//	cout << parsedLine.ID << " " << parsedLine.ra << " " << parsedLine.dec << " " << parsedLine.mag << '\n';

	return parsedLine;
}

double Database::calcAlphaInDeg(fileLine star1, fileLine star2)
{
	//Winkel-Pythagoras (geht das überhaupt gut? - vermutlich nur für kleine Winkel)
	double diffDec = star1.dec - star2.dec;
	double diffRa = star1.ra - star2.ra;
	diffDec = diffDec / 180 * M_PI;
	diffRa = diffRa / 180 * M_PI;
	return sqrt(diffDec*diffDec + diffRa*diffRa) / M_PI * 180;
}

double Database::calcBetaInDeg(fileLine centerStar, fileLine star1, fileLine star2) //TODO: Test this crazy shit!
{
	//idea: make for each neighbour a line on a circle and beta is the diff between thos two
	double raDiff1 = star1.ra - centerStar.ra;
	double decDiff1 = star1.dec - centerStar.dec;
	double alpha1 = atan(decDiff1/raDiff1); //because they have same units, there is no need to convert to rad
	alpha1 = alpha1 / M_PI * 180;
	if (raDiff1 * decDiff1 > 0) {
		alpha1 *= -1;
	}
	if (raDiff1 >= 0) {
		alpha1 += 90;
	} else {
		alpha1 += 270;
	}

	double raDiff2 = star2.ra - centerStar.ra;
	double decDiff2 = star2.dec - centerStar.dec;
	double alpha2 = atan(decDiff2/raDiff2); //because they have same units, there is no need to convert to rad
	alpha2 = alpha2 / M_PI * 180;
	if (raDiff2 * decDiff2 > 0) {
		alpha2 *= -1;
	}
	if (raDiff2 >= 0) {
		alpha2 += 90;
	} else {
		alpha2 += 270;
	}

	return min(abs(alpha1-alpha2), abs(abs(alpha1-alpha2) - 360));
}

void Database::parseFromHipparcos(string filename, double maxMag)
{
	ifstream hipparcosTXT;
	string line;
	int droppedCtr = 0;
	hipparcosTXT.open(filename, ios::in);


	if (hipparcosTXT.is_open())	{
		while (getline(hipparcosTXT, line)) {
			struct fileLine fileline = parseLine(line);
			if (fileline.mag <= maxMag) {
				rawFromfile->push_back(fileline);
			} else {
				droppedCtr++;
			}
		}
		hipparcosTXT.close();
	}
	else {
		cout << "Unable to open file";
		exit(1);
	}
	cout << "imported " << rawFromfile->size() << " lines from file" << endl;
	cout << "dropped " << droppedCtr << " lines because their magnitude was over " << maxMag << endl;
}

void Database::rawToTable1(double distance = 1)
{
	for (int i = 0; i < rawFromfile->size(); i++) {
		fileLine raw = rawFromfile->at(i);
		tab1 tab1Line;
		double c = cos(raw.dec / 180 * M_PI) * distance;
		tab1Line.ID = raw.ID;
		tab1Line.x = cos(raw.ra / 180 * M_PI) * c;
		tab1Line.y = sin(raw.ra / 180 * M_PI) * c;
		tab1Line.z = sqrt(distance*distance - c*c);
		tab1Line.mag = raw.mag;
		table1->push_back(tab1Line);
//		cout << tab1Line.ID << " " << tab1Line.x << " " << tab1Line.y << " " << tab1Line.z << " " << tab1Line.mag << endl;
	}
}

void Database::fillTable2()
{
	for (int i = 0; i < rawFromfile->size(); i++) {
		struct fileLine currentStar = rawFromfile->at(i);
		struct tab2 tab2Line;
		struct fileLine nearest;
		struct fileLine scndNearest;
		double minAlpha = INFINITY;
		unsigned int empty = 0;

		for (int j = 0; j < rawFromfile->size(); j++) {
			struct fileLine testStar = rawFromfile->at(j);
			if (currentStar.ID == testStar.ID || ((currentStar.ra == testStar.ra) && (currentStar.dec == testStar.dec))) {
				//nothing
			} else {
				double alpha = calcAlphaInDeg(currentStar, rawFromfile->at(j));
				if (alpha < minAlpha /*|| empty < 2*/) {
					scndNearest = nearest;
					nearest = rawFromfile->at(j);
					minAlpha = alpha;
					empty++;
				}
			}
		}
		tab2Line.ID1 = currentStar.ID;
		tab2Line.ID2 = nearest.ID;
		tab2Line.ID3 = scndNearest.ID;
		tab2Line.beta = calcBetaInDeg(currentStar, nearest, scndNearest);
		tab2Line.alpha1 = calcAlphaInDeg(currentStar, nearest);
		tab2Line.alpha2 = calcAlphaInDeg(currentStar, scndNearest);
		table2->push_back(tab2Line);
//		cout << tab2Line.ID1 << " " << tab2Line.ID2 << " " << tab2Line.ID3 << " " << tab2Line.beta << " " << tab2Line.alpha1 << " " << tab2Line.alpha2 << endl;
	}
}

fileLine Database::getById(int ID)
{
	for (int i = 0; i < rawFromfile->size(); i++) {
		if (rawFromfile->at(i).ID == ID) {
			return rawFromfile->at(i);
		}
	}
}

tab1 Database::getTab1ById(int ID)
{
	for (int i = 0; i < table1->size(); i++) {
		if (table1->at(i).ID == ID) {
			return table1->at(i);
		}
	}
}

tab2 Database::findBestMatch(double beta, double alpha1, double alpha2)
{
	double minDiff = INFINITY;
	struct tab2 bestMatch;
	for (int i = 0; i < table2->size(); i++) {
		struct tab2 current = table2->at(i);
		double diff = abs(beta - current.beta) + abs(alpha1 - current.alpha1) + abs(alpha2 - current.alpha2);
		if (diff < minDiff) {
			bestMatch = current;
			minDiff = diff;
//			cout << "diff: " << diff << endl;
		}
	}
	for (int i = 0; i < table2->size(); i++) { //maybe alphas are swapped
		struct tab2 current = table2->at(i);
		double diff = abs(beta - current.beta) + abs(alpha2 - current.alpha1) + abs(alpha1 - current.alpha2);
		if (diff < minDiff) {
			bestMatch = current;
			minDiff = diff;
//			cout << "diff: " << diff << endl;
		}
	}
	cout << "total diff of best match: " << minDiff << " deg" << endl;
	return bestMatch;
}

Database::Database(string filenameHipparcos, double maxMag)
{
	cout << "initialising database..." << endl;
	rawFromfile = new vector<fileLine> ;
	table1 = new vector<tab1>;
	table2 = new vector<tab2>;
	parseFromHipparcos(filenameHipparcos, maxMag);
	cout << "generating table 1..." << endl;
	rawToTable1(1);
	cout << "generating table 2..." << endl;
	fillTable2();
	cout << endl;
}

Database::~Database()
{
	delete rawFromfile;
	delete table1;
	delete table2;
}
