#pragma once
#include "MazePlayerControllerInterface.h"

namespace amazeinggame
{

	class CMazePlayerAIController :
		public CMazePlayerControllerInterface
	{
	public:
		CMazePlayerAIController();
		virtual ~CMazePlayerAIController();
		void update(CMazePlayerModel * const in_playerModel) final;
	};
}
