#ifndef BISFINDER_H
#define BISFINDER_H

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

#include "MeldFinder.h"
#include "GearPiece.h"
#include "CommonStructs.h"
#include "FullSet.h"

using namespace std;

class BiSFinder
{
public:
	BiSFinder();
	virtual ~BiSFinder();
	static BiSFinder& Instance() { return m_instance; }
	void init();
	void findBiS();
	void displayResults();
protected:
private:
	void loadConfig(const string& configFilePath);
	void loadGearPieces(const string& gearFilePath);
	void initializeFullSet();
	void loadFoodData(const string& foodFilePath);
	void initializeMultipliers(const string& critMultFilePath);
	vector<GearPiece*> m_gearPieces[11];
	FullSet* m_saved_sets[100];
	vector<food> m_foods;
	FullSet m_full_set;
	void addExclusion(string name, int type);
	bool isExcluded(string name, int type);

	static BiSFinder m_instance;
};

#endif
