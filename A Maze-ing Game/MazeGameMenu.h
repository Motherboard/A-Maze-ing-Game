#pragma once

#include <vector>
#include "rect.h"

namespace irr
{
	namespace gui
	{
		class IGUIElement;
		class IGUIEnvironment;
	}

	namespace video
	{
		class IVideoDriver;
	}
}

namespace amazeinggame
{
	class CMazeGameEngine;

	class CMazeGameMenu
	{
	public:
		enum MenuElement {NewGameBtn, VideoSettingsBtn, GameSettingsBtn, QuitBtn, SetVideoSettingsBtn, BackToMainMenuBtn, SetGameSettingsBtn, BackToGameBtn, ResolutionDropDown, MazeWidth, MazeHeight, MazeNumOfAI, MazeAIDifficultyLevel};
		CMazeGameMenu();
		void init(irr::gui::IGUIEnvironment * const in_guiEnv, const irr::core::recti & in_menuRect);
		void showMainMenu();
		bool isMenuShowing();
		void showVideoSettings();
		void showGameSettings();
		void hideMenu();
		std::pair<unsigned char,unsigned char> getChosenMazeDimensions();
		std::pair<int, int> getChosenResolution();
		unsigned int getChosenNumOfAIPlayers();
		unsigned int getChosenAIDifficultyLevel();
		~CMazeGameMenu();
	private:
		void setMenuEnabled(bool in_menuState, std::vector <irr::gui::IGUIElement *> & in_menuContainer);
		std::vector <irr::gui::IGUIElement *> _mainMenuElements;
		std::vector <irr::gui::IGUIElement *> _videoSettingsElements;
		std::vector <irr::gui::IGUIElement *> _gameSettingsElements;
		enum class CurrentMenu {MainMenu, VideoSettings, GameSettings, None} _currentMenu;
		unsigned int _numOfAI, _difficultyLevel, resolutionWidth, resolutionHeight, mazeWidth, mazeHeight;
		irr::gui::IGUIEnvironment * _guiEnv = nullptr;
		//some constants to control the size of gui elements
		const float ElementHeightRatio = 0.15;
		const float ElementSeparationRatio = 0.05;
	};

}