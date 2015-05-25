#include "MenuEventReciever.h"
#include "MazeGameEngine.h"
#include "irrlicht.h"
#include "MazeGameMenu.h"
#include "easylogging++.h"

namespace amazeinggame
{

	CMenuEventReciever::CMenuEventReciever(CMazeGameEngine * const in_gameEngine)
		: _parentGameEngine(in_gameEngine)
	{
	}


	CMenuEventReciever::~CMenuEventReciever()
	{
	}

	void CMenuEventReciever::startNewGame()
	{
		auto mazeDimensions = _parentGameEngine->menu.getChosenMazeDimensions();
		auto numOfAI = _parentGameEngine->menu.getChosenNumOfAIPlayers();
		auto AIDifficultyLevel = _parentGameEngine->menu.getChosenAIDifficultyLevel();
		_parentGameEngine->initWorld(mazeDimensions.first, mazeDimensions.second, numOfAI, AIDifficultyLevel);
		_parentGameEngine->setupWorld();
	}

	void CMenuEventReciever::setResolution()
	{
		auto resolution = _parentGameEngine->menu.getChosenResolution();
		_parentGameEngine->setResulotion(resolution.first, resolution.second);
	}

	bool CMenuEventReciever::OnEvent(const irr::SEvent& event)
	{
		if (event.EventType == irr::EEVENT_TYPE::EET_GUI_EVENT)
		{
			if (event.GUIEvent.EventType != irr::gui::EGET_BUTTON_CLICKED)
			{
				//LOG(DEBUG) << "Got GUI event which is not buton clicked: " << event.GUIEvent.EventType;
				return false;
			}
			switch (event.GUIEvent.Caller->getID())
			{
			case CMazeGameMenu::MenuElement::BackToGameBtn:
				if (_parentGameEngine->isGameOngoing()) { _parentGameEngine->hideMenu(); } break;
			case CMazeGameMenu::MenuElement::BackToMainMenuBtn:
				_parentGameEngine->menu.showMainMenu(); break;
			case CMazeGameMenu::MenuElement::GameSettingsBtn:
				_parentGameEngine->menu.showGameSettings(); break;
			case CMazeGameMenu::MenuElement::NewGameBtn:
				startNewGame(); break;
			case CMazeGameMenu::MenuElement::QuitBtn:
				_parentGameEngine->quit(); break;
			case CMazeGameMenu::MenuElement::SetGameSettingsBtn:
				startNewGame(); break;
			case CMazeGameMenu::MenuElement::SetVideoSettingsBtn:
				setResolution(); break;
			case CMazeGameMenu::MenuElement::VideoSettingsBtn:
				_parentGameEngine->menu.showVideoSettings(); break;
			default:
				return false;
			}
			return true;
		}
		else
		{
			//LOG(DEBUG) << "Got a non-gui event: " << event.EventType;
			return false;
		}
	}
}