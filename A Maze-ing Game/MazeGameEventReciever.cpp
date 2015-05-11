#include "MazeGameEventReciever.h"
#include "MazeGameEngine.h"
#include "MazePlayer.h"
#include "irrlicht.h"

CMazeGameEventReciever::CMazeGameEventReciever(CMazeGameEngine * const in_gameEngine, CMazePlayer * const in_mazePlayer)
	: _parentGameEngine(in_gameEngine), _mazePlayer(in_mazePlayer)
{

}


CMazeGameEventReciever::~CMazeGameEventReciever()
{
}

bool CMazeGameEventReciever::OnEvent(const irr::SEvent& event)
{
	if (event.EventType == irr::EET_KEY_INPUT_EVENT)
	{
		switch (event.KeyInput.Key)
		{
			case irr::EKEY_CODE::KEY_UP: if (event.KeyInput.PressedDown) {
				_mazePlayer->setDirection(CMazePlayer::MovementDirection::North);
			}
									 else
										 _mazePlayer->stop(); break;
			case irr::EKEY_CODE::KEY_DOWN: if (event.KeyInput.PressedDown) {
				_mazePlayer->setDirection(CMazePlayer::MovementDirection::South);
			}
										   else
											   _mazePlayer->stop(); break;
			case irr::EKEY_CODE::KEY_LEFT: if (event.KeyInput.PressedDown) {
				_mazePlayer->setDirection(CMazePlayer::MovementDirection::West); 
			}
										   else
											   _mazePlayer->stop(); break;
			case irr::EKEY_CODE::KEY_RIGHT: if (event.KeyInput.PressedDown) {
				_mazePlayer->setDirection(CMazePlayer::MovementDirection::East);
			}
											else
												_mazePlayer->stop(); break;
			case irr::EKEY_CODE::KEY_ESCAPE: if (event.KeyInput.PressedDown) _parentGameEngine->showMenu();
		default: return false;
		}
	}
	return true;
}