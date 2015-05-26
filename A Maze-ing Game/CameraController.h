#pragma once

#include "vector3d.h"
#include "ICameraSceneNode.h"


namespace amazeinggame
{
	//A class for controlling the camera movement in a smooth way, it should be evolved along with the rest of the world.
	class CCameraController
	{
	public:
		CCameraController();
		//initialize the camera to be controlled by this controller - should point to a camera created by the scene graph
		//by the addCamera function.
		void setCamera(irr::scene::ICameraSceneNode * const in_camera);
		//modify camera position and target to desired positions, deltaT is in seconds
		void evolve(float deltaT);
		//set the zoom factor, 1 is the curent zoom, 2 means zoom out by a factor of 2, 0.5 means zoom in by factor of 2
		void setCameraZoom(float zoom);
		//zoom in by a fraction of the currnet zoom
		void zoomIn();
		//zoom out by a fraction of the current zoom
		void zoomOut();
		//set desired target position for the camera to move to, second parameter determines whether the angle of 
		//isometric view should be maintained - i.e. move the camera position along with the target position.
		void setTarget(const irr::core::vector3df &in_target, bool in_keepPositionToTargetVector = true);
		//set desired camera position for the camera to move to, second parameter determines whether the angle of 
		//isometric view should be maintained - i.e. move the target position along with the camera position.
		void setPosition(const irr::core::vector3df &in_position, bool in_keepPositionToTargetVector = true);
		//set the up vector of the camera.
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