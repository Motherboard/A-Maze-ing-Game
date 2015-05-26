#pragma once

//The CMazePlayerView is responsible for representing the player model graphically.
//It is assigned a player model, it polls it for it's state and determines the animation and position accordingly.
//It can also interpolate a new position if it's update function is called more often than the player model's evolve function
//If it possesses the camera it would also update it's target position to follow the player view.

namespace irr {
	namespace scene {
		class ISceneManager;
		class IAnimatedMeshSceneNode;
		class ISceneNode;
	}
	namespace video
	{
		class ITexture;
	}
}

namespace amazeinggame
{

	class CMazePlayerModel;
	class CCameraController;

	class CMazePlayerView
	{
	public:
		CMazePlayerView(CMazePlayerModel * const in_mazePlayer);
		CMazePlayerView();
		~CMazePlayerView();
		//update the position and animation, the in_RatioOfNextToPreviousTick determines the interpolation of the position
		//according to the player's velocity.
		void update(float in_RatioOfNextToPreviousTick);
		//This is the setup function of the player view - it loads the appropriate model, sets it's player model and 
		//according to the model's state determines it's initial position.
		irr::scene::IAnimatedMeshSceneNode * addSceneNode(const CMazePlayerModel & in_mazePlayer, irr::scene::ISceneManager * const in_sceneManager, irr::scene::ISceneNode * const parentSceneNode);
		const irr::scene::IAnimatedMeshSceneNode * getSceneNode() const;
		//only one player view may posses a given camera, the player
		void possesCamera(CCameraController * const in_camera);
		enum class PlayerViewColor {Blue, Red};
		//set the color of the player view (used to destinguish between the AI and the player)
		void setColor(PlayerViewColor in_color);
	private:
		void startWalkAnimation();
		void startIdleAnimation();
		void startRotationAnimation();
		void startDropDeadAnimation();
		void adjustCamera();
		static void setCurrentCameraPossesor(CMazePlayerView * in_possesor, CCameraController * const in_camera);
		const CMazePlayerModel * _playerModel = nullptr;
		irr::scene::IAnimatedMeshSceneNode * _playerSceneNode = nullptr;
		CCameraController * _camera = nullptr;
		irr::scene::ISceneManager * _sceneManager;
		enum class CurrentAnimationState { Walking, Rotating, Idle, Dead, NotSet } _currentAnimation = CurrentAnimationState::NotSet;
	};

}