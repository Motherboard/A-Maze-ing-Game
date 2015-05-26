#pragma once

//This is an interface for a player model's controller
//The player model would call it's update function with it's "this" pointer, and the controller would
//tell the player what to do next.

namespace amazeinggame
{
	class CMazePlayerModel;

	class CMazePlayerControllerInterface
	{
	public:
		virtual void update(CMazePlayerModel * const in_playerModel) = 0;
		CMazePlayerControllerInterface();
		virtual ~CMazePlayerControllerInterface();
	};
}

