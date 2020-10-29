#include "LevelManager.h"

LevelManager::LevelManager() {
	this->levelCounter = 1;
	ifstream mapFile(MAP_FILE);

	string rawRow = "";
	int rowCount = 0;
	
	this->wallCount = 0;
	this->munchieCount = 0;

	this->maxRows = MAP_ROWS;
	this->maxCols = MAP_COLS;


	if (mapFile.is_open()) {
		while (getline(mapFile, rawRow, '\n')) {
			for (int i = 0; i < MAP_COLS; i++) {
				if (rawRow.length() > i) {
					//store element to map array
					this->map[rowCount][i] = (char)rawRow.at(i);

					//count map elements
					switch (this->map[rowCount][i]) {
					case (char)TileType::tWALL:
						this->wallCount++;
						break;
					case (char)TileType::tPELLET:
					case (char)TileType::tMUNCHIE:
						this->munchieCount++;
						break;
					default:
						break;
					}
				}
			}

			rowCount++;

			rawRow.clear();
		}

		mapFile.close();
	}

}

LevelManager::~LevelManager(){}

/*
int LevelManager::CountTiles(char target) {
	int result = 0;

	switch (target) {
	case '#':
		if (this->wallCount > 0) {
			return this->wallCount;
		}

	case '+':
	case '.':
		if (this->munchieCount > 0) {
			return this->munchieCount;
		}
	default:
		for (int row = 0; row < MAP_ROWS; row++) {
			for (int col = 0; col < MAP_COLS; row++) {
				if (this->map[row][col] == target) {
					result++;
				}
			}
		}
		break;
	}

	return result;
}
*/

int LevelManager::CountTiles(TileType target) {
	int result = 0;

	switch (target) {
	case TileType::tWALL:
		if (this->wallCount > 0) {
			return this->wallCount;
		}

	case TileType::tMUNCHIE:
	case TileType::tPELLET:
		if (this->munchieCount > 0) {
			return this->munchieCount;
		}
	default:
		for (int row = 0; row < MAP_ROWS; row++) {
			for (int col = 0; col < MAP_COLS; row++) {
				if (this->map[row][col] == char(target)) {
					result++;
				}
			}
		}
		break;
	}

	return result;
}

void Draw(int elapsedTime) {}

char LevelManager::GetMapTile(int row, int col) {
	return this->map[row][col];
}

TileType LevelManager::GetTileType(int row, int col) {
	return (TileType)this->map[row][col];
}

Vector2i LevelManager::GetRandomTilePos(TileType tile) {
	int counter = 0;
	vector<Vector2i> tmpArray(0);
	Vector2i tmpVect = Vector2i();

	for (int row = 0; row < MAP_ROWS; row++) {
		for (int col = 0; col < MAP_COLS; col++) {
			if (map[row][col] == (char)tile) {
				tmpVect.X = col;
				tmpVect.Y = row;

				tmpArray.push_back(tmpVect);

				counter++;
			}
		}
	}

	return tmpArray[rand() % counter];
}

bool LevelManager::IsMapTile(int row, int col, TileType tile) {
	return (this->map[row][col] == (char)tile);
}

int LevelManager::MaxCols() { return this->maxCols; }

int LevelManager::MaxRows() { return this->maxRows; }
