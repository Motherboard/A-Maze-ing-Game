#pragma once
#include "IEventReceiver.h"

//This is the event reciever that gets activated when the game egine is in menu mode
//It holds the GUI logic

namespace amazeinggame
{
	class CMazeGameEngine;

	class CMenuEventReciever :
		public irr::IEventReceiver
	{
	public:
		CMenuEventReciever(CMazeGameEngine * const in_gameEngine);
		virtual ~CMenuEventReciever();
		bool OnEvent(const irr::SEvent& event) final;
	private:
		void startNewGame();
		void setResolution();
		CMazeGameEngine * const _parentGameEngine = nullptr;
	};
}

