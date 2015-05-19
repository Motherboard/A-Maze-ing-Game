#pragma once

#include "vector3d.h"
#include "ICameraSceneNode.h"

class CameraController
{
public:
	CameraController();
	void setCamera(irr::scene::ICameraSceneNode * const in_camera);
	void evolve(float deltaT);
	void setCameraZoom(float zoom);
	void zoomIn();
	void zoomOut();
	void setTarget(irr::core::vector3df &in_target);
	void setViewingAngle(irr::core::vector3df &in_angle);
	void setUpVector(irr::core::vector3df &up_vector);
	/*void setTarget(irr::core::vector3df &&in_target);
	void setViewingAngle(irr::core::vector3df &&in_angle);
	void setUpVector(irr::core::vector3df &&up_vector);*/
	~CameraController();
private:
	float _transitionSpeed = 0;
	const float Acceleratoin = 1;
	const float MaxSpeed = 10;
	bool _isPositionNeedsModifying = false, _isAngleNeedsModifying = false,
		_isTargetNeedsModifying = false, _isUpVectorNeedsModifying = false;
	irr::core::vector3df _desiredPos, _desiredTarget, _desiredAngle, _desiredUpVector;
	irr::core::vector3df _posChangeDir, _targetChangeDir, _angleChange, _upVectorChange;
	irr::scene::ICameraSceneNode * _camera;
};

