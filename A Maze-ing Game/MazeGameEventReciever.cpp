#include "MazeGameEventReciever.h"
#include "MazeGameEngine.h"
#include "MazePlayerHumanController.h"
#include "irrlicht.h"
#include "easylogging++.h"

namespace amazeinggame
{


CMazeGameEventReciever::CMazeGameEventReciever(CMazeGameEngine * const in_gameEngine)
		: _parentGameEngine(in_gameEngine)
{

}


CMazeGameEventReciever::~CMazeGameEventReciever()
{
}

void CMazeGameEventReciever::setPlayerController(CMazePlayerHumanController * const in_playerController)
{
	_playerController = in_playerController;
}

bool CMazeGameEventReciever::OnEvent(const irr::SEvent& event)
{
	if (event.EventType == irr::EET_KEY_INPUT_EVENT)
	{
		if (_isActionPending)
			return false;
		
		if (!_parentGameEngine->isGameWorldInitialized())
		{
			_parentGameEngine->showMenu();// if the world is not initialized, show the menu
			return true;
		}
		
		switch (event.KeyInput.Key)
		{
			if (_playerController)
			{
				case irr::EKEY_CODE::KEY_UP: if (event.KeyInput.PressedDown) {

					_playerController->setDirection(maze::Direction::North);
				}
											 else
											 {
												 _playerController->stop();
											 } return true;
				case irr::EKEY_CODE::KEY_DOWN: if (event.KeyInput.PressedDown) {
					_playerController->setDirection(maze::Direction::South);
				}
											   else
											   {
												   _playerController->stop();
											   }return true;
				case irr::EKEY_CODE::KEY_LEFT: if (event.KeyInput.PressedDown) {
					_playerController->setDirection(maze::Direction::West);
				}
											   else
											   {
												   _playerController->stop();
											   }return true;
				case irr::EKEY_CODE::KEY_RIGHT: if (event.KeyInput.PressedDown) {
					_playerController->setDirection(maze::Direction::East);
				}
												else
												{
													_playerController->stop();
												}return true;
			}
		}
		if (event.KeyInput.Key == irr::EKEY_CODE::KEY_ESCAPE && event.KeyInput.PressedDown && _parentGameEngine)
		{
			_parentGameEngine->showMenu();
			return true;
		}
	}
	return false;

}
}