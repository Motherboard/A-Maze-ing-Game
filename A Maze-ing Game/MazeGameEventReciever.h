#pragma once

#include "IEventReceiver.h"


//This event reciever is responsible for controlling a human player using the arrow keys, as well as bringing up the menu
//if one presses Esc. or any key after losing.

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
		//This flag is used to determine if a helper thread was started, and we should wait for it to finish it's job
		//The helper thread should reset the flag once it's done (we don't care if we lose some events due to racing 
		//conditions, so no need for locks
		bool _isActionPending = false;
	};

}