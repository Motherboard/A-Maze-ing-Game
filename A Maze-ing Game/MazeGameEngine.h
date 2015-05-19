#pragma once
#include "IrrlichtEngineInterface.h"
#include "MazePlayerModel.h"
#include "MazePlayerView.h"
#include "MazeGameEventReciever.h"
#include <memory>
#include "irr_ptr.h"
#include "MazeGameWorldModel.h"


namespace irr
{
	namespace scene
	{
		//class IAnimatedMesh;
		class IAnimatedMeshSceneNode;
		class IMeshSceneNode;
		class ICameraSceneNode;
		class ISceneNode;
		class IMesh;
	}
	class IReciever;
}

namespace amazeinggame
{

	class CMazeGameEngine :
		public irr::extra::CIrrlichtEngineInterface
	{
	public:
		CMazeGameEngine();
		~CMazeGameEngine();
		void initWorld(unsigned char in_width, unsigned char in_length, unsigned char numOfHumanPlayers, unsigned char numOfAIPlayers);
		void setupWorld() final;
		void setResulotion(int in_width, int in_height);
		void showMenu();
		void hideMenu();
		void cameraZoomOut();
		void cameraZoomIn();
		void cameraPan(float in_dx, float in_dy);
		void setCameraTargetCoordinates(float in_x, float in_y);
		void setCameraTargetSceneNode(irr::scene::ISceneNode &in_sceneNodeToFollow);
	private:
		void evolveWorld() final;
		void buildMaze();
		void addFinishPoint();
		void showWinScreen();
		bool isMenuOpen = false;
		bool isWinScreenShowing = false;
		float _width, _length;
		std::vector<CMazePlayerView> _playerViews;
		irr::scene::ISceneNode * _mazeRootSceneNode = nullptr;
		irr::scene::ICameraSceneNode * _camera;
		const float _minTimeBetweenFrames = 0.016; //60 FPS
		CMazeGameEventReciever _gameEventReciever;
		CMazeGameWorldModel _worldModel;
		
		//CMenuEventHandler _menuEventHandler;
		//irr::extra::irr_ptr<irr::scene::IMetaTriangleSelector *> _mazeTriangleSelector;
	};

}