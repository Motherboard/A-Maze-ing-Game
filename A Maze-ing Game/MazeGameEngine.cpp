#include "MazeGameEngine.h"
#include "CMaze.h"
#include "irrlicht.h"
#include <iostream>
#include <algorithm>
#include "CBatchingMesh.h"
#include <random>
#include "easylogging++.h"

namespace amazeinggame
{


	CMazeGameEngine::CMazeGameEngine()
		: _gameEventReciever(this), _menuEventReciever(this)
	{
		setupMenuAndWinScreen();
	}

	void CMazeGameEngine::setupMenuAndWinScreen()
	{
		auto screenSize = _videoDriver->getScreenSize();
		_endGameImg = _guiEnvironment->addImage(irr::core::recti(irr::core::vector2di(screenSize.Width / 4, screenSize.Height / 4), screenSize / 4));
		_endGameImg->setVisible(false);
		_endGameImg->setScaleImage(true);
		_endGameImg->setColor(irr::video::SColor(200, 255, 255, 255));
		menu.init(_guiEnvironment, irr::core::recti(
			irr::core::vector2di(screenSize.Width / 4, screenSize.Height / 4), screenSize / 2));
		_introScreen = _guiEnvironment->addImage(irr::core::recti(irr::core::vector2di(0, 0), _videoDriver->getScreenSize()));
		_introScreen->setScaleImage(true);
		_introScreen->setImage(_videoDriver->getTexture("../media/game intro screen.jpg"));
		_device->setEventReceiver(&_gameEventReciever);
		//showMenu();
	}

	CMazeGameEngine::~CMazeGameEngine()
	{
	}


	void CMazeGameEngine::initWorld(unsigned char in_width, unsigned char in_length, unsigned char in_numOfAIPlayers, unsigned char in_AIDifficultyLevel)
	{
		//clear the player views list, since we're going to get rid of the player models - dont want to leave dangling pointers around!
		_playerViews.clear();
		_gameEventReciever.setPlayerController(nullptr); //make sure the controller does not point to a dangling controller
		_width = in_width;
		_length = in_length;
		LOG(INFO) << "making the maze";
		bool success = false;
		int numOfTries = 0;
		do
		{
			try
			{
				_worldModel.initGameWorld(in_width, in_length, 1, in_numOfAIPlayers, in_AIDifficultyLevel);
				success = true;
			}
			catch (std::exception exp)
			{
				++numOfTries;
				LOG(WARNING) << exp.what();
			}
		} while (!success && numOfTries < 10);
		if (!success)
		{
			throw std::exception("Failed creating game world with the current parameters");
		}
	}



	void CMazeGameEngine::buildMaze()
	{
		LOG(INFO) << "getting the walls" << std::endl;
		auto walls = _worldModel.getMaze().getMazeWalls();

		irr::extra::irr_ptr<irr::scene::CBatchingMesh *> batchingMesh(new irr::scene::CBatchingMesh());
		
		irr::scene::IGeometryCreator const * geometryCreator(_sceneManager->getGeometryCreator());
		irr::extra::irr_ptr<irr::scene::IMesh *> floorTile(geometryCreator->createPlaneMesh(irr::core::dimension2d<irr::f32>(1, 1)));
		floorTile->getMeshBuffer(0)->getMaterial().setTexture(0, _videoDriver->getTexture("..\\media\\floor.jpg"));
		for (unsigned int y = 0; y < _length; ++y)
		{
			for (unsigned int x = 0; x < _width; ++x)
			{
				batchingMesh->addMesh(floorTile.get(),
					irr::core::vector3df(x - 0.5, 0,
					y - 0.5));
			}
		}
		irr::extra::irr_ptr<irr::scene::IMesh *> horizontalBoundingWall(geometryCreator->createCubeMesh(irr::core::vector3df(0.9f, 1.5f, 0.1f)));
		irr::extra::irr_ptr<irr::scene::IMesh *> verticalBoundingWall(geometryCreator->createCubeMesh(irr::core::vector3df(0.1f, 1.5f, 0.9f)));
		auto wallTexture = _videoDriver->getTexture("../media/t351sml.jpg");
		horizontalBoundingWall->getMeshBuffer(0)->getMaterial().setTexture(0, wallTexture);
		verticalBoundingWall->getMeshBuffer(0)->getMaterial().setTexture(0, wallTexture);
		float horizontalWallOffsetX = -0.45f;
		float horizontalWallOffsetY = -0.95f;
		float verticalWallOffsetX = -0.95f;
		float verticalWallOffsetY = -0.45f;
		//add maze bounding walls
		for (unsigned int x = 0; x < _width; ++x)
		{
			batchingMesh->addMesh(horizontalBoundingWall.get(), irr::core::vector3df(x + horizontalWallOffsetX, 0, horizontalWallOffsetY));
			batchingMesh->addMesh(horizontalBoundingWall.get(), irr::core::vector3df(x + horizontalWallOffsetX, 0, horizontalWallOffsetY + _length));
		}
		for (unsigned int y = 0; y < _length; ++y)
		{
			batchingMesh->addMesh(verticalBoundingWall.get(), irr::core::vector3df(verticalWallOffsetX, 0, y + verticalWallOffsetY));
			batchingMesh->addMesh(verticalBoundingWall.get(), irr::core::vector3df(verticalWallOffsetX + _width, 0, y + verticalWallOffsetY));
		}

		//add all other maze walls
		for (const auto & wall : walls)
		{
			int dx = wall.x2 - wall.x1;
			int dy = wall.y2 - wall.y1;
			if (dx) //check if horizontal wall
			{
				for (auto x = 0; x < dx; ++x)
					batchingMesh->addMesh(horizontalBoundingWall.get(), irr::core::vector3df(x + wall.x1 + horizontalWallOffsetX, 0,
					wall.y1 + horizontalWallOffsetY));
			}
			else //or vertical wall
			{
				for (auto y = 0; y < dy; ++y)
					batchingMesh->addMesh(verticalBoundingWall.get(), irr::core::vector3df(wall.x1 + verticalWallOffsetX, 0,
					y + wall.y1 + verticalWallOffsetY));
			}
		}
		batchingMesh->update();
		auto floorNode = _sceneManager->addMeshSceneNode(batchingMesh.get(), _sceneManager->getRootSceneNode(), 0);
		//floorNode->setTriangleSelector(_sceneManager->createOctreeTriangleSelector(batchingMesh.get(),floorNode));
		floorNode->setMaterialFlag(irr::video::EMF_FOG_ENABLE, true);
		//floorNode->setMaterialType(irr::video::EMT_PARALLAX_MAP_SOLID);
		_mazeRootSceneNode = floorNode;
	}

	void CMazeGameEngine::addFinishPoint()
	{
		auto prizeLocation = _worldModel.getFinishPoint();
		auto prizeSceneNode = _sceneManager->addBillboardSceneNode(_mazeRootSceneNode, irr::core::dimension2df(0.5f, 0.5f),
			irr::core::vector3df(static_cast<float>(prizeLocation.first) - 0.5f, 0.75f, static_cast<float>(prizeLocation.second) - 0.5f));
		prizeSceneNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		prizeSceneNode->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
		prizeSceneNode->setMaterialTexture(0, _videoDriver->getTexture("../media/particle.bmp"));
		_sceneManager->addLightSceneNode(prizeSceneNode, irr::core::vector3df(), irr::video::SColorf(1.0f, 0.0f, 0.5f), 1.0f);
		auto animator = _sceneManager->createFlyCircleAnimator(
			irr::core::vector3df(static_cast<float>(prizeLocation.first) - 0.5f, 0.75f, static_cast<float>(prizeLocation.second) - 0.5f), 0.2f);
		prizeSceneNode->addAnimator(animator);
		animator->drop();
		auto miniPrizeSceneNode = _sceneManager->addBillboardSceneNode(prizeSceneNode, irr::core::dimension2df(0.3f, 0.3f));
		miniPrizeSceneNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		miniPrizeSceneNode->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
		miniPrizeSceneNode->setMaterialTexture(0, _videoDriver->getTexture("../media/portal2.bmp"));
		animator = _sceneManager->createFlyCircleAnimator(irr::core::vector3df(), 0.15f,0.015f);
		miniPrizeSceneNode->addAnimator(animator);
		animator->drop();

	}

	void CMazeGameEngine::loadAdditionalResources()
	{
		_winScreen = _videoDriver->getTexture("../media/winner.jpg");
		_videoDriver->makeColorKeyTexture(_winScreen, irr::core::position2d<irr::s32>(0, 0));
		_loseScreen = _videoDriver->getTexture("../media/failed.png");
		_videoDriver->makeColorKeyTexture(_loseScreen, irr::core::position2d<irr::s32>(0, 0));
	}

	void CMazeGameEngine::startOrientationScene()
	{
		_levelStartTime = _timer->getRealTime();
		unsigned int lastFrameTime = _timer->getRealTime();
		bool isPlayerPossingCamera = false;
		//first run while _finishPointWatchTime has not elapsed yet, than wait for player to
		//possess the camera, and finaly wait for the camera to finish it's movement.
		while (((_timer->getRealTime() - _finishPointWatchTime  < _levelStartTime || !isPlayerPossingCamera)
					&& _device->run()) || _camera.isMoving())
		{
			if ((_timer->getRealTime() - _finishPointWatchTime >= _levelStartTime)
				&& !isPlayerPossingCamera)
			{
				isPlayerPossingCamera = true;
				if (_playerViews.size())
				{
					_playerViews.front().possesCamera(&_camera);
				}
			}
			_videoDriver->beginScene(true, true, irr::video::SColor(255, 0, 0, 0));
			_sceneManager->drawAll();
			_guiEnvironment->drawAll();
			_videoDriver->endScene();
			unsigned int dt = _timer->getRealTime() - lastFrameTime;
			if (dt < _minTimeBetweenFrames*1000.0f)
			{
				_device->sleep((_minTimeBetweenFrames*1000.0f) - dt);
			}
			dt = _timer->getRealTime() - lastFrameTime;
			_camera.evolve(static_cast<float>(dt) / 1000.0f);
			lastFrameTime = _timer->getRealTime();
		}
	}

	void CMazeGameEngine::setupPlayerViews()
	{
		//make sure that we don't set more player views than playr models
		_playerViews.clear();
		const int numOfHumanPlayers = _worldModel.getNumOfHumanPlayers();
		//TODO: set player controls and name if there are more than one human player...
		//right now there is no split screen view, and no network controller - so this engine supports only one human player
		if (numOfHumanPlayers > 1)
		{
			throw std::exception("Too many human players in world - engine does not support more than one player right now");
		}
		if (numOfHumanPlayers) //if there is a human player
		{
			_playerViews.emplace_back();
			auto & humanPlayerView = _playerViews.back();
			humanPlayerView.addSceneNode(_worldModel.getHumanPlayer(0), _sceneManager, _mazeRootSceneNode);
			_gameEventReciever.setPlayerController(_worldModel.getHumanPlayer(0).getMazePlayerHumanController());
		}
		const auto numOfAIPlayers = _worldModel.getNumOfAIPlayers();
		auto textureForAIPlayer = _videoDriver->getTexture("../media/nskinrd.jpg");
		for (auto i = 0; i < numOfAIPlayers; ++i)
		{
			_playerViews.emplace_back();
			auto & AIPlayerView = _playerViews.back();
			AIPlayerView.addSceneNode(_worldModel.getAIPlayer(i), _sceneManager, _mazeRootSceneNode);
			AIPlayerView.setTexture(textureForAIPlayer);
		}
	}

	void CMazeGameEngine::setupCamera()
	{
		auto cameraNode = _sceneManager->addCameraSceneNode();
		_sceneManager->addLightSceneNode(cameraNode, irr::core::vector3df(0, 0.5, 0), irr::video::SColorf(1.0f, 1.0f, 1.0f), 5.0f);
		cameraNode->setPosition(irr::core::vector3df(static_cast<float>(_worldModel.getFinishPoint().first) - 0.5f
			, 4.0f, static_cast<float>(_worldModel.getFinishPoint().second) - 2.5f));
		cameraNode->setTarget(irr::core::vector3df(static_cast<float>(_worldModel.getFinishPoint().first) - 0.5f
			, 0.0f, static_cast<float>(_worldModel.getFinishPoint().second) - 0.5f));
		cameraNode->setUpVector(irr::core::vector3df(0, 0, 1));
		_camera.setCamera(cameraNode);
	}

	void CMazeGameEngine::setupWorld()
	{
		_sceneManager->clear();
		_videoDriver->setFog(irr::video::SColor(0, 30, 20, 10), irr::video::EFT_FOG_LINEAR, 3, 10, .03f, true, false);
		buildMaze();
		addFinishPoint();
		loadAdditionalResources();
		setupCamera();
		setupPlayerViews();
		hideMenu();
		startOrientationScene();
	}

	void CMazeGameEngine::evolveWorld()
	{
		if (menu.isMenuShowing() || !_worldModel.isWorldInitialized())
		{
			return;
		}
		float deltaT = getTimeFromPreviousFrame();
		if (deltaT > 0.3) //This probably happened because of a spike drop in framerate, ignore this long frame time.
		{ //if it happend due to the FPS droping to 3, the game is not playable anyway...
			LOG(DEBUG) << "Skipped frame, it was too late";
			return;
		}
		if (deltaT < _minTimeBetweenFrames) //I want max 60 FPS
		{
			_device->sleep((_minTimeBetweenFrames - deltaT) * 1000.0f);
			deltaT = getTimeFromPreviousFrame();
		}
		_worldModel.evolve(deltaT);
		for (auto & playerView : _playerViews)
			playerView.update(0);
		_camera.evolve(deltaT);
		if (_worldModel.isMazeGameWon())
		{
			showWinScreen();
		}
	}

	void CMazeGameEngine::showWinScreen()
	{
		if (!isWinScreenShowing)
		{
			if (_worldModel.getMazeWinnerIdx() == 0) //it's the player
			{
				if (!_winScreen)
				{
					_winScreen = _videoDriver->getTexture("../media/winner.jpg");
				}
				_endGameImg->setImage(_winScreen);
			}
			else //it's the AI
			{
				if (!_loseScreen)
				{
					_loseScreen = _videoDriver->getTexture("../media/failed.png");
				}
				_endGameImg->setImage(_loseScreen);
			}
			_endGameImg->setVisible(true);
			isWinScreenShowing = true;
		}
	}

	void CMazeGameEngine::setResulotion(unsigned int in_width, unsigned int in_height)
	{
		init(in_width, in_height);
		setupMenuAndWinScreen();
		if (isGameOngoing())
			setupWorld();
		startEventLoop();
	}
	void CMazeGameEngine::showMenu()
	{
		_introScreen->setVisible(false);
		isWinScreenShowing = false;
		_endGameImg->setVisible(false);
		menu.showMainMenu();
		_device->setEventReceiver(&_menuEventReciever);
	}
	void CMazeGameEngine::hideMenu()
	{
		menu.hideMenu();
		_device->setEventReceiver(&_gameEventReciever);
	}

	void CMazeGameEngine::cameraZoomOut()
	{
		_camera.zoomOut();
	}
	void CMazeGameEngine::cameraZoomIn()
	{
		_camera.zoomIn();
	}
	void CMazeGameEngine::cameraPan(float in_dx, float in_dy)
	{

		_camera.setTarget(_camera.getTarget() + irr::core::vector3df(in_dx, 0, in_dy));

	}

	void CMazeGameEngine::setCameraTargetCoordinates(float in_x, float in_y)
	{
		_camera.setTarget(irr::core::vector3df(in_x, 0, in_y));
	}

	void CMazeGameEngine::setCameraTargetPlayer(CMazePlayerView &in_playerToFollow)
	{
		in_playerToFollow.possesCamera(&_camera);
	}

	void CMazeGameEngine::quit()
	{
		_device->drop();
		_device = nullptr;
	}

	bool CMazeGameEngine::isGameOngoing()
	{
		return (!_worldModel.isMazeGameWon() && _worldModel.isWorldInitialized());
	}

}