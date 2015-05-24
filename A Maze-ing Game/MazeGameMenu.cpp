#include "MazeGameMenu.h"
#include "irrlicht.h"
#include "easylogging++.h"

namespace amazeinggame
{

	CMazeGameMenu::CMazeGameMenu()
	{
	}


	CMazeGameMenu::~CMazeGameMenu()
	{
	}

	void CMazeGameMenu::init(irr::gui::IGUIEnvironment * const in_guiEnv, const irr::core::recti & in_menuRect)
	{
		LOG(DEBUG) << "Initializing menu";
		_guiEnv = in_guiEnv;
		//set the main menu.
		auto in_videoDriver = in_guiEnv->getVideoDriver();
		irr::core::dimension2di elementSize = in_menuRect.getSize(); 
		elementSize.Height *= ElementHeightRatio;
		int elementSeperation = in_menuRect.getHeight() * ElementSeparationRatio;
		irr::core::vector2di elementPosition(in_menuRect.UpperLeftCorner);
		auto button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::BackToGameBtn,L"Back to Game");
		//button->setImage(in_videoDriver->getTexture("../media/backToGame.jpg"));
		_mainMenuElements.push_back(button);
		elementPosition.Y += elementSeperation + elementSize.Height;
		button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::NewGameBtn, L"New Game");
		//button->setImage(in_videoDriver->getTexture("../media/newGame.jpg"));
		_mainMenuElements.push_back(button);
		elementPosition.Y += elementSeperation + elementSize.Height;
		button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::GameSettingsBtn, L"Game Settings");
		//button->setImage(in_videoDriver->getTexture("../media/gameSettings.jpg"));
		_mainMenuElements.push_back(button);
		elementPosition.Y += elementSeperation + elementSize.Height;
		button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::VideoSettingsBtn, L"Video Settings");
		//button->setImage(in_videoDriver->getTexture("../media/videoSettings.jpg"));
		_mainMenuElements.push_back(button);
		elementPosition.Y += elementSeperation + elementSize.Height;
		button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::QuitBtn, L"Quit");
		//button->setImage(in_videoDriver->getTexture("../media/quitGame.jpg"));
		_mainMenuElements.push_back(button);
		//set the video settings menu
		elementPosition.Y = in_menuRect.UpperLeftCorner.Y;
		auto resImage = in_guiEnv->addImage(irr::core::recti(elementPosition, elementSize));
		//resImage->setImage(in_videoDriver->getTexture("../media/Resolution.jpg"));
		_videoSettingsElements.push_back(resImage);
		elementPosition.Y += elementSeperation + elementSize.Height;
		auto listBox = in_guiEnv->addListBox(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::ResolutionDropDown, true);
		listBox->addItem(L"800x600");
		listBox->addItem(L"1024x768");
		listBox->addItem(L"1200x1600");
		listBox->setSelected(1);
		_videoSettingsElements.push_back(listBox);
		//add buttons at the bottom to both game and setting menu
		elementPosition.Y = in_menuRect.UpperLeftCorner.Y + in_menuRect.getHeight();
		elementSize.Width = (elementSize.Width - elementSeperation) / 2.0f;
		button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::BackToMainMenuBtn, L"Back to Main Menu");
		//button->setImage(in_videoDriver->getTexture("../media/backToMainMenu.jpg"));
		_videoSettingsElements.push_back(button);
		_gameSettingsElements.push_back(button);
		elementPosition.X += (elementSize.Width + elementSeperation) / 2.0f;
		button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::SetVideoSettingsBtn, L"Apply (restart game)");
		//button->setImage(in_videoDriver->getTexture("../media/setVideoSettings.jpg"));
		_videoSettingsElements.push_back(button);
		button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::SetGameSettingsBtn, L"Apply (restart game)");
		//button->setImage(in_videoDriver->getTexture("../media/setGameSettings.jpg"));
		_gameSettingsElements.push_back(button);
		//set the game settings menu
		elementPosition = in_menuRect.UpperLeftCorner;
		auto spinBox = in_guiEnv->addSpinBox(L"Number of AI players", irr::core::recti(elementPosition, elementSize),true,nullptr,MenuElement::MazeNumOfAI);
		spinBox->setDecimalPlaces(0);
		spinBox->setRange(0.0f, 10.0f);
		spinBox->setValue(3.0f);
		_gameSettingsElements.push_back(spinBox);
		elementPosition.Y += elementSeperation + elementSize.Height;
		spinBox = in_guiEnv->addSpinBox(L"AI level", irr::core::recti(elementPosition, elementSize), true, nullptr, MenuElement::MazeAIDifficultyLevel);
		spinBox->setDecimalPlaces(0);
		spinBox->setRange(1.0f, 10.0f);
		spinBox->setValue(3.0f);
		_gameSettingsElements.push_back(spinBox);
		elementPosition.Y += elementSeperation + elementSize.Height;
		spinBox = in_guiEnv->addSpinBox(L"Maze Width", irr::core::recti(elementPosition, elementSize), true, nullptr, MenuElement::MazeWidth);
		spinBox->setDecimalPlaces(0);
		spinBox->setRange(10.0f, 40.0f);
		spinBox->setStepSize(2.0f);
		spinBox->setValue(20.0f);
		_gameSettingsElements.push_back(spinBox);
		elementPosition.Y += elementSeperation + elementSize.Height;
		spinBox = in_guiEnv->addSpinBox(L"Maze Height", irr::core::recti(elementPosition, elementSize), true, nullptr, MenuElement::MazeHeight);
		spinBox->setDecimalPlaces(0);
		spinBox->setRange(10.0f, 40.0f);
		spinBox->setStepSize(2.0f);
		spinBox->setValue(20.0f);
		_gameSettingsElements.push_back(spinBox);
		
		//hide all menu elements
		setMenuEnabled(false, _mainMenuElements);
		setMenuEnabled(false, _gameSettingsElements);
		setMenuEnabled(false, _videoSettingsElements);
		_currentMenu = CurrentMenu::None;

		LOG(DEBUG) << "Finished initializing menu";
	}

	void CMazeGameMenu::showMainMenu()
	{
		if (_currentMenu == CurrentMenu::MainMenu)
		{ 
			return; //already showing the main manu
		}
		LOG(DEBUG) << "Showing main menu";
		switch (_currentMenu)
		{
		case CurrentMenu::GameSettings: 
			setMenuEnabled(false, _gameSettingsElements);
			break;
		case CurrentMenu::VideoSettings:
			setMenuEnabled(false, _videoSettingsElements);
			break;
		default:
			break;
		}
		setMenuEnabled(true, _mainMenuElements);
		_currentMenu = CurrentMenu::MainMenu;
	}


	bool CMazeGameMenu::isMenuShowing()
	{
		return _currentMenu != CurrentMenu::None;
	}


	void CMazeGameMenu::showVideoSettings()
	{
		if (_currentMenu == CurrentMenu::VideoSettings)
		{ 
			return; //already showing the video settings
		}
		LOG(DEBUG) << "Showing video settings";
		switch (_currentMenu)
		{
		case CurrentMenu::GameSettings:
			setMenuEnabled(false, _gameSettingsElements);
			break;
		case CurrentMenu::MainMenu:
			setMenuEnabled(false, _mainMenuElements);
			break;
		default:
			break;
		}
		setMenuEnabled(true, _videoSettingsElements);
		_currentMenu = CurrentMenu::VideoSettings;
	}


	void CMazeGameMenu::showGameSettings()
	{
		if (_currentMenu == CurrentMenu::GameSettings)
		{
			return; //already showing the game settings
		}
		LOG(DEBUG) << "Showing game settings";
		switch (_currentMenu)
		{
		case CurrentMenu::MainMenu:
			setMenuEnabled(false, _mainMenuElements);
			break;
		case CurrentMenu::VideoSettings:
			setMenuEnabled(false, _videoSettingsElements);
			break;
		default:
			break;
		}
		setMenuEnabled(true, _gameSettingsElements);
		
		_currentMenu = CurrentMenu::GameSettings;
	}


	void CMazeGameMenu::hideMenu()
	{
		if (_currentMenu == CurrentMenu::None)
		{
			return; //already showing no menu
		}
		LOG(DEBUG) << "Hiding menu";
		switch (_currentMenu)
		{
		case CurrentMenu::MainMenu:
			setMenuEnabled(false, _mainMenuElements);
			break;
		case CurrentMenu::VideoSettings:
			setMenuEnabled(false, _videoSettingsElements);
			break;
		case CurrentMenu::GameSettings:
			setMenuEnabled(false, _gameSettingsElements);
			break;
		default: //this would never happen...
			break;
		}
		_currentMenu = CurrentMenu::None;
	}


	std::pair<unsigned char, unsigned char> CMazeGameMenu::getChosenMazeDimensions()
	{
		try
		{
			if (!_guiEnv)
			{
				LOG(ERROR) << "gui environment is null";
				throw std::exception("menu was not set correctly!");
			}
			if (!_guiEnv->getRootGUIElement())
			{
				LOG(ERROR) << "No gui root element!";
				throw std::exception("no gui root element");
			}
			auto mazeWidthGuiElement = _guiEnv->getRootGUIElement()->getElementFromId(MenuElement::MazeWidth);
			if (!mazeWidthGuiElement)
			{
				LOG(ERROR) << "MazeWidth was not found!";
				throw std::exception("Could not find MazeWidth!");
			}
			auto mazeWidthSpinBox = dynamic_cast<irr::gui::IGUISpinBox *>(mazeWidthGuiElement);
			if (!mazeWidthSpinBox)
			{
				LOG(ERROR) << "Could not up-cast IGUIElement to IGUISpinBox";
				throw std::exception("Could not up-cast IGUIElement to IGUISpinBox");
			}
			float mazeWidth = mazeWidthSpinBox->getValue();
			float mazeHeight = dynamic_cast<irr::gui::IGUISpinBox *>(
				_guiEnv->getRootGUIElement()->getElementFromId(MenuElement::MazeHeight))->getValue();
			LOG(DEBUG) << "Got maze dimensions: " << mazeWidth << "," << mazeHeight;
			return std::make_pair(static_cast<unsigned char>(round(mazeWidth)), static_cast<unsigned char>(round(mazeHeight)));
		}
		catch (std::exception & exp)
		{
			LOG(ERROR) << exp.what();
			throw exp;
		}
	}
	std::pair<int, int> CMazeGameMenu::getChosenResolution()
	{
		auto resList = dynamic_cast<irr::gui::IGUIListBox *>(_guiEnv->getRootGUIElement()->getElementFromId(MenuElement::MazeHeight));
		std::wstring resStr(resList->getListItem(resList->getSelected()));
		auto xPosition = resStr.find(L"x");
		auto widthStr = resStr.substr(0, xPosition);
		auto heightStr = resStr.substr(xPosition + 1);
		LOG(DEBUG) << "Got resolution of: " << widthStr << "," << heightStr;
		return std::make_pair<int, int>(std::stoi(widthStr), std::stoi(heightStr));
	}
	unsigned int CMazeGameMenu::getChosenNumOfAIPlayers()
	{
		float numOfAI = dynamic_cast<irr::gui::IGUISpinBox *>(
			_guiEnv->getRootGUIElement()->getElementFromId(MenuElement::MazeNumOfAI))->getValue();
		return static_cast<unsigned int>(round(numOfAI));
	}
	unsigned int CMazeGameMenu::getChosenAIDifficultyLevel()
	{
		float AIDifficultyLevel = dynamic_cast<irr::gui::IGUISpinBox *>(
			_guiEnv->getRootGUIElement()->getElementFromId(MenuElement::MazeAIDifficultyLevel))->getValue();
		return static_cast<unsigned int>(round(AIDifficultyLevel));
	}

	void CMazeGameMenu::setMenuEnabled(bool in_menuState, std::vector <irr::gui::IGUIElement *> & in_menuContainer)
	{
		for (auto element : in_menuContainer) {
			element->setVisible(in_menuState);
			element->setEnabled(in_menuState);
		}
	}
}