#pragma once

#include "vector3d.h"
#include "ICameraSceneNode.h"
namespace amazeinggame
{
	class CCameraController
	{
	public:
		CCameraController();
		void setCamera(irr::scene::ICameraSceneNode * const in_camera);
		void evolve(float deltaT);
		void setCameraZoom(float zoom);
		void zoomIn();
		void zoomOut();
		void setTarget(const irr::core::vector3df &in_target, bool in_keepPositionToTargetVector = true);
		void setPosition(const irr::core::vector3df &in_position, bool in_keepPositionToTargetVector = true);
		void setUpVector(const irr::core::vector3df &up_vector);
		const irr::core::vector3df & getTarget() const;
		const irr::core::vector3df & getPosition() const;
		bool isMoving() const;
		~CCameraController();
	private:
		float _positionTransitionSpeed = 0;
		float _targetTransitionSpeed = 0;
		float _angularTransitionSpeed = 0;
		float upVectorTransitionSpeed = 0;
		const float Acceleratoin = 5;
		const float MaxSpeed = 10;
		bool _isPositionNeedsModifying = false,_isTargetNeedsModifying = false, 
			_keepPositionToTargetVector = false;
		irr::core::vector3df _desiredPos, _desiredTarget, _desiredUpVector;
		irr::core::vector3df _posChangeDir, _targetChangeDir;
		irr::core::vector3df _leftVector; //this is the vector multiplication of the up vector and the position to target vector - it should be kept constant.
		irr::core::vector3df _posToTarget;
		irr::scene::ICameraSceneNode * _camera;
	};

}