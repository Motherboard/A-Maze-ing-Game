#include "MazeGameEngine.h"
#include "CMaze.h"
#include "irrlicht.h"
#include <iostream>
#include <algorithm>
#include "CBatchingMesh.h"
#include <random>

namespace amazeinggame
{


	CMazeGameEngine::CMazeGameEngine()
		: _gameEventReciever(this)
	{

	}


	CMazeGameEngine::~CMazeGameEngine()
	{
	}


	void CMazeGameEngine::initWorld(unsigned char in_width, unsigned char in_length, unsigned char in_numOfHumanPlayers, unsigned char in_numOfAIPlayers)
	{
		_width = in_width;
		_length = in_length;
		std::clog << "making the maze" << std::endl;
		bool success = false;
		int numOfTries = 0;
		do
		{
			try
			{
				_worldModel.initGameWorld(_width, _length, in_numOfHumanPlayers, in_numOfAIPlayers);
				success = true;
			}
			catch (std::exception exp)
			{
				++numOfTries;
				std::cerr << exp.what() << std::endl;
			}
		} while (!success && numOfTries < 10);
		if (!success)
			throw std::exception("Failed creating game world with the current parameters");
	}



	void CMazeGameEngine::buildMaze()
	{
		std::clog << "getting the walls" << std::endl;
		auto walls = _worldModel.getMaze().getMazeWalls();

		irr::extra::irr_ptr<irr::scene::CBatchingMesh *> batchingMesh(new irr::scene::CBatchingMesh());

		irr::scene::IGeometryCreator const * geometryCreator(_sceneManager->getGeometryCreator());
		irr::extra::irr_ptr<irr::scene::IMesh *> floorTile(geometryCreator->createPlaneMesh(irr::core::dimension2d<irr::f32>(1, 1)));
		floorTile->getMeshBuffer(0)->getMaterial().setTexture(0, _videoDriver->getTexture("..\\media\\floor.jpg"));
		for (int y = 0; y < _length; ++y)
		{
			for (int x = 0; x < _width; ++x)
			{
				batchingMesh->addMesh(floorTile.get(),
					irr::core::vector3df(x - 0.5, 0,
					y - 0.5));
			}
		}
		irr::extra::irr_ptr<irr::scene::IMesh *> horizontalBoundingWall(geometryCreator->createCubeMesh(irr::core::vector3df(0.9, 1, 0.1)));
		irr::extra::irr_ptr<irr::scene::IMesh *> verticalBoundingWall(geometryCreator->createCubeMesh(irr::core::vector3df(0.1, 1, 0.9)));
		auto wallTexture = _videoDriver->getTexture("../media/t351sml.jpg");
		horizontalBoundingWall->getMeshBuffer(0)->getMaterial().setTexture(0, wallTexture);
		verticalBoundingWall->getMeshBuffer(0)->getMaterial().setTexture(0, wallTexture);
		float horizontalWallOffsetX = -0.45;
		float horizontalWallOffsetY = -0.95;
		float verticalWallOffsetX = -0.95;
		float verticalWallOffsetY = -0.45;
		//add maze bounding walls
		for (int x = 0; x < _width; ++x)
		{
			batchingMesh->addMesh(horizontalBoundingWall.get(), irr::core::vector3df(x + horizontalWallOffsetX, 0, horizontalWallOffsetY));
			batchingMesh->addMesh(horizontalBoundingWall.get(), irr::core::vector3df(x + horizontalWallOffsetX, 0, horizontalWallOffsetY + _length));
		}
		for (int y = 0; y < _length; ++y)
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
				for (int x = 0; x < dx; ++x)
					batchingMesh->addMesh(horizontalBoundingWall.get(), irr::core::vector3df(x + wall.x1 + horizontalWallOffsetX, 0,
					wall.y1 + horizontalWallOffsetY));
			}
			else //or vertical wall
			{
				for (int y = 0; y < dy; ++y)
					batchingMesh->addMesh(verticalBoundingWall.get(), irr::core::vector3df(wall.x1 + verticalWallOffsetX, 0,
					y + wall.y1 + verticalWallOffsetY));
			}
		}
		batchingMesh->update();
		auto floorNode = _sceneManager->addMeshSceneNode(batchingMesh.get(), _sceneManager->getRootSceneNode(), 0);
		//floorNode->setTriangleSelector(_sceneManager->createOctreeTriangleSelector(batchingMesh.get(),floorNode));

		_mazeRootSceneNode = floorNode;
	}

	void CMazeGameEngine::setupWorld()
	{
		_sceneManager->clear();
		_sceneManager->addLightSceneNode(_sceneManager->getRootSceneNode(), irr::core::vector3df(5.5, 2, 5.5));
		buildMaze();
		/*_mazeTriangleSelector.reset(_sceneManager->createMetaTriangleSelector());
		_mazeTriangleSelector->addTriangleSelector(_mazeRootSceneNode->getTriangleSelector());
		for (auto & child : _mazeRootSceneNode->getChildren())
		{
		_mazeTriangleSelector->addTriangleSelector(_sceneManager->createTriangleSelectorFromBoundingBox(child));
		}*/
		_camera = _sceneManager->addCameraSceneNode();
		_camera->setUpVector(irr::core::vector3df(0, 0, 1));
		const int numOfHumanPlayers = _worldModel.getNumOfHumanPlayers(); 
		//TODO: set player controls and name if there are more than one human player...
		//right now there is no split screen view, and no network controller - so this engine supports only one human player
		if (numOfHumanPlayers > 1)
			throw std::exception("Too many human players in world - engine does not support more than one player right now");
		if (numOfHumanPlayers) //if there is a human player
		{
			_playerViews.emplace_back();
			auto & humanPlayerView = _playerViews.back();
			humanPlayerView.addSceneNode(_worldModel.getHumanPlayer(0), _sceneManager, _mazeRootSceneNode);
			humanPlayerView.possesCamera(_camera);
			_gameEventReciever.setPlayerController(_worldModel.getHumanPlayer(0).getMazePlayerHumanController());
		}
		const int numOfAIPlayers = _worldModel.getNumOfAIPlayers();
		auto textureForAIPlayer = _videoDriver->getTexture("../media/nskinrd.jpg");
		for (int i = 0; i < numOfAIPlayers; ++i)
		{
			_playerViews.emplace_back();
			auto & AIPlayerView = _playerViews.back();
			AIPlayerView.addSceneNode(_worldModel.getAIPlayer(i), _sceneManager, _mazeRootSceneNode);
			AIPlayerView.setTexture(textureForAIPlayer);
			AIPlayerView.possesCamera(_camera);
		}
		_device->setEventReceiver(&_gameEventReciever);
	}

	void CMazeGameEngine::evolveWorld()
	{
		float deltaT = getTimeFromPreviousFrame();
		if (deltaT < _minTimeBetweenFrames) //I want max 60 FPS
		{
			_device->sleep((_minTimeBetweenFrames - deltaT) * 1000);
			deltaT = getTimeFromPreviousFrame();
		}
		_worldModel.evolve(deltaT);
		for (auto & playerView : _playerViews)
			playerView.update(0);
		//_guiEnvironment->addStaticText()

	}

	void CMazeGameEngine::setResulotion(int in_width, int in_height)
	{

	}
	void CMazeGameEngine::showMenu()
	{

	}
	void CMazeGameEngine::hideMenu()
	{

	}

	void CMazeGameEngine::cameraZoomOut()
	{
		//_camera-
	}
	void CMazeGameEngine::cameraZoomIn()
	{

	}
	void CMazeGameEngine::cameraPan(float in_dx, float in_dy)
	{

		_camera->setPosition(_camera->getAbsolutePosition() + irr::core::vector3df(in_dx, 0, in_dy));

	}

	void CMazeGameEngine::setCameraTargetCoordinates(float in_x, float in_y)
	{
		_camera->setTarget(irr::core::vector3df(in_x, 0, in_y));
	}

	void CMazeGameEngine::setCameraTargetSceneNode(irr::scene::ISceneNode &in_sceneNodeToFollow)
	{
		_camera->setParent(&in_sceneNodeToFollow);
	}
}