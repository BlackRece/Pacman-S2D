#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#define MAX_ELEMENTS 644
#define MAP_ROWS 23
#define MAP_COLS 27
#define MAP_FILE "level_0.txt"

#include <fstream>
#include <sstream>
#include "Vector2i.h"

using namespace std;

enum class TileType : char {
	//obstacles
	tWALL = '#',
	tGATE = '=',
	
	//collectables
	tBONUS = 'B',
	tMUNCHIE = '.',
	tPELLET = '+',

	//players
	tGHOST = 'G',
	tPLAYER = 'P',

	//directions
	tLEFT = '<',
	tRIGHT = '>',
	tUP = '^'
};

class LevelManager
{
private:
	int wallCount;
	int munchieCount;
	int maxRows;
	int maxCols;
	char map[MAP_ROWS][MAP_COLS];

	char GetMapTile(int row, int col);

public:
	LevelManager();
	~LevelManager();

	int levelCounter;

	int CountTiles(TileType target);

	TileType GetTileType(int row, int col);

	Vector2i GetRandomTilePos(TileType tile);

	bool IsMapTile(int row, int col, TileType tile);

	int MaxRows();

	int MaxCols();

	// do I need these?
	//void Draw(int elapsedTime);

	//void LoadMap();

	//ghost class only
	//Movement NotSoRandomMotion(Vector2i pos);

	//void Update(int elapsedTime);
};

#endif