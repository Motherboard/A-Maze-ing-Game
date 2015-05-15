#pragma once

namespace irr {
	namespace scene {
		class ISceneManager;
		class ICameraSceneNode;
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

	class CMazePlayerView
	{
	public:
		CMazePlayerView(CMazePlayerModel * const in_mazePlayer);
		CMazePlayerView();
		~CMazePlayerView();
		void update(float in_RatioOfNextToPreviousTick);
		void addSceneNode(const CMazePlayerModel & in_mazePlayer, irr::scene::ISceneManager * const in_sceneManager, irr::scene::ISceneNode * const parentSceneNode);
		void possesCamera(irr::scene::ICameraSceneNode * const in_camera);
		void setTexture(irr::video::ITexture * in_texture);
	private:
		void startWalkAnimation();
		void startIdleAnimation();
		void startRotationAnimation();
		void adjustCamera();
		static void setCurrentCameraPossesor(CMazePlayerView * in_possesor, irr::scene::ICameraSceneNode * const in_camera);
		const CMazePlayerModel * _playerModel = nullptr;
		irr::scene::IAnimatedMeshSceneNode * _playerSceneNode = nullptr;
		irr::scene::ICameraSceneNode * _camera = nullptr;
		enum class CurrentAnimationState { Walking, Rotating, Idle, NotSet } _currentAnimation = CurrentAnimationState::NotSet;
	};

}