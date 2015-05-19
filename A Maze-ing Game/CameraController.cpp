#include "CameraController.h"


CameraController::CameraController()
{
}


CameraController::~CameraController()
{
}

void CameraController::setCamera(irr::scene::ICameraSceneNode * const in_camera)
{
	_camera = in_camera;
}

void CameraController::evolve(float deltaT)
{
	if (_isPositionNeedsModifying)
	{
		irr::core::vector3df & currentPosition = _camera->getPosition();
		float reminingDistance = currentPosition.getDistanceFrom(_desiredPos);
		if (reminingDistance < _positionTransitionSpeed * deltaT)
		{
			if (_keepPositionToTargetVector)
			{
				_camera->setTarget(_camera->getTarget() + _desiredPos - _camera->getPosition());
			}
			_camera->setPosition(_desiredPos);
			_isPositionNeedsModifying = false;
		}
		else
		{
			//accelerate until x = 0.5v^2/a
			if (_positionTransitionSpeed < MaxSpeed && reminingDistance > 0.5 * _positionTransitionSpeed*_positionTransitionSpeed / Acceleratoin)
				_positionTransitionSpeed += Acceleratoin*deltaT;
			else
				_positionTransitionSpeed -= Acceleratoin*deltaT;
			_camera->setPosition(_camera->getPosition() + _posChangeDir * _positionTransitionSpeed * deltaT);
			if (_keepPositionToTargetVector)
			{
				_camera->setTarget(_camera->getTarget() + _posChangeDir * _positionTransitionSpeed * deltaT);
			}
		}
	}
	if (_isTargetNeedsModifying)
	{
		irr::core::vector3df & currentTarget = _camera->getTarget();
		float reminingDistance = currentTarget.getDistanceFrom(_desiredTarget);
		if (reminingDistance < _targetTransitionSpeed * deltaT)
		{
			_camera->setTarget(_desiredTarget);
			_isTargetNeedsModifying = false;
			if (_keepPositionToTargetVector)
			{
				_camera->setPosition(_camera->getPosition() + _desiredTarget - _camera->getTarget());
			}
		}
		else
		{
			//accelerate until x = 0.5v^2/a
			if (_targetTransitionSpeed < MaxSpeed && reminingDistance > 0.5 * _targetTransitionSpeed*_targetTransitionSpeed / Acceleratoin)
				_targetTransitionSpeed += Acceleratoin*deltaT;
			else
				_targetTransitionSpeed -= Acceleratoin*deltaT;
			_camera->setTarget(_camera->getPosition() + _targetChangeDir * _targetTransitionSpeed * deltaT);
			if (_keepPositionToTargetVector)
			{
				_camera->setPosition(_camera->getPosition() + _targetChangeDir * _positionTransitionSpeed * deltaT);
			}
		}
	}
	if (_isAngleNeedsModifying)
	{
		//irr::core::quaternion::rotationFromTo()
	}
}
void CameraController::setCameraZoom(float zoom)
{

}
void CameraController::zoomIn()
{

}
void CameraController::zoomOut()
{

}
void CameraController::setTarget(irr::core::vector3df &in_target, bool in_keepPositionToTargetVector)
{
	
}


void CameraController::setViewingAngle(irr::core::vector3df &in_angle)
{

}
void CameraController::setUpVector(irr::core::vector3df &up_vector)
{

}

//
//void CameraController::setTarget(irr::core::vector3df &&in_target)
//{
//
//}
//void CameraController::setViewingAngle(irr::core::vector3df &&in_angle)
//{
//
//}
//void CameraController::setUpVector(irr::core::vector3df &&up_vector)
//{
//
//}