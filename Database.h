#ifndef DATABASE_H
#define DATABASE_H
#include <string>
#include <vector>
using namespace std;

struct fileLine {
		int ID;
		double ra;  //degree (in file it is hours)
		double dec; //degree
		double mag;
};

struct tab1 {
		int ID;
		double x; //Normierte Vektoren
		double y;
		double z;
		double mag;
};

struct tab2 {
		int ID1; //M
		int ID2; //Alpha1
		int ID3; //Alpha2
		double beta;
		double alpha1;
		double alpha2;
};

class Database
{
	private:
		vector<fileLine>* rawFromfile;
		vector<tab1>* table1;
		vector<tab2>* table2;
		static inline struct fileLine parseLine(string line);
		static inline double calcAlphaInDeg(struct fileLine star1, struct fileLine star2);
		static inline double calcBetaInDeg (struct fileLine centerStar, struct fileLine star1, struct fileLine star2);
	public:
		void parseFromHipparcos(string filename, double maxMag);
		void rawToTable1(double distance);
		void fillTable2();
		struct fileLine getById(int ID);
		struct tab1 getTab1ById(int ID);
		struct tab2 findBestMatch(double beta, double alpha1, double alpha2);
		Database(string filenameHipparcos, double maxMag);
		~Database();
};

#endif // DATABASE_H
