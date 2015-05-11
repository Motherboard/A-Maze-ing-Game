#include "IrrlichtEngineInterface.h"
#include "irrlicht.h"
#include <iostream>

#pragma comment(lib, "Irrlicht.lib")

CIrrlichtEngineInterface::CIrrlichtEngineInterface()
{
	init();
}


CIrrlichtEngineInterface::~CIrrlichtEngineInterface()
{
	if (_device)
		_device->drop();
}

void CIrrlichtEngineInterface::init()
{
	_device =
		irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2d<irr::u32>(800, 600), 16,
		false, false, false, 0);
	if (!_device)
	{
		std::clog << "failed creating openGL window... reverting to software rendering" << std::endl;
		_device =
			irr::createDevice(irr::video::EDT_SOFTWARE, irr::core::dimension2d<irr::u32>(800, 600), 16,
			false, false, false, 0);
		if (!_device)
		{
			std::exception("failed initializg software rendering, there is no other choice but to give up...");
		}
	}
	_videoDriver = _device->getVideoDriver();
	_sceneManager = _device->getSceneManager();
	_guiEnvironment = _device->getGUIEnvironment();
	_timer = _device->getTimer();
}

void CIrrlichtEngineInterface::startEventLoop()
{
	bool wasPaused = false;
	while (_device->run()) //call _device->close() to exit event loop
	{
		if (_device->isWindowActive())
		{
			if (!wasPaused)
				evolveWorld();
			else
				wasPaused = false;
			_videoDriver->beginScene(true, true, irr::video::SColor(255, 100, 101, 140));
			_sceneManager->drawAll();
			_guiEnvironment->drawAll();
			_videoDriver->endScene();
			_frameTime = _timer->getRealTime();
		}
		else
		{
			wasPaused = true;
			_device->yield();
		}
	}
}

float CIrrlichtEngineInterface::getTimeFromPreviousFrame()
{
	return static_cast<float>(_timer->getRealTime() - _frameTime) / 1000.0f;
}