#include "Pacman.h"
#include <iostream>

//Entry Point for Application
int main(int argc, char* argv[]) {
	Pacman* game = new Pacman(argc, argv);
	cin.get();
	return 0;
}