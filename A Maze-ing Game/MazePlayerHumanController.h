#pragma once
#include "MazePlayerControllerInterface.h"
#include "CMaze.h"

//This is the human controller for the player model that would be controlled by a human (duh)
//It's setDirection and stop functions would get called by the event reciever and it would tell the player what to do
//after the player calls update on itself (it's "this" pointer).

namespace amazeinggame
{

	class CMazePlayerHumanController :
		public CMazePlayerControllerInterface
	{
	public:
		void update(CMazePlayerModel * const in_playerModel) final;
		void setDirection(maze::Direction in_direction);
		void stop();
		CMazePlayerHumanController();
		virtual ~CMazePlayerHumanController();
	private:
		bool _isStopRequested = true;
		bool _isSetDirectionRequested = false;
		maze::Direction _direction;
	};
}

