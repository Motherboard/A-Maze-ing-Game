#include "MazePlayerView.h"
#include "MazePlayerModel.h"
#include <irrlicht.h>
#include <algorithm>
#include <iostream>
#include "CameraController.h"

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
	update(0);
}

void CMazePlayerView::setCurrentCameraPossesor(CMazePlayerView * in_possesor, CCameraController * const in_camera)
{
	static CMazePlayerView * currentCameraPossesor(nullptr);
	if (currentCameraPossesor != nullptr && currentCameraPossesor != in_possesor)
	{
		currentCameraPossesor->_camera = nullptr;
	}
	currentCameraPossesor = in_possesor;
	if (currentCameraPossesor)
	{
		currentCameraPossesor->_camera = in_camera;
	}
}


void CMazePlayerView::possesCamera(CCameraController * const in_camera)
{
	if (in_camera)
	{
		CMazePlayerView::setCurrentCameraPossesor(this, in_camera);
		_camera->setTarget(_playerSceneNode->getAbsolutePosition());
	}
	else
	{
		CMazePlayerView::setCurrentCameraPossesor(nullptr, nullptr);
	}
}

void CMazePlayerView::adjustCamera()
{
	if (!_camera)
	{
		return;
	}
	_camera->setTarget(_playerSceneNode->getAbsolutePosition());
}


void CMazePlayerView::startWalkAnimation()
{
	if (CurrentAnimationState::Walking == _currentAnimation)
	{
		return;
	}
	_currentAnimation = CurrentAnimationState::Walking;
	_playerSceneNode->setFrameLoop(0, 13);
}
void CMazePlayerView::startIdleAnimation()
{
	if (CurrentAnimationState::Idle == _currentAnimation)
	{
		return;
	}
	_currentAnimation = CurrentAnimationState::Idle;
	_playerSceneNode->setFrameLoop(205, 249);
}
void CMazePlayerView::startRotationAnimation()
{
	if (CurrentAnimationState::Rotating == _currentAnimation)
	{
		return;
	}
	_currentAnimation = CurrentAnimationState::Rotating;
	_playerSceneNode->setFrameLoop(133, 144);
}

void CMazePlayerView::startDropDeadAnimation()
{
	if (CurrentAnimationState::Dead == _currentAnimation)
	{
		return;
	}
	_currentAnimation = CurrentAnimationState::Dead;
	_playerSceneNode->setLoopMode(false);
	_playerSceneNode->setAnimationSpeed(10);
	_playerSceneNode->setFrameLoop(165,172);
}

irr::scene::IAnimatedMeshSceneNode * CMazePlayerView::addSceneNode(const CMazePlayerModel & in_mazePlayer, irr::scene::ISceneManager * const in_sceneManager, irr::scene::ISceneNode * const in_parent)
{
	_playerModel = &in_mazePlayer;
	_sceneManager = in_sceneManager;
	irr::scene::IAnimatedMesh * ninjaMesh = in_sceneManager->getMesh("../media/ninja.b3d");
	auto playerState = _playerModel->getPlayerState();
	_playerSceneNode = in_sceneManager->addAnimatedMeshSceneNode(ninjaMesh, in_parent, -1, 
		irr::core::vector3df(playerState.x - 0.5, 0, playerState.y - 0.5),
		irr::core::vector3df(), irr::core::vector3df(0.1, 0.1, 0.1));
	update(0);
	return _playerSceneNode;
}


void CMazePlayerView::update(float in_deltaT)
{
	auto playerState = _playerModel->getPlayerState();
	if (playerState.isPlayerDead)
	{
		startDropDeadAnimation();
		return;
	}
	_playerSceneNode->setPosition(irr::core::vector3df(playerState.x - 0.5, 0, playerState.y - 0.5)
		+ irr::core::vector3df(playerState.speedX, 0, playerState.speedY) * in_deltaT);
	_playerSceneNode->setRotation(irr::core::vector3df(0,
		playerState.currentAngle + std::min(playerState.angularSpeed * in_deltaT, playerState.remainingAngle), 0));
	if (playerState.speedX != 0 || playerState.speedY != 0)
	{
		startWalkAnimation();
		adjustCamera();
		if (_camera)
		{
			_camera->evolve(in_deltaT);
		}
	}
	else
	{
		startIdleAnimation();
	}
}

void CMazePlayerView::setColor(PlayerViewColor in_color)
{
	irr::video::ITexture *texture = nullptr;
	switch (in_color)
	{
	case PlayerViewColor::Blue:
		texture = _sceneManager->getVideoDriver()->getTexture("../media/nskinbl.jpg");
		break;
	case PlayerViewColor::Red:
		texture = _sceneManager->getVideoDriver()->getTexture("../media/nskinrd.jpg");
		break;
	default:
		break;
	}
	_playerSceneNode->getMaterial(0).setTexture(0, texture);
}

const irr::scene::IAnimatedMeshSceneNode * CMazePlayerView::getSceneNode() const
{
	return _playerSceneNode;
}

}