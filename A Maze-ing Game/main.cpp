#include "MazeGameEngine.h"
#include <iostream>


int main(int argc, char * argv[])
{
	CMazeGameEngine game;
	game.setMazeSize(10, 10);
	game.setupWorld();
	game.startEventLoop();
	return 0;
}