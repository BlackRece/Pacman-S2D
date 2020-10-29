#include "HUD.h"

HUD::HUD() {
	currentScorePos = new Vector2();
	highScoresPos = new Vector2();

	currentScorePos->X = 28 * 32;
	currentScorePos->Y = 16;

	highScoresPos->X = 28 * 32;;
	highScoresPos->Y = 48;

	/*
	1 - Enter score
		Retrieve name
		Retrieve score
		load scores (if not already loaded?)
		if new high score
		 - replace lowest score
		else
		 - discard new input
		display message
		re-sort high scores
		store scores if list has changed
		pause for input
		return to menu
	2 - Display Score
		load scores (if not already loaded?)
		if scores file empty display message
		pause for input
		return to menu
	3 - Exit
		exit
	*/
	/*
	int option = 0;
	bool hasFinished = false;

	while (!hasFinished) {
		//reset option
		option = 0;

		ShowMenu();

		cout << "\n\nPlease choose: ";

		//get input
		std::cin >> option;

		//test input
		switch (option) {
		case 1:	//Enter Score
			if (AddScore()) {

				SortScores();

				SaveScores();

			}
			break;
		case 2: //Display Score
			DisplayScores();
			break;
		case 3: //Exit
			hasFinished = true;
			break;
		default:
			cout <<
				"\nERROR: " << option << " is an invalid option!" <<
				"\nPress [ENTER] to retry." << endl;
			std::cin.get();

			break;
		}
	}
	*/
}

HUD::~HUD() {
	delete currentScorePos;
	delete highScoresPos;
}

bool HUD::AddScore(Data newScore) {
	bool wasAdded = false;

	LoadScores();

	if (scores[MAXSCORES - 1].score < newScore.score) {
		scores[MAXSCORES - 1].name = newScore.name;
		scores[MAXSCORES - 1].score = newScore.score;

		wasAdded = true;
	}

	return wasAdded;
}

void HUD::CurrentScore(int score) {
	currentScore.score = score;
}

void HUD::DisplayScores() {
	LoadScores();

	if (ScoreCount() > 0) {
		//heading
		cout <<
			"\n\tDisplaying Scores:" <<
			"\n\t==================\n" << endl;

		//scores
		for (int i = 0; i < MAXSCORES; i++) {
			cout <<
				"\t" << i << "\t" << scores[i].name << "\t-\t" << scores[i].score << endl;
		}

	}
	else {
		//message
		cout <<
			"\n\t====================\n" <<
			"\n\tNO SCORES TO DISPLAY\n" <<
			"\n\t====================\n" << endl;
	}

	//prompt & pause
	cout << "\nPress [ENTER] to return to the menu.\n" << endl;
}

void HUD::Draw(int framecount) {
	string tmp = "Score: " + to_string(currentScore.score);
	string pos = "";
	Vector2 tmpPos = *highScoresPos;
	SpriteBatch::DrawString(tmp.c_str(), currentScorePos, Color::Green);

	tmp = "Hi Scores:";
	SpriteBatch::DrawString(tmp.c_str(), highScoresPos, Color::Green);

	for (int i = 0; i < MAXSCORES; i++) {
		//pos = ()
		tmp = to_string(i+1) + " - " + to_string(scores[i].score);
		tmpPos.Y += 20;

		SpriteBatch::DrawString(tmp.c_str(), &tmpPos, Color::Green);
	}
}

Data HUD::GetInput(string newName, int newScore) {
	return Data{ newName, newScore };
}

bool HUD::LoadScores(string fileName) {
	ifstream fScore(fileName);
	string fLine = "";
	int fCount = 0, fMark = 0;

	if (!fScore) {
		return false;
	}

	while (getline(fScore, fLine)) {
		//find delimiter
		fMark = fLine.find_last_of(MARK);

		//store line into struct
		scores[fCount].name = fLine.substr(0, fMark);
		scores[fCount].score = stoi(fLine.substr(fMark + 1));

		//increment score counter
		if (fCount < MAXSCORES) {
			fCount++;
		} else {
			break;
		}
	}

	//close file
	fScore.close();

	//return result
	if (fCount > 0) {
		return true;
	}
	else {
		return false;
	}

}

void HUD::SaveScores(string fileName) {
	ofstream fScore(fileName, fstream::out | fstream::trunc);
	string fLine = "";

	if (fScore.is_open()) {
		//file exists and opened
		if (ScoreCount() > 0) {
			//scores available to be saved
			for (int i = 0; i < MAXSCORES; i++) {
				if (scores[i].score > 0) {
					fScore << scores[i].name << MARK << scores[i].score << endl;
				}
			}

		}

		fScore.close();
	}
}

int HUD::ScoreCount() {
	int count = 0;

	for (int i = 0; i < MAXSCORES; i++) {
		if (scores[i].score > -1) {
			count++;
		}
	}

	return count;
}

void HUD::ShowMenu() {
	//clear console
	system("cls");

	//title
	cout << "\t-============================-\n" << "\t |_.-{  TOP TEN SCORES  }-._|\n" << "\t-============================-" << endl;

	//menu
	cout << "\n\t\t[ MENU ]" <<
		"\n\t1.\tEnter Score" <<
		"\n\t2.\tDisplay Score" <<
		"\n\t3.\tExit" << endl;
}

void HUD::SortScores() {
	bool sorted = false;
	Data tmp;

	while (!sorted) {
		sorted = true;
		for (int i = 0; i < MAXSCORES; i++) {
			if (i != MAXSCORES - 1) {
				if (scores[i].score >= scores[i + 1].score) {
					//sorted
				}
				else {
					//unsorted - swap
					sorted = false;
					//store current position
					tmp.name = scores[i].name;
					tmp.score = scores[i].score;

					//move higher score
					scores[i].name = scores[i + 1].name;
					scores[i].score = scores[i + 1].score;

					//move lower score
					scores[i + 1].name = tmp.name;
					scores[i + 1].score = tmp.score;
				}
			}
		}
	}
}
