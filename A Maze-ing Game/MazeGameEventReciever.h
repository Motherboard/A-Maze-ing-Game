#pragma once

#include "IEventReceiver.h"


namespace amazeinggame
{

	class CMazeGameEngine;
	class CMazePlayerHumanController;

	class CMazeGameEventReciever :
		public irr::IEventReceiver
	{
	public:
		CMazeGameEventReciever(CMazeGameEngine * const in_gameEngine);
		void setPlayerController(CMazePlayerHumanController * const in_playerController);
		virtual ~CMazeGameEventReciever();
		bool OnEvent(const irr::SEvent& event) final;
	private:
		CMazeGameEngine * const _parentGameEngine = nullptr;
		CMazePlayerHumanController * _playerController = nullptr;
	};

}