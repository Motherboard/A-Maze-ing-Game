#pragma once
#include "IrrlichtEngineInterface.h"
#include "MazePlayer.h"
#include "MazeGameEventReciever.h"
#include <memory>
#include "irr_ptr.h"


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


class CMazeGameEngine :
	public CIrrlichtEngineInterface
{
public:
	CMazeGameEngine();
	~CMazeGameEngine();
	void setMazeSize(int in_width, int in_length);
	void setupWorld() final;
	void setResulotion(int in_width, int in_height);
	void showMenu();
	void hideMenu();
	void cameraZoomOut();
	void cameraZoomIn();
	void cameraPan(float in_dx, float in_dy);
	void setCameraTargetCoordinates(float in_x, float in_y);
	void setCameraTargetSceneNode(irr::scene::ISceneNode &in_sceneNodeToFollow);
	//bool checkCollision(const irr::scene::ISceneNode &in_sceneNode);
private:
	void evolveWorld() final;
	void buildMaze();
	float _width, _length;
	CMazePlayer _player;
	irr::scene::ISceneNode * _mazeRootSceneNode = nullptr;
	irr::scene::ICameraSceneNode * _camera;
	const float _minTimeBetweenFrames = 0.016; //60 FPS
	CMazeGameEventReciever _gameEventReciever;
	//CMenuEventHandler _menuEventHandler;
	irr::extra::irr_ptr<irr::scene::IMetaTriangleSelector *> _mazeTriangleSelector;
};

