#include "MazeGameEventReciever.h"
#include "MazeGameEngine.h"
#include "MazePlayerModel.h"
#include "irrlicht.h"

CMazeGameEventReciever::CMazeGameEventReciever(CMazeGameEngine * const in_gameEngine, CMazePlayerModel * const in_mazePlayer)
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
				_mazePlayer->setDirection(Maze::Direction::North);
			}
									 else
										 _mazePlayer->stop(); break;
			case irr::EKEY_CODE::KEY_DOWN: if (event.KeyInput.PressedDown) {
				_mazePlayer->setDirection(Maze::Direction::South);
			}
										   else
											   _mazePlayer->stop(); break;
			case irr::EKEY_CODE::KEY_LEFT: if (event.KeyInput.PressedDown) {
				_mazePlayer->setDirection(Maze::Direction::West); 
			}
										   else
											   _mazePlayer->stop(); break;
			case irr::EKEY_CODE::KEY_RIGHT: if (event.KeyInput.PressedDown) {
				_mazePlayer->setDirection(Maze::Direction::East);
			}
											else
												_mazePlayer->stop(); break;
			case irr::EKEY_CODE::KEY_ESCAPE: if (event.KeyInput.PressedDown) _parentGameEngine->showMenu();
		default: return false;
		}
	}
	return true;
}