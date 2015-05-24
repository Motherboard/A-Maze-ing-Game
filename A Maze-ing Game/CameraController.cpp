#include "CameraController.h"
#include <iostream>
#include "easylogging++.h"

namespace amazeinggame
{
	CCameraController::CCameraController()
	{
	}


	CCameraController::~CCameraController()
	{
	}

	void CCameraController::setCamera(irr::scene::ICameraSceneNode * const in_camera)
	{
		_camera = in_camera;
		_desiredPos = _camera->getPosition();
		_desiredTarget = _camera->getTarget();
		_posToTarget = _desiredTarget - _desiredPos;
		setUpVector(_camera->getUpVector());
		
	}

	void CCameraController::evolve(float deltaT)
	{
		if (_isPositionNeedsModifying)
		{	
			//accelerate until x = 0.5v^2/a
			irr::core::vector3df currentPosition = _camera->getPosition();
			float reminingDistance = currentPosition.getDistanceFrom(_desiredPos);
			if (reminingDistance > 0.5 * _positionTransitionSpeed*_positionTransitionSpeed / Acceleratoin)
			{
				if (_positionTransitionSpeed + Acceleratoin*deltaT < MaxSpeed)
				{
					_positionTransitionSpeed += Acceleratoin*deltaT;
				}
				else
				{
					_positionTransitionSpeed = MaxSpeed;
				}
			}
			else if (_positionTransitionSpeed - Acceleratoin*deltaT > 0)
			{
				_positionTransitionSpeed -= Acceleratoin*deltaT;
			}
			else
			{
				_positionTransitionSpeed = 0;
			}
			if (reminingDistance <= _positionTransitionSpeed * deltaT)
			{
				_camera->setPosition(_desiredPos);
				_isPositionNeedsModifying = false;
			}
			else
			{
				_camera->setPosition(_camera->getPosition() + _posChangeDir * _positionTransitionSpeed * deltaT);
			}
			currentPosition = _camera->getPosition();
		}
		if (_isTargetNeedsModifying)
		{
			irr::core::vector3df currentTarget = _camera->getTarget();
			float reminingDistance = currentTarget.getDistanceFrom(_desiredTarget);
			//accelerate until x = 0.5v^2/a
			if (reminingDistance > 0.5 * _targetTransitionSpeed*_targetTransitionSpeed / Acceleratoin)
			{
				if (_targetTransitionSpeed + Acceleratoin*deltaT < MaxSpeed)
				{
					_targetTransitionSpeed += Acceleratoin*deltaT;
				}
				else
				{
					_targetTransitionSpeed = MaxSpeed;
				}
			}
			else if (_targetTransitionSpeed - Acceleratoin*deltaT > 0)
			{
				_targetTransitionSpeed -= Acceleratoin*deltaT;
			}
			else
			{
				_targetTransitionSpeed = 0;
			}
			if (reminingDistance <= _targetTransitionSpeed * deltaT)
			{
				_camera->setTarget(_desiredTarget);
				_isTargetNeedsModifying = false;
			}
			else
			{
				_camera->setTarget(_camera->getTarget() + _targetChangeDir * _targetTransitionSpeed * deltaT);
			}
		}
		if (_isPositionNeedsModifying || _isTargetNeedsModifying)
		{
			
			if (_posToTarget.crossProduct(_desiredUpVector) != _leftVector)
			{
				_camera->setUpVector(_leftVector.crossProduct(_posToTarget));
			}
		}
		
	}
	void CCameraController::setCameraZoom(float zoom)
	{
		_posToTarget = _posToTarget.normalize() * zoom;
		setPosition(_desiredTarget - _posToTarget);
	}
	void CCameraController::zoomIn()
	{
		_posToTarget *= 0.9;
		setPosition(_desiredTarget - _posToTarget);
	}
	void CCameraController::zoomOut()
	{
		_posToTarget *= 1.1;
		setPosition(_desiredTarget - _posToTarget);
	}
	void CCameraController::setTarget(const irr::core::vector3df &in_target, bool in_keepPositionToTargetVector)
	{
		_desiredTarget = in_target;
		_targetChangeDir = in_target - _camera->getTarget();
		_targetChangeDir.normalize();
		_isTargetNeedsModifying = true;
		_keepPositionToTargetVector = in_keepPositionToTargetVector;
		if (_keepPositionToTargetVector)
		{
			setPosition(_desiredTarget - _posToTarget);
		}
		else
		{
			_posToTarget = _desiredTarget - _desiredPos;
		}
	}

	void CCameraController::setPosition(const irr::core::vector3df &in_position, bool in_keepPositionToTargetVector)
	{
		_desiredPos = in_position;
		_posChangeDir = in_position - _camera->getPosition();
		_posChangeDir.normalize();
		_isPositionNeedsModifying = true;
		_keepPositionToTargetVector = in_keepPositionToTargetVector;
		if (_keepPositionToTargetVector)
		{
			setTarget(_desiredPos + _posToTarget,false);
		}
		else
		{
			_posToTarget = _desiredTarget - _desiredPos;
		}
	}


	const irr::core::vector3df & CCameraController::getTarget() const
	{
		return _desiredTarget;
	}
	
	const irr::core::vector3df & CCameraController::getPosition() const
	{
		return _desiredPos;
	}

	void CCameraController::setUpVector(const irr::core::vector3df &up_vector)
	{
		_leftVector = _posToTarget.crossProduct(up_vector).normalize();
		_desiredUpVector = up_vector;
	}

	bool CCameraController::isMoving() const
	{
		return (_isPositionNeedsModifying || _isTargetNeedsModifying);
	}
}
