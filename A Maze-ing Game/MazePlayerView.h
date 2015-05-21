#pragma once

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
		void update(float in_RatioOfNextToPreviousTick);
		irr::scene::IAnimatedMeshSceneNode * addSceneNode(const CMazePlayerModel & in_mazePlayer, irr::scene::ISceneManager * const in_sceneManager, irr::scene::ISceneNode * const parentSceneNode);
		const irr::scene::IAnimatedMeshSceneNode * getSceneNode() const;
		void possesCamera(CCameraController * const in_camera);
		void setTexture(irr::video::ITexture * in_texture);
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
		enum class CurrentAnimationState { Walking, Rotating, Idle, Dead, NotSet } _currentAnimation = CurrentAnimationState::NotSet;
	};

}