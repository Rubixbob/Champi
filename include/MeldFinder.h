#ifndef MELDFINDER_H
#define MELDFINDER_H

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

#include "CommonStructs.h"
#include "GearPiece.h"
#include "FullSet.h"

class MeldFinder
{
public:
	MeldFinder();
	virtual ~MeldFinder();

	void findBestMelds();

	FullSet* m_full_set;
	FullSet* m_saved_sets[100];
	vector<food*> m_foods;

	bool done;

protected:

private:
};

#endif
