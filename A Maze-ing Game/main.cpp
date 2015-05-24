#include "MazeGameEngine.h"
#include <iostream>
#include "easylogging++.h"


INITIALIZE_EASYLOGGINGPP



int main(int argc, char * argv[])
{
	using namespace amazeinggame;
	try
	{
		START_EASYLOGGINGPP(argc, argv);
		el::Configurations conf("LoggerConfig.cfg");
		el::Loggers::reconfigureAllLoggers(conf);
		LOG(INFO) << "Starting game";
		CMazeGameEngine game;
		//game.initWorld(20, 20, 1, 3);
		//game.setupWorld();
		game.startEventLoop();
		return 0;
	}
	catch (std::exception exp)
	{
		LOG(FATAL) << exp.what();
		return 1;
	}
}