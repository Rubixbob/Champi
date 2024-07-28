#include "MeldFinder.h"

#define MAT 54
#define MIN_SKS 420
#define MAX_SKS 5000

MeldFinder::MeldFinder() :done(false)
{
	//ctor
	for (int i = 0; i < 100; i++) {
		m_saved_sets[i] = nullptr;
	}
}

MeldFinder::~MeldFinder()
{
	//dtor
	delete m_full_set;
	for (int i = 0; i < 100; i++) {
		if (m_saved_sets[i])
			delete m_saved_sets[i];
	}
}

void MeldFinder::findBestMelds()
{
	int maxMeldSlots = 0;
	for (int i = 0; i < 12; i++)
	{
		if (i == 4) continue;
		maxMeldSlots += m_full_set->gear_set[i]->meldSlots;
	}

	statStruct gearStats;
	gearStats.wd = 0;
	gearStats.str = baseMainStat;
	gearStats.subs[0] = 420;
	gearStats.subs[1] = 420;
	gearStats.subs[2] = 440;
	gearStats.subs[3] = 420;
	for (int i = 0; i < 12; i++)
	{
		if (i == 4) continue;
		GearPiece* piece = m_full_set->gear_set[i];

		gearStats.wd += piece->wd;
		gearStats.str += piece->str;
		for (int k = 0; k < 4; k++)
		{
			gearStats.subs[k] += piece->subs[k];
		}
	}

	// Meld combinations init
	int meldComb[4];
	int meldCombOld[4];
	meldComb[3] = maxMeldSlots;
	for (int i = 0; i < 3; i++)
	{
		meldComb[i] = 0;
	}
	for (int i = 0; i < 4; i++)
	{
		meldCombOld[i] = meldComb[i];
	}

	// Materia slots init
	int availableSlots[4];
	int availableSlotsInit[4];
	int neededSlots[4];

	for (int i = 0; i < 4; i++)
	{
		availableSlotsInit[i] = 0;
	}
	for (int i = 0; i < 12; i++)
	{
		if (i == 4) continue;
		// TODO: check submeldtable change
		GearPiece* piece = m_full_set->gear_set[i];
		availableSlotsInit[piece->subStat] += round(1.0 * min(piece->subs[piece->mainStat] - piece->subs[piece->subStat], piece->meldSlots * MAT) / MAT);
		availableSlotsInit[piece->thirdStat] += piece->meldSlots;
		availableSlotsInit[piece->fourthStat] += piece->meldSlots;
	}

	// Score init
	int maxScore = maxMeldSlots * MAT;

	vector<vector<vector<vector<int>>>> savedScores(maxMeldSlots + 1, vector<vector<vector<int>>>(maxMeldSlots + 1, vector<vector<int>>(maxMeldSlots + 1, vector<int>(maxMeldSlots + 1, (maxMeldSlots)))));
	//int savedScores[maxMeldSlots + 1][maxMeldSlots + 1][maxMeldSlots + 1][maxMeldSlots + 1];

	for (int i = 0; i <= maxMeldSlots; i++)
	{
		for (int j = 0; j <= maxMeldSlots; j++)
		{
			for (int k = 0; k <= maxMeldSlots; k++)
			{
				for (int l = 0; l <= maxMeldSlots; l++)
				{
					savedScores[i][j][k][l] = -1;
				}
			}
		}
	}

	while (meldComb[0] <= maxMeldSlots)
	{
		bool possible = true;
		// Init
		for (int i = 0; i < 4; i++)
		{
			neededSlots[i] = meldComb[i];
			availableSlots[i] = availableSlotsInit[i];
			if (neededSlots[i] > availableSlots[i])
				possible = false;
		}

		if (possible)
		{
			// Check DPS with perfect melds
			statStruct maxMeldedGearStat;
			maxMeldedGearStat.wd = gearStats.wd;
			maxMeldedGearStat.str = gearStats.str;
			for (int i = 0; i < 4; i++)
			{
				maxMeldedGearStat.subs[i] = gearStats.subs[i] + meldComb[i] * MAT;
			}

			possible = false;
			// Iterate on foods
			for (unsigned int j = 0; j < m_foods.size(); j++)
			{
				statStruct maxFoodMeldedGearStat;
				maxFoodMeldedGearStat.wd = maxMeldedGearStat.wd;
				// Party bonus
				maxFoodMeldedGearStat.str = maxMeldedGearStat.str * 1.05;
				for (int i = 0; i < 4; i++)
				{
					maxFoodMeldedGearStat.subs[i] = maxMeldedGearStat.subs[i] + min(maxMeldedGearStat.subs[i] * m_foods[j]->mult[i], m_foods[j]->cap[i]);
				}
				// Compute dps and compare to saved sets
				if (maxFoodMeldedGearStat.subs[3] >= MIN_SKS && gearStats.subs[3] <= MAX_SKS)
				{
					double dps = dirMult[maxFoodMeldedGearStat.subs[0]] * critMult[maxFoodMeldedGearStat.subs[1]] * detMult[maxFoodMeldedGearStat.subs[2]] * sksMult[maxFoodMeldedGearStat.subs[3]] * strMult[maxFoodMeldedGearStat.str];
					unsigned int gcd = floor(floor(2.5 * (1000 - floor(130 * (maxFoodMeldedGearStat.subs[3] - 420) / 2780))) * (100 - haste) / 1000);
					unsigned int minGcd = floor(floor(2.5 * (1000 - floor(130 * (gearStats.subs[3] - 420) / 2780))) * (100 - haste) / 1000);
					if (gearStats.subs[3] < MIN_SKS)
						minGcd = floor(floor(2.5 * (1000 - floor(130 * (MIN_SKS - 420) / 2780))) * (100 - haste) / 1000);
					int idx = 250 - gcd;
					int minIdx = 250 - minGcd;
					// Check all saved dps within gcd range
					for (int i = minIdx; i <= idx; i++)
					{
						if (!m_saved_sets[i] || dps > m_saved_sets[i]->dps)
							possible = true;
					}
				}
			}
		}

		if (possible)
		{
			// Find melds
			int currentPerm[12];
			int savedPerm[12];
			for (int i = 0; i < 12; i++) {
				currentPerm[i] = 0;
				savedPerm[i] = 0;
			}
			int idx = 0;
			int score = 0;
			int savedScore = 0;
			int scoreGoal = 0;
			bool scoreGiven = false;
			while (idx < 12 && idx >= 0)
			{
				if (idx == 4)
					cout << "idx = 4, we shouldn't be there" << endl;
				GearPiece* piece = m_full_set->gear_set[idx];
				// Check if perm can be used
				bool useful = true;
				for (int i = 0; i < 4; i++)
				{
					if (piece->meldStatPermutation[currentPerm[idx]][i] > neededSlots[i])
						useful = false;
				}
				if (useful)
				{
					int currentSavedScore = savedScores[neededSlots[0]][neededSlots[1]][neededSlots[2]][neededSlots[3]];
					int nextScoreGoal = currentSavedScore - piece->matStat[currentPerm[idx]][0]
						- piece->matStat[currentPerm[idx]][1]
						- piece->matStat[currentPerm[idx]][2]
						- piece->matStat[currentPerm[idx]][3];
					int nextSavedScore = savedScores[neededSlots[0] - piece->meldStatPermutation[currentPerm[idx]][0]]
						[neededSlots[1] - piece->meldStatPermutation[currentPerm[idx]][1]]
						[neededSlots[2] - piece->meldStatPermutation[currentPerm[idx]][2]]
						[neededSlots[3] - piece->meldStatPermutation[currentPerm[idx]][3]];
					if (scoreGiven)
					{
						if (idx < 11 && nextSavedScore < nextScoreGoal)
							useful = false;
					}
					else
					{
						if (nextSavedScore > 0)
						{
							if (nextSavedScore > nextScoreGoal)
							{
								scoreGiven = true;
								scoreGoal = nextSavedScore;
							}
							else
								useful = false;
						}
						else if (nextSavedScore == 0)
							useful = false;
					}
				}

				// If useful, take the slots and go to next piece
				if (useful)
				{
					for (int i = 0; i < 4; i++)
					{
						availableSlots[i] -= piece->meldStatPermutation[currentPerm[idx]][i];
						neededSlots[i] -= piece->meldStatPermutation[currentPerm[idx]][i];
					}
					idx++;
					if (idx == 4) idx++;
					piece = m_full_set->gear_set[idx];
					if (idx >= 12)
					{
						score = 0;
						int path[4];
						for (int i = 0; i < 4; i++)
						{
							path[i] = 0;
						}
						bool saving = true;
						for (int j = 11; j >= 0; j--)
						{
							if (j == 4) continue;
							// Save score on the path
							for (int i = 0; i < 4; i++)
							{
								score += m_full_set->gear_set[j]->matStat[currentPerm[j]][i];
								path[i] += m_full_set->gear_set[j]->meldStatPermutation[currentPerm[j]][i];
							}
							if (saving && (int)score < savedScores[path[0]][path[1]][path[2]][path[3]])
								saving = false;
							if (saving)
								savedScores[path[0]][path[1]][path[2]][path[3]] = score;
						}
						if (score < maxScore)
						{
							// Compare to saved score and save
							if (score > savedScore)
							{
								//                                cout << "Saving score" << endl;
								for (int i = 0; i < 12; i++)
								{
									savedPerm[i] = currentPerm[i];
								}
								savedScore = score;
							}
							useful = false;
							idx--;
							piece = m_full_set->gear_set[idx];
							for (int i = 0; i < 4; i++)
							{
								availableSlots[i] += piece->meldStatPermutation[currentPerm[idx]][i];
								neededSlots[i] += piece->meldStatPermutation[currentPerm[idx]][i];
							}
							if (scoreGiven)
								currentPerm[idx] = piece->meldPermutations;
						}
					}
				}
				// If not, go to next perm or up
				if (!useful)
				{
					currentPerm[idx]++;
					while (idx >= 0 && (currentPerm[idx] >= piece->meldPermutations))
					{
						currentPerm[idx] = 0;
						if (savedScores[neededSlots[0]][neededSlots[1]][neededSlots[2]][neededSlots[3]] < 0)
							savedScores[neededSlots[0]][neededSlots[1]][neededSlots[2]][neededSlots[3]] = 0;
						else if (savedScores[neededSlots[0]][neededSlots[1]][neededSlots[2]][neededSlots[3]] == scoreGoal)
						{
							scoreGoal = 0;
							scoreGiven = false;
						}
						idx--;
						if (idx == 4) idx--;
						piece = m_full_set->gear_set[idx];
						if (idx >= 0)
						{
							// Give back slots and go to next perm
							for (int i = 0; i < 4; i++)
							{
								availableSlots[i] += piece->meldStatPermutation[currentPerm[idx]][i];
								neededSlots[i] += piece->meldStatPermutation[currentPerm[idx]][i];
							}
							// If score is given and we're here, score has been found and we're going up
							if (scoreGiven)
								currentPerm[idx] = piece->meldPermutations;
							else
								currentPerm[idx]++;
						}
					}
				}
			}

			if (score == maxScore)
			{
				for (int i = 0; i < 12; i++)
				{
					savedPerm[i] = currentPerm[i];
				}
				savedScore = score;
			}

			// Create set + melds
			if (savedScore > 0)
			{
				for (int i = 0; i < 12; i++)
				{
					if (i == 4) continue;
					for (int j = 0; j < 5; j++)
					{
						m_full_set->gearMeldTable[i][j] = -1;
					}
				}
				for (int j = 0; j < 12; j++)
				{
					if (j == 4) continue;

					for (int k = 0; k < m_full_set->gear_set[j]->meldSlots; k++)
					{
						int meldCpt = 0;
						for (int i = 0; i < 4; i++)
						{
							if (m_full_set->gearMeldTable[j][k] < 0 && m_full_set->gear_set[j]->meldStatPermutation[savedPerm[j]][i] > 0)
							{
								meldCpt += m_full_set->gear_set[j]->meldStatPermutation[savedPerm[j]][i];
								if (meldCpt > k)
									m_full_set->gearMeldTable[j][k] = i;
							}
						}
					}
				}

				statStruct meldedGearStat;
				meldedGearStat.wd = gearStats.wd;
				meldedGearStat.str = gearStats.str;
				for (int i = 0; i < 4; i++)
				{
					meldedGearStat.subs[i] = gearStats.subs[i];
				}
				for (int j = 0; j < 12; j++)
				{
					if (j == 4) continue;
					for (int i = 0; i < 4; i++)
					{
						meldedGearStat.subs[i] += m_full_set->gear_set[j]->matStat[savedPerm[j]][i];
					}
				}

				// Iterate on foods
				for (unsigned int j = 0; j < m_foods.size(); j++)
				{
					statStruct foodMeldedGearStat;
					foodMeldedGearStat.wd = meldedGearStat.wd;

					// Party bonus
					foodMeldedGearStat.str = meldedGearStat.str * 1.05;
					for (int i = 0; i < 4; i++)
					{
						foodMeldedGearStat.subs[i] = meldedGearStat.subs[i] + min(meldedGearStat.subs[i] * m_foods[j]->mult[i], m_foods[j]->cap[i]);
					}

					// Compute dps and compare to saved sets
					if (foodMeldedGearStat.subs[3] >= MIN_SKS && foodMeldedGearStat.subs[3] <= MAX_SKS) {
						double dps = dirMult[foodMeldedGearStat.subs[0]] * critMult[foodMeldedGearStat.subs[1]] * detMult[foodMeldedGearStat.subs[2]] * sksMult[foodMeldedGearStat.subs[3]] * strMult[foodMeldedGearStat.str];
						unsigned int gcd = floor(floor(2.5 * (1000 - floor(130 * (foodMeldedGearStat.subs[3] - 420) / 2780))) * (100 - haste) / 1000);
						int idx = 250 - gcd;
						if (!m_saved_sets[idx] || dps > m_saved_sets[idx]->dps)
						{
							// Create set
							m_full_set->food = m_foods[j];
							m_full_set->foodMeldedGearStats.wd = foodMeldedGearStat.wd;
							m_full_set->meldedGearStats.str = meldedGearStat.str;
							m_full_set->foodMeldedGearStats.str = foodMeldedGearStat.str;
							for (int i = 0; i < 4; i++)
							{
								m_full_set->meldedGearStats.subs[i] = meldedGearStat.subs[i];
								m_full_set->foodMeldedGearStats.subs[i] = foodMeldedGearStat.subs[i];
							}
							m_full_set->dps = dps;
							m_full_set->gcd = gcd;

							// TODO: looks bad
							if (m_saved_sets[idx]) {
								delete m_saved_sets[idx];
							}
							m_saved_sets[idx] = new FullSet(*m_full_set);
						}
					}
				}
			}
		}

		meldComb[0] = (meldCombOld[0] + meldCombOld[1] == maxMeldSlots) ? (meldCombOld[0] + 1) : meldCombOld[0];
		meldComb[1] = (meldComb[0] == meldCombOld[0]) ? (meldCombOld[3] == 0 ? meldCombOld[1] + 1 : meldCombOld[1]) : 0;
		meldComb[2] = (meldComb[1] == meldCombOld[1]) ? meldCombOld[2] + 1 : 0;
		meldComb[3] = maxMeldSlots - meldComb[0] - meldComb[1] - meldComb[2];
		for (int i = 0; i < 4; i++) {
			meldCombOld[i] = meldComb[i];
		}
	}

	mutex_active_threads.lock();
	active_threads--;
	mutex_active_threads.unlock();
	done = true;
}
