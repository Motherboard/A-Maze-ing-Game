#include "MazeGameEngine.h"
#include <iostream>


int main(int argc, char * argv[])
{
	using namespace amazeinggame;
	try
	{
		CMazeGameEngine game;
		game.initWorld(20, 20, 0, 1);
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