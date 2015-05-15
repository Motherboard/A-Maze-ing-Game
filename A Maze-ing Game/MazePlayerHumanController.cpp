#include "MazePlayerHumanController.h"
#include "MazePlayerModel.h"

namespace amazeinggame
{
	CMazePlayerHumanController::CMazePlayerHumanController()
	{
	}


	CMazePlayerHumanController::~CMazePlayerHumanController()
	{
	}

	void CMazePlayerHumanController::update(CMazePlayerModel * in_playerModel)
	{
		if (_isStopRequested)
		{
			in_playerModel->stop();
			_isStopRequested = false;
			return;
		}
		if (_isSetDirectionRequested)
		{
			in_playerModel->setDirection(_direction);
			_isSetDirectionRequested = false;
		}
	}

	void CMazePlayerHumanController::setDirection(maze::Direction in_direction)
	{
		_direction = in_direction;
		_isSetDirectionRequested = true;
	}
	void CMazePlayerHumanController::stop()
	{
		_isStopRequested = true;
	}
}