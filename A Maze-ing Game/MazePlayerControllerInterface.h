#pragma once

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

