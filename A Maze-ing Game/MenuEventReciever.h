#pragma once
#include "IEventReceiver.h"

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

