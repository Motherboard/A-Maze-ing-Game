#pragma once
#include <memory>

class CMazeGameEngine;

namespace irr
{
	namespace scene
	{
		class IAnimatedMesh;
		class IAnimatedMeshSceneNode;
		class ISceneNode;
		class ISceneManager;
		class IMetaTriangleSelector;
		class ICollisionCallback;
	}
}

struct CPlayerInnerData;

class CMazePlayer
{
public:
	CMazePlayer(CMazeGameEngine * const in_gameEngine);
	~CMazePlayer();
	enum class MovementDirection { North, South, East, West };
	void setDirection(MovementDirection in_newDirection);
	void stop();
	void evolve(float deltaT);
	void init(irr::scene::ISceneNode * const in_parentSceneNode, float in_relativeXPosition, float in_relativeYPositiony, 
		irr::scene::ISceneManager * const in_sceneManager, irr::scene::IMetaTriangleSelector * const in_triangleSeletor);
	void setPlayerMovementSpeed(float in_speed);
	void possesCamera(bool shouldPossesCamera = true);
private:
	bool isAboutToCollide();
	void startWalkAnimation();
	void startIdleAnimation();
	void startRotationAnimation();
	void moveStraight(float deltaT);
	void turn(float deltaT);
	bool checkAndUseMovementQueue();
	std::unique_ptr<CPlayerInnerData> _pInnerData; //this is the place to hide the direction of movement, so we don't have to include irrlicht from the header.
	static void setCurrentCameraPossesor(CMazePlayer * in_possesor);
};

