#include "MazePlayer.h"
#include "irrlicht.h"
#include "MazeGameEngine.h"
#include <math.h>
#include <iostream>
#include <deque>

enum class MovementState { MoveStraight, Rotate };
enum class CurrentAnimationState { Walking, Rotating, Idle, NotSet };
struct CPlayerInnerData
{
	CPlayerInnerData(CMazeGameEngine * const in_gameEngine) : gameEngine(in_gameEngine) { }
	irr::core::vector3df currentDirection = irr::core::vector3df(0, 0, 1);
	irr::core::vector3df previousDirection = irr::core::vector3df(0, 0, 1);
	MovementState movementState = MovementState::MoveStraight;
	bool isMoving = false;
	CMazeGameEngine * const gameEngine; //this is always set in the construction...
	irr::scene::IAnimatedMesh * playerMesh = nullptr;
	irr::extra::irr_ptr<irr::scene::IAnimatedMeshSceneNode *> playerSceneNode;
	irr::scene::ISceneManager * sceneManager = nullptr;
	float speed = 5;
	float angularSpeed = 720;
	CMazePlayer::MovementDirection movementDirection = CMazePlayer::MovementDirection::North;
	float remainingAngle = 0;
	bool isStopRequested;
	float distanceWalkedFromLastTurn = 0;
	irr::scene::ITriangleSelector * levelTriangleSelector = nullptr;
	std::deque<CMazePlayer::MovementDirection> movementQueue;
	CurrentAnimationState currentAnimation = CurrentAnimationState::NotSet;
	bool isPossessingCamera = false;
};



void CMazePlayer::setCurrentCameraPossesor(CMazePlayer * in_possesor)
{
	static CMazePlayer * currentCameraPossesor(nullptr);
	if (currentCameraPossesor != nullptr && currentCameraPossesor != in_possesor)
		currentCameraPossesor->_pInnerData->isPossessingCamera = false;
	currentCameraPossesor = in_possesor;
	if (currentCameraPossesor)
		currentCameraPossesor->_pInnerData->isPossessingCamera = true;
}


void CMazePlayer::possesCamera(bool shouldPossesCamera)
{
	if (shouldPossesCamera)
	{
		CMazePlayer::setCurrentCameraPossesor(this);
		_pInnerData->gameEngine->setCameraTargetCoordinates(_pInnerData->playerSceneNode->getAbsolutePosition().X, _pInnerData->playerSceneNode->getAbsolutePosition().Z);
	}
	else
		CMazePlayer::setCurrentCameraPossesor(nullptr);
}

CMazePlayer::CMazePlayer(CMazeGameEngine * const in_gameEngine)
{
	_pInnerData.reset(new CPlayerInnerData(in_gameEngine));
	
}

void CMazePlayer::init(irr::scene::ISceneNode * const parentSceneNode, float in_x, float in_y,
	irr::scene::ISceneManager * const sceneManager,
	irr::scene::IMetaTriangleSelector * const triangleSeletor)
{
	if (!_pInnerData->playerMesh)
		_pInnerData->playerMesh = sceneManager->getMesh("C:\\c++\\irrlicht-1.8.1\\media\\ninja.b3d");
	_pInnerData->playerSceneNode.reset(sceneManager->addAnimatedMeshSceneNode(_pInnerData->playerMesh, parentSceneNode, -1,
		irr::core::vector3df(in_x, 0, in_y), irr::core::vector3df(), irr::core::vector3df(0.1, 0.1, 0.1)));
	_pInnerData->sceneManager = sceneManager;
	_pInnerData->levelTriangleSelector = triangleSeletor;
	startIdleAnimation();
}

void CMazePlayer::setPlayerMovementSpeed(float in_speed)
{
	_pInnerData->speed = in_speed;
}

CMazePlayer::~CMazePlayer()
{
}


void CMazePlayer::setDirection(MovementDirection in_newDirection)
{
	if (_pInnerData->movementDirection != in_newDirection)
	{
		if (_pInnerData->isMoving)
		{ //if we're already moving, complete the move - and only than change direction.
			if (_pInnerData->movementQueue.size() > 2) //forget about earlier movement requests - the player has changed his mind.
				_pInnerData->movementQueue.pop_front();
			_pInnerData->movementQueue.push_back(in_newDirection);
			return;
		}
		_pInnerData->previousDirection = _pInnerData->currentDirection;
		_pInnerData->movementDirection = in_newDirection;
		switch (_pInnerData->movementDirection)
		{
		case MovementDirection::East: _pInnerData->currentDirection = irr::core::vector3df(1, 0, 0); break;
		case MovementDirection::West: _pInnerData->currentDirection = irr::core::vector3df(-1, 0, 0); break;
		case MovementDirection::North: _pInnerData->currentDirection = irr::core::vector3df(0, 0, 1); break;
		case MovementDirection::South: _pInnerData->currentDirection = irr::core::vector3df(0, 0, -1); break;
		default:
			break;
		}
		irr::core::vector3df directionChange = _pInnerData->currentDirection - _pInnerData->previousDirection;
		if (1 < abs(directionChange.X) || 1 < abs(directionChange.Z))
			_pInnerData->remainingAngle = static_cast<int>(round(180 + _pInnerData->remainingAngle)) % 360;
		else if (irr::core::vector3df(_pInnerData->previousDirection.Z, 0, -_pInnerData->previousDirection.X)
			.dotProduct(_pInnerData->currentDirection) < 0)
			_pInnerData->remainingAngle = static_cast<int>(round(_pInnerData->remainingAngle - 90)) % 360;
		else
			_pInnerData->remainingAngle = static_cast<int>(round(_pInnerData->remainingAngle + 90)) % 360;
		_pInnerData->movementState = MovementState::Rotate;
		//startRotationAnimation();
		startIdleAnimation();
	}
	else
	{
		startWalkAnimation();
	}
	_pInnerData->isMoving = true;
	_pInnerData->isStopRequested = false;
}
void CMazePlayer::evolve(float deltaT)
{
	if (!_pInnerData->isMoving)
	{
		if (!checkAndUseMovementQueue())
			return;
	}
	if (_pInnerData->movementState == MovementState::Rotate)
		turn(deltaT);
	else if (!isAboutToCollide())
		moveStraight(deltaT);
	else //collided
	{
		_pInnerData->isMoving = false;
		startIdleAnimation();
		checkAndUseMovementQueue();
	}

}

void CMazePlayer::turn(float deltaT)
{
	float rotationAngle;
	if (abs(_pInnerData->remainingAngle) < _pInnerData->angularSpeed * deltaT)
	{
		rotationAngle = _pInnerData->remainingAngle;
		_pInnerData->movementState = MovementState::MoveStraight;
		_pInnerData->distanceWalkedFromLastTurn = 0;
		startWalkAnimation();
	}
	else
		rotationAngle = _pInnerData->remainingAngle > 0 ? _pInnerData->angularSpeed * deltaT : -_pInnerData->angularSpeed * deltaT;
	_pInnerData->remainingAngle -= rotationAngle;
	_pInnerData->playerSceneNode->setRotation(irr::core::vector3df(0, _pInnerData->playerSceneNode->getRotation().Y + rotationAngle, 0));
}

void CMazePlayer::moveStraight(float deltaT)
{
	irr::core::vector3df movementVector = _pInnerData->currentDirection * deltaT * _pInnerData->speed;
	bool shouldStop = false;
	float newDistanceFromLastTurn = _pInnerData->distanceWalkedFromLastTurn + deltaT * _pInnerData->speed;
	if ((newDistanceFromLastTurn > 1) && _pInnerData->isStopRequested &&
		(newDistanceFromLastTurn >= std::ceil(_pInnerData->distanceWalkedFromLastTurn)))
	{ //if there are turns waiting to be taken, we can take them at this point
		movementVector.setLength(newDistanceFromLastTurn - std::ceil(_pInnerData->distanceWalkedFromLastTurn));
		_pInnerData->isMoving = false;
		shouldStop = true;
		_pInnerData->distanceWalkedFromLastTurn = 0;
	}
	else
		_pInnerData->distanceWalkedFromLastTurn = newDistanceFromLastTurn;
	_pInnerData->playerSceneNode->setPosition(_pInnerData->playerSceneNode->getPosition() + movementVector);
	if (shouldStop) //this is here to make sure the walking animation continues up untill the last frame.
	{
		startIdleAnimation();
		checkAndUseMovementQueue();
	}
	if (_pInnerData->isPossessingCamera)
	{
		_pInnerData->gameEngine->cameraPan(movementVector.X / 2.0f, movementVector.Z / 2.0f);
		_pInnerData->gameEngine->setCameraTargetCoordinates(_pInnerData->playerSceneNode->getAbsolutePosition().X, _pInnerData->playerSceneNode->getAbsolutePosition().Z);
	}
}

void CMazePlayer::stop()
{
	_pInnerData->isStopRequested = true;
}

bool CMazePlayer::isAboutToCollide()
{
	irr::core::vector3df collisionPoint;
	irr::core::triangle3df collisionTriangle;
	irr::scene::ISceneNode * collisionNode;
	bool collisionResult = _pInnerData->sceneManager->getSceneCollisionManager()->getCollisionPoint(
		irr::core::line3df(_pInnerData->playerSceneNode->getAbsolutePosition(), 
		_pInnerData->playerSceneNode->getAbsolutePosition() + 
		0.2 * _pInnerData->playerSceneNode->getBoundingBox().MinEdge.getLength() * _pInnerData->currentDirection), 
		_pInnerData->levelTriangleSelector, collisionPoint, collisionTriangle, collisionNode);
	return collisionResult; 
}

void CMazePlayer::startWalkAnimation()
{
	if (CurrentAnimationState::Walking == _pInnerData->currentAnimation)
		return;
	_pInnerData->currentAnimation = CurrentAnimationState::Walking;
	_pInnerData->playerSceneNode->setFrameLoop(0, 14);
}
void CMazePlayer::startIdleAnimation()
{
	if (CurrentAnimationState::Idle == _pInnerData->currentAnimation)
		return;
	_pInnerData->currentAnimation = CurrentAnimationState::Idle;
	_pInnerData->playerSceneNode->setFrameLoop(205, 249);
}
void CMazePlayer::startRotationAnimation()
{
	if (CurrentAnimationState::Rotating == _pInnerData->currentAnimation)
		return;
	_pInnerData->currentAnimation = CurrentAnimationState::Rotating;
	_pInnerData->playerSceneNode->setFrameLoop(133 , 144);
}

bool CMazePlayer::checkAndUseMovementQueue()
{
	if (_pInnerData->movementQueue.size())
	{
		setDirection(_pInnerData->movementQueue.front());
		_pInnerData->movementQueue.pop_front();
		return true;
	}
	return false;
}
