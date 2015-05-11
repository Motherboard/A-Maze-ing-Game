#pragma once

#include "IEventReceiver.h"

class CMazeGameEngine;
class CMazePlayer;

class CMazeGameEventReciever :
	public irr::IEventReceiver
{
public:
	CMazeGameEventReciever(CMazeGameEngine * const in_gameEngine, CMazePlayer * const in_mazePlayer);
	virtual ~CMazeGameEventReciever();
	bool OnEvent(const irr::SEvent& event) final;
private:
	CMazeGameEngine * const _parentGameEngine;
	CMazePlayer * const _mazePlayer;
};

