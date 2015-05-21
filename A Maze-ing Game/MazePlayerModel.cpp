#include "MazePlayerModel.h"
#include "vector2d.h"
#include <math.h>
#include <iostream>
#include <deque>
#include "MazeGameWorldModel.h"
#include "MazePlayerControllerInterface.h"
#include "MazePlayerHumanController.h"

namespace amazeinggame
{

	enum class MovementState { MoveStraight, Rotate };

	struct CPlayerInnerData
	{
		CPlayerInnerData()  { }
		irr::core::vector2df currentPosition;
		irr::core::vector2df currentDirection = irr::core::vector2df(0, 1);
		irr::core::vector2df previousDirection = irr::core::vector2df(0, 1);
		float distanceToStopBeforeWall = 0.1;
		MovementState movementState = MovementState::MoveStraight;
		float currentSpeed = 0;
		float typicalSpeed = 5;
		float angularSpeed = 720;
		maze::Direction movementDirection = maze::Direction::North;
		float remainingAngle = 0;
		float currentAngle = 0;
		bool isStopRequested = false;
		float distanceWalkedFromLastTurn = 0;
		std::deque<maze::Direction> movementQueue;
	};




	CMazePlayerModel::CMazePlayerModel()
	{
		_pInnerData.reset(new CPlayerInnerData());

	}

	void CMazePlayerModel::init(int in_x, int in_y, const CMazeGameWorldModel * const in_worldModel,
		std::unique_ptr<CMazePlayerControllerInterface> && in_controller)
	{
		_pInnerData->currentPosition = irr::core::vector2df(in_x, in_y);
		_worldModel = in_worldModel;
		_controller = std::move(in_controller);
	}

	void CMazePlayerModel::setPlayerMovementSpeed(float in_speed)
	{
		_pInnerData->typicalSpeed = in_speed;
	}

	CMazePlayerModel::CMazePlayerModel(CMazePlayerModel &&other)
	{
		_pInnerData = std::move(other._pInnerData);
		_name = std::move(other._name);
		_worldModel = std::move(other._worldModel);
	}

	CMazePlayerModel::~CMazePlayerModel()
	{
	}


	void CMazePlayerModel::setDirection(maze::Direction in_newDirection)
	{
		if (_pInnerData->movementDirection != in_newDirection)
		{
			if (_pInnerData->currentSpeed > 0)
			{ //if we're already moving, complete the move - and only than change direction.
				if (_pInnerData->movementQueue.size() > 2) //forget about earlier movement requests - the player has changed his mind.
					_pInnerData->movementQueue.pop_front();
				_pInnerData->movementQueue.push_back(in_newDirection);
				return;
			}
			_pInnerData->previousDirection = _pInnerData->currentDirection;
			_pInnerData->movementDirection = in_newDirection;
			auto tempVectorDir = maze::getVectorFromDirection(_pInnerData->movementDirection);
			_pInnerData->currentDirection = irr::core::vector2df(tempVectorDir.first, tempVectorDir.second);

			irr::core::vector2df directionChange = _pInnerData->currentDirection - _pInnerData->previousDirection;
			if (1 < abs(directionChange.X) || 1 < abs(directionChange.Y))
				_pInnerData->remainingAngle = static_cast<int>(round(180 + _pInnerData->remainingAngle)) % 360;
			else if (irr::core::vector2df(_pInnerData->previousDirection.Y, -_pInnerData->previousDirection.X)
				.dotProduct(_pInnerData->currentDirection) < 0)
				_pInnerData->remainingAngle = static_cast<int>(round(_pInnerData->remainingAngle - 90)) % 360;
			else
				_pInnerData->remainingAngle = static_cast<int>(round(_pInnerData->remainingAngle + 90)) % 360;
			_pInnerData->movementState = MovementState::Rotate;
		}
		else
		{
			_pInnerData->movementState = MovementState::MoveStraight;
		}
		_pInnerData->currentSpeed = _pInnerData->typicalSpeed;
		_pInnerData->isStopRequested = false;
	}

	void CMazePlayerModel::setPlayerDead()
	{
		_isPlayerDead = true;
	}

	void CMazePlayerModel::evolve(float deltaT)
	{
		if (_isPlayerDead)
			return;
		_controller->update(this);
		if (0 == _pInnerData->currentSpeed)
		{
			if (!checkAndUseMovementQueue())
				return;
		}
		if (_pInnerData->movementState == MovementState::Rotate)
			turn(deltaT);
		else
			moveStraight(deltaT);


	}

	void CMazePlayerModel::turn(float deltaT)
	{
		float rotationAngle;
		if (abs(_pInnerData->remainingAngle) < _pInnerData->angularSpeed * deltaT)
		{
			rotationAngle = _pInnerData->remainingAngle;
			_pInnerData->movementState = MovementState::MoveStraight;
			_pInnerData->distanceWalkedFromLastTurn = 0;
		}
		else
			rotationAngle = _pInnerData->remainingAngle > 0 ? _pInnerData->angularSpeed * deltaT : -_pInnerData->angularSpeed * deltaT;
		_pInnerData->remainingAngle -= rotationAngle;
		_pInnerData->currentAngle += rotationAngle;
		//_pInnerData->playerSceneNode->setRotation(irr::core::vector3df(0, _pInnerData->playerSceneNode->getRotation().Y + rotationAngle, 0));
	}

	void CMazePlayerModel::moveStraight(float deltaT)
	{
		float stepSize = deltaT * _pInnerData->currentSpeed;
		if (stepSize > 1)
			stepSize = 1;
		irr::core::vector2df movementVector = _pInnerData->currentDirection * stepSize;
		bool shouldStop = false;
		float newDistanceFromLastTurn = _pInnerData->distanceWalkedFromLastTurn + stepSize;
		float newDistanceFromCellCenterEdge = newDistanceFromLastTurn - std::ceil(_pInnerData->distanceWalkedFromLastTurn);
		float newDistanceFromCellWall = newDistanceFromLastTurn - std::ceil(_pInnerData->distanceWalkedFromLastTurn - _pInnerData->distanceToStopBeforeWall);
		if ((newDistanceFromCellWall >= 0) && isAboutToCollide())
		{ //going to collide with wall
			movementVector = movementVector.normalize() * (std::ceil(_pInnerData->distanceWalkedFromLastTurn - _pInnerData->distanceToStopBeforeWall) - _pInnerData->distanceWalkedFromLastTurn);
			shouldStop = true;
			_pInnerData->distanceWalkedFromLastTurn = 0;
		}
		else if ((newDistanceFromLastTurn > 1) && _pInnerData->isStopRequested && (newDistanceFromCellCenterEdge >= 0))
		{ //if there are turns waiting to be taken, we can take them at this point
			movementVector = movementVector.normalize() * (std::ceil(_pInnerData->distanceWalkedFromLastTurn) - _pInnerData->distanceWalkedFromLastTurn);
			shouldStop = true;
			_pInnerData->distanceWalkedFromLastTurn = 0;
		}
		else
			_pInnerData->distanceWalkedFromLastTurn = newDistanceFromLastTurn;
		//_pInnerData->playerSceneNode->setPosition(_pInnerData->playerSceneNode->getPosition() + movementVector);
		_pInnerData->currentPosition += movementVector;
		if (shouldStop) //this is here to make sure the walking animation continues up untill the last frame.
		{
			_pInnerData->currentSpeed = 0;
			checkAndUseMovementQueue();
		}
	}

	void CMazePlayerModel::stop()
	{
		_pInnerData->isStopRequested = true;
	}

	bool CMazePlayerModel::isAboutToCollide()
	{
		//This was the "physical" way to check for collisions - it's way slower than the new way...
		/*irr::core::vector3df collisionPoint;
		irr::core::triangle3df collisionTriangle;
		irr::scene::ISceneNode * collisionNode;
		bool collisionResult = _pInnerData->sceneManager->getSceneCollisionManager()->getCollisionPoint(
		irr::core::line3df(_pInnerData->playerSceneNode->getAbsolutePosition(),
		_pInnerData->playerSceneNode->getAbsolutePosition() +
		0.2 * _pInnerData->playerSceneNode->getBoundingBox().MinEdge.getLength() * _pInnerData->currentDirection),
		_pInnerData->levelTriangleSelector, collisionPoint, collisionTriangle, collisionNode);*/
		if (_worldModel)
			return !(_worldModel->getMaze().isDirectionAllowedFromPosition(std::round(_pInnerData->currentPosition.X),
			std::round(_pInnerData->currentPosition.Y), _pInnerData->movementDirection));
		else
			return false; //if there is no maze... we can go everywhere!!!
	}

	bool CMazePlayerModel::checkAndUseMovementQueue()
	{
		if (_pInnerData->movementQueue.size())
		{
			setDirection(_pInnerData->movementQueue.front());
			_pInnerData->movementQueue.pop_front();
			return true;
		}
		return false;
	}

	CMazePlayerState CMazePlayerModel::getPlayerState() const
	{
		//x, y, speedX, speedY, currentAngle, angularSpeed, remainingAngle, isPlayerDead;
		return CMazePlayerState{ _pInnerData->currentPosition.X, _pInnerData->currentPosition.Y
			, _pInnerData->currentSpeed * _pInnerData->currentDirection.X
			, _pInnerData->currentSpeed * _pInnerData->currentDirection.Y,
			_pInnerData->currentAngle, _pInnerData->angularSpeed, _pInnerData->remainingAngle, 
			_isPlayerDead };
	}

	CMazePlayerHumanController * const CMazePlayerModel::getMazePlayerHumanController() const
	{
		return dynamic_cast<CMazePlayerHumanController *>(_controller.get());
	}

	void CMazePlayerModel::setPlayerName(std::string in_playerName)
	{
		_name = in_playerName;
	}
	const std::string & CMazePlayerModel::getPlayerName() const
	{
		return _name;
	}
}