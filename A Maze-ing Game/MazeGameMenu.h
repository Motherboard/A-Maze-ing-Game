#pragma once

//This class represents the graphical aspect of the GUI: it shows and hides menus and quesries the GUI elements 
//for their values.
//The CMenuEventReciever class is responsible for getting user events and handling the logic of what to do with them.

#include <vector>
#include <type_traits>
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
		//initialize the menu items 
		//in_guiEnv should point to an instance of GuiEnvironment obtained from the irrlicht device
		//in_menuRect specifies the region the menu will take up.
		void init(irr::gui::IGUIEnvironment * const in_guiEnv, const irr::core::recti & in_menuRect);
		void showMainMenu();
		bool isMenuShowing();
		void showVideoSettings();
		void showGameSettings();
		void hideMenu();
		//get the values for maze dimensions currently selected in the game settings menu
		//first is width, second is height
		std::pair<unsigned char,unsigned char> getChosenMazeDimensions();
		//get the resolution from the video settings menu
		//first is width, second is height
		std::pair<int, int> getChosenResolution();
		unsigned int getChosenNumOfAIPlayers();
		unsigned int getChosenAIDifficultyLevel();
		~CMazeGameMenu();
	private:
		void setMenuEnabled(bool in_menuState, std::vector <irr::gui::IGUIElement *> & in_menuContainer);
		template <typename T>
		std::remove_pointer_t<T> * getGuiElement(int in_id)
		{
			//check that T is a valid type to try to upcast during compile time.
			static_assert(std::is_base_of<irr::gui::IGUIElement, std::remove_pointer_t<T>>::value, 
				"T must inherit irr::gui::IGUIElement");
			if (!_guiEnv)
			{
				//LOG(ERROR) << "gui environment is null";
				throw std::exception("menu was not set correctly!");
			}
			if (!_guiEnv->getRootGUIElement())
			{
				//LOG(ERROR) << "No gui root element!";
				throw std::exception("no gui root element");
			}
			auto guiElement = _guiEnv->getRootGUIElement()->getElementFromId(in_id);
			if (!guiElement)
			{
				//LOG(ERROR) << "MazeWidth was not found!";
				throw std::exception("Could not find gui element");
			}
			auto requestedElement = dynamic_cast<std::remove_pointer_t<T> *>(guiElement);
			if (!requestedElement)
			{
				throw std::exception("Failed dynamic casting of GUI element");
			}
			return requestedElement;
		}
		std::vector <irr::gui::IGUIElement *> _mainMenuElements;
		std::vector <irr::gui::IGUIElement *> _videoSettingsElements;
		std::vector <irr::gui::IGUIElement *> _gameSettingsElements;
		irr::gui::IGUIElement * _menuBackground;
		enum class CurrentMenu {MainMenu, VideoSettings, GameSettings, None} _currentMenu;
		irr::gui::IGUIEnvironment * _guiEnv = nullptr;
		//some constants to control the size of gui elements
		const float ElementHeightRatio = 0.15;
		const float ElementSeparationRatio = 0.05;
	};

}