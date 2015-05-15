#pragma once
#include "MazePlayerControllerInterface.h"
#include "CMaze.h"

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

