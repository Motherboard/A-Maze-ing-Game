#include "MazePlayerView.h"
#include "MazePlayerModel.h"
#include <irrlicht.h>
#include <algorithm>
#include <iostream>

namespace amazeinggame
{


CMazePlayerView::CMazePlayerView()
{
}


CMazePlayerView::~CMazePlayerView()
{
}



CMazePlayerView::CMazePlayerView(CMazePlayerModel * const in_mazePlayer)
	: _playerModel(in_mazePlayer)
{
}

void CMazePlayerView::setCurrentCameraPossesor(CMazePlayerView * in_possesor, irr::scene::ICameraSceneNode * const in_camera)
{
	static CMazePlayerView * currentCameraPossesor(nullptr);
	if (currentCameraPossesor != nullptr && currentCameraPossesor != in_possesor)
		currentCameraPossesor->_camera = nullptr;
	currentCameraPossesor = in_possesor;
	if (currentCameraPossesor)
		currentCameraPossesor->_camera = in_camera;
}


void CMazePlayerView::possesCamera(irr::scene::ICameraSceneNode * const in_camera)
{
	if (in_camera)
	{
		CMazePlayerView::setCurrentCameraPossesor(this, in_camera);
		adjustCamera();
	}
	else
		CMazePlayerView::setCurrentCameraPossesor(nullptr,nullptr);
}

void CMazePlayerView::adjustCamera()
{
	if (!_camera)
		return;
	_camera->setPosition(_playerSceneNode->getAbsolutePosition() + irr::core::vector3df(0, 5, -2));
	_camera->setUpVector(irr::core::vector3df(0, 0, 1));
	_camera->setTarget(_playerSceneNode->getAbsolutePosition());
	
}


void CMazePlayerView::startWalkAnimation()
{
	if (CurrentAnimationState::Walking == _currentAnimation)
		return;
	_currentAnimation = CurrentAnimationState::Walking;
	_playerSceneNode->setFrameLoop(0, 13);
}
void CMazePlayerView::startIdleAnimation()
{
	if (CurrentAnimationState::Idle == _currentAnimation)
		return;
	_currentAnimation = CurrentAnimationState::Idle;
	_playerSceneNode->setFrameLoop(205, 249);
}
void CMazePlayerView::startRotationAnimation()
{
	if (CurrentAnimationState::Rotating == _currentAnimation)
		return;
	_currentAnimation = CurrentAnimationState::Rotating;
	_playerSceneNode->setFrameLoop(133, 144);
}



void CMazePlayerView::addSceneNode(const CMazePlayerModel & in_mazePlayer, irr::scene::ISceneManager * const in_sceneManager, irr::scene::ISceneNode * const in_parent)
{
	_playerModel = &in_mazePlayer;
	irr::scene::IAnimatedMesh * ninjaMesh = in_sceneManager->getMesh("../media/ninja.b3d");
	auto playerState = _playerModel->getPlayerState();
	_playerSceneNode = in_sceneManager->addAnimatedMeshSceneNode(ninjaMesh, in_parent, -1, 
		irr::core::vector3df(playerState.x - 0.5, 0, playerState.y - 0.5),
		irr::core::vector3df(), irr::core::vector3df(0.1, 0.1, 0.1));
}


void CMazePlayerView::update(float in_deltaT)
{
	auto playerState = _playerModel->getPlayerState();
	_playerSceneNode->setPosition(irr::core::vector3df(playerState.x - 0.5, 0, playerState.y - 0.5)
		+ irr::core::vector3df(playerState.speedX, 0, playerState.speedY) * in_deltaT);
	_playerSceneNode->setRotation(irr::core::vector3df(0,
		playerState.currentAngle + std::min(playerState.angularSpeed * in_deltaT, playerState.remainingAngle), 0));
	if (playerState.speedX != 0 || playerState.speedY != 0)
	{
		startWalkAnimation();
		adjustCamera();
	}
	else
	{
		startIdleAnimation();
	}
}

void CMazePlayerView::setTexture(irr::video::ITexture * in_texture)
{
	_playerSceneNode->getMaterial(0).setTexture(0, in_texture);
}

}