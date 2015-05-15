#include "MazeGameEngine.h"
#include <iostream>


int main(int argc, char * argv[])
{
	using namespace amazeinggame;
	CMazeGameEngine game;
	game.initWorld(10, 10,1,3);
	game.setupWorld();
	game.startEventLoop();
	return 0;
}