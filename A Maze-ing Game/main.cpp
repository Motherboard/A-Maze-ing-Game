#include "MazeGameEngine.h"
#include <iostream>


int main(int argc, char * argv[])
{
	using namespace amazeinggame;
	try
	{
		CMazeGameEngine game;
		game.initWorld(20, 20, 1, 2);
		game.setupWorld();
		game.startEventLoop();
		return 0;
	}
	catch (std::exception exp)
	{
		std::cerr << exp.what() << std::endl;
		return 1;
	}
}