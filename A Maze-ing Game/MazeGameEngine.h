#pragma once
#include "IrrlichtEngineInterface.h"
#include "MazePlayerModel.h"
#include "MazePlayerView.h"
#include "MazeGameEventReciever.h"
#include <memory>
#include "irr_ptr.h"
#include "MazeGameWorldModel.h"
#include "CameraController.h"
#include "menuEventReciever.h"
#include "MazeGameMenu.h"

//CMazeGameEngine is the main class that of the game
//It holds the world model, initialized it, connects it to the world view classes, renders the views, and evolves the 
//world and the views.

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
	namespace video
	{
		class ITexture;
	}
	namespace gui
	{
		class IGUIImage;
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
		//Initialize the world (maze and players) Although the world can support more than one human player, the engine currently
		//limits it to just one, no split screen interface or networking support yet.
		void initWorld(unsigned char in_width, unsigned char in_length, unsigned char numOfAIPlayers, unsigned char in_AIDifficultyLevel);
		//Build the scenegraph according to the world, assign controllers to the players models of the world, assign views to the player models.
		//and start the orientation scene.
		void setupWorld() final;
		//reset the resolution of the game
		void setResulotion(unsigned int in_width, unsigned int in_height);
		//show the main menu
		void showMenu();
		//hide the main menu
		void hideMenu();
		//zoom camera out
		void cameraZoomOut();
		//zoom camera in
		void cameraZoomIn();
		//move camera, the input is the ammount to move in each direction
		void cameraPan(float in_dx, float in_dy);
		//set camera target position
		void setCameraTargetCoordinates(float in_x, float in_y);
		//set camera target player view
		void setCameraTargetPlayer(CMazePlayerView &in_sceneNodeToFollow);
		//quit the game
		void quit();
		//returns true if the world is initialized and the game was not won (by human or AI)
		bool isGameOngoing();
		//holds the menu item (for easy access to the selected preferences)
		CMazeGameMenu menu;
	private:
		void evolveWorld() final;
		void setupMenuAndWinScreen();
		void buildMaze();
		void addFinishPoint();
		void showWinScreen();
		void loadAdditionalResources();
		void setupCamera();
		void setupPlayerViews();
		void startOrientationScene();
		bool isWinScreenShowing = false;
		float _width, _length;
		std::vector<CMazePlayerView> _playerViews;
		irr::scene::ISceneNode * _mazeRootSceneNode = nullptr;
		CCameraController _camera;
		const float _minTimeBetweenFrames = 0.016; //60 FPS
		CMazeGameEventReciever _gameEventReciever;
		CMazeGameWorldModel _worldModel;
		unsigned int _levelStartTime;
		bool _startPlaying = false;
		const unsigned int _finishPointWatchTime = 2000;
		irr::video::ITexture * _winScreen = nullptr, * _loseScreen = nullptr;
		CMenuEventReciever _menuEventReciever;
		irr::gui::IGUIImage *_endGameImg = nullptr;
		irr::gui::IGUIImage * _introScreen = nullptr;
		//irr::extra::irr_ptr<irr::scene::IMetaTriangleSelector *> _mazeTriangleSelector;
	};

}