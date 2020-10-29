#ifndef HUD_H
#define HUD_H

#define SCORE_FILE "scores.txt"
#define MARK "|"
#define MAXSCORES 10

#include <iostream>
#include <fstream>
#include <string>

#include "Entity.h"

using namespace std;

struct Data {
	string name = "";
	int score = 0;
};

class HUD : public Entity
{
private:
	Data scores[MAXSCORES];

	Vector2* currentScorePos;
	Vector2* highScoresPos;

	Data currentScore;

	void DisplayScores();
	void SaveScores(string fileName = SCORE_FILE);
	int ScoreCount();
	void ShowMenu();	//dont need

public:
	HUD();
	~HUD();

	bool AddScore(Data newScore);
	void CurrentScore(int score);
	void Draw(int framecount = 0);
	Data GetInput(string newName, int newScore);
	bool LoadScores(string fileName = SCORE_FILE);
	void SortScores();
};

#endif