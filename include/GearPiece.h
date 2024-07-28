#ifndef GEARPIECE_H
#define GEARPIECE_H

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <math.h>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System.hpp>

using namespace std;

class GearPiece
{
public:
	GearPiece();
	GearPiece(int t, string n, ifstream* file, int idx);
	GearPiece(const GearPiece& other);
	virtual ~GearPiece();

	int type;
	string name;
	int wd;
	int str;
	int mainStat;
	int subStat;
	int thirdStat;
	int fourthStat;
	int permutation;
	int index;
	int subs[4];

	int meldStatPermutation[9][4];
	int matStat[9][4];

	int meldSlots;
	int meldPermutations;

	bool isUnique;

	void init();

protected:

private:
};

#endif
