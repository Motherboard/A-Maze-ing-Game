#pragma once

#include "IEventReceiver.h"

class CMazeGameEngine;
class CMazePlayerModel;

class CMazeGameEventReciever :
	public irr::IEventReceiver
{
public:
	CMazeGameEventReciever(CMazeGameEngine * const in_gameEngine, CMazePlayerModel * const in_mazePlayer);
	virtual ~CMazeGameEventReciever();
	bool OnEvent(const irr::SEvent& event) final;
private:
	CMazeGameEngine * const _parentGameEngine;
	CMazePlayerModel * const _mazePlayer;
};

