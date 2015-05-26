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
		//TODO: Add cool images as buttons.
		LOG(DEBUG) << "Initializing menu";
		_mainMenuElements.clear();
		_gameSettingsElements.clear();
		_videoSettingsElements.clear();
		_guiEnv = in_guiEnv;
		auto backgroundImg = in_guiEnv->addImage(irr::core::recti(irr::core::vector2di(0, 0), in_guiEnv->getVideoDriver()->getScreenSize()));
		backgroundImg->setScaleImage(true);
		backgroundImg->setColor(irr::video::SColor(150, 255, 255, 255));
		backgroundImg->setImage(in_guiEnv->getVideoDriver()->getTexture("../media/p-of-eternity-maze-by-malcolm.jpg"));
		auto backImg = in_guiEnv->addImage(irr::core::recti(
			irr::core::vector2di(in_menuRect.UpperLeftCorner.X * (1.0f - ElementSeparationRatio), in_menuRect.UpperLeftCorner.Y * (1.0f - ElementSeparationRatio)), 
			irr::core::dimension2di(in_menuRect.getSize().Width *(1.0f + ElementSeparationRatio), in_menuRect.getSize().Height *(1.0f + ElementSeparationRatio + ElementHeightRatio))), backgroundImg);
		backImg->setScaleImage(true);
		backImg->setColor(irr::video::SColor(200, 255, 255, 255));
		backImg->setImage(in_guiEnv->getVideoDriver()->getTexture("../media/irrlicht2_up.jpg"));
		_menuBackground = backgroundImg;
		in_guiEnv->getSkin()->setFont(in_guiEnv->getFont("../media/fonthaettenschweiler.bmp"));
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
		resImage->setText(L"Select the desired resolution");
		//resImage->setImage(in_videoDriver->getTexture("../media/Resolution.jpg"));
		_videoSettingsElements.push_back(resImage);
		elementPosition.Y += elementSeperation + elementSize.Height;
		auto listBox = in_guiEnv->addListBox(irr::core::recti(elementPosition, irr::core::dimension2di(elementSize.Width,in_menuRect.getHeight()- (elementSize.Height + 2.0f * elementSeperation))), nullptr, MenuElement::ResolutionDropDown, true);
		listBox->addItem(L"800x600");
		listBox->addItem(L"1024x768");
		listBox->addItem(L"1200x1600");
		listBox->setSelected(0);
		_videoSettingsElements.push_back(listBox);
		//add buttons at the bottom to both game and setting menu
		elementPosition.Y = in_menuRect.UpperLeftCorner.Y + in_menuRect.getHeight();
		elementSize.Width = (elementSize.Width - elementSeperation) / 2.0f;
		button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::BackToMainMenuBtn, L"Back to Main Menu");
		//button->setImage(in_videoDriver->getTexture("../media/backToMainMenu.jpg"));
		_videoSettingsElements.push_back(button);
		_gameSettingsElements.push_back(button);
		elementPosition.X += elementSize.Width + elementSeperation;
		button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::SetVideoSettingsBtn, L"Apply (restart game)");
		//button->setImage(in_videoDriver->getTexture("../media/setVideoSettings.jpg"));
		_videoSettingsElements.push_back(button);
		button = in_guiEnv->addButton(irr::core::recti(elementPosition, elementSize), nullptr, MenuElement::SetGameSettingsBtn, L"Apply (restart game)");
		//button->setImage(in_videoDriver->getTexture("../media/setGameSettings.jpg"));
		_gameSettingsElements.push_back(button);
		//set the game settings menu
		elementPosition.Y = in_menuRect.UpperLeftCorner.Y;
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
		spinBox->setRange(20.0f, 60.0f);
		spinBox->setStepSize(2.0f);
		spinBox->setValue(30.0f);
		_gameSettingsElements.push_back(spinBox);
		elementPosition.Y += elementSeperation + elementSize.Height;
		spinBox = in_guiEnv->addSpinBox(L"Maze Height", irr::core::recti(elementPosition, elementSize), true, nullptr, MenuElement::MazeHeight);
		spinBox->setDecimalPlaces(0);
		spinBox->setRange(20.0f, 60.0f);
		spinBox->setStepSize(2.0f);
		spinBox->setValue(30.0f);
		_gameSettingsElements.push_back(spinBox);
		elementPosition = in_menuRect.UpperLeftCorner;
		_gameSettingsElements.push_back( in_guiEnv->addStaticText(L"Number of AI players:", irr::core::recti(elementPosition, elementSize)));
		elementPosition.Y += elementSeperation + elementSize.Height;
		_gameSettingsElements.push_back(in_guiEnv->addStaticText(L"AI Level:", irr::core::recti(elementPosition, elementSize)));
		elementPosition.Y += elementSeperation + elementSize.Height;
		_gameSettingsElements.push_back(in_guiEnv->addStaticText(L"Maze Width:", irr::core::recti(elementPosition, elementSize)));
		elementPosition.Y += elementSeperation + elementSize.Height;
		_gameSettingsElements.push_back(in_guiEnv->addStaticText(L"Maze Length:", irr::core::recti(elementPosition, elementSize)));
		elementPosition.Y += elementSeperation + elementSize.Height;

		//hide all menu elements
		setMenuEnabled(false, _mainMenuElements);
		setMenuEnabled(false, _gameSettingsElements);
		setMenuEnabled(false, _videoSettingsElements);
		_menuBackground->setVisible(false);
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
		_menuBackground->setVisible(true);
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
		_menuBackground->setVisible(true);
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
		_menuBackground->setVisible(true);
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
		_menuBackground->setVisible(false);
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
			float mazeWidth = getGuiElement<irr::gui::IGUISpinBox>(MenuElement::MazeWidth)->getValue();
			float mazeHeight = getGuiElement<irr::gui::IGUISpinBox>(MenuElement::MazeHeight)->getValue();
			LOG(DEBUG) << "Got maze dimensions: " << mazeWidth << "," << mazeHeight;
			return std::make_pair(static_cast<unsigned char>(round(mazeWidth)), static_cast<unsigned char>(round(mazeHeight)));
		}
		catch (std::exception & exp)
		{
			LOG(ERROR) << "Failed reading width or length for maze dimensions: " << exp.what();
			throw exp;
		}
	}
	std::pair<int, int> CMazeGameMenu::getChosenResolution()
	{
		
		auto resList = getGuiElement<irr::gui::IGUIListBox>(MenuElement::ResolutionDropDown);
		std::wstring resStr(resList->getListItem(resList->getSelected()));
		auto xPosition = resStr.find(L"x");
		auto widthStr = resStr.substr(0, xPosition);
		auto heightStr = resStr.substr(xPosition + 1);
		LOG(DEBUG) << "Got resolution of: " << widthStr << "," << heightStr;
		return std::make_pair<int, int>(std::stoi(widthStr), std::stoi(heightStr));
	}
	unsigned int CMazeGameMenu::getChosenNumOfAIPlayers()
	{
		float numOfAI = getGuiElement<irr::gui::IGUISpinBox>(MenuElement::MazeNumOfAI)->getValue();
		return static_cast<unsigned int>(round(numOfAI));
	}
	unsigned int CMazeGameMenu::getChosenAIDifficultyLevel()
	{
		float AIDifficultyLevel = getGuiElement<irr::gui::IGUISpinBox>(MenuElement::MazeAIDifficultyLevel)->getValue();
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