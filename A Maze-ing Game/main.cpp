#include "MazeGameEngine.h"
#include <iostream>


int main(int argc, char * argv[])
{
	CMazeGameEngine game;
	game.setMazeSize(20, 20);
	game.setupWorld();
	game.startEventLoop();
	return 0;
}