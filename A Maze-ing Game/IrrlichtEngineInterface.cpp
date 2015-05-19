#include "IrrlichtEngineInterface.h"
#include "irrlicht.h"
#include <iostream>

#pragma comment(lib, "Irrlicht.lib")

namespace irr
{
	namespace extra
	{


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
			double loop = 0;
			double avgEvolve = 0;
			double avgDraw = 0;
			while (_device->run()) //call _device->close() to exit event loop
			{	
				irr::u32 dtEvolve, dtDraw; 
				if (_device->isWindowActive())
				{
					if (!wasPaused)
					{
						dtEvolve = _timer->getRealTime();
						evolveWorld();
						dtEvolve = _timer->getRealTime() - dtEvolve;
						avgEvolve = (avgEvolve*loop + static_cast<double>(dtEvolve)) / (++loop);
					}
					else
						wasPaused = false;
					_frameTime = _timer->getRealTime();
					dtDraw = _timer->getRealTime();
					_videoDriver->beginScene(true, true, irr::video::SColor(255, 100, 101, 140));
					_sceneManager->drawAll();
					_guiEnvironment->drawAll();
					_videoDriver->endScene();
					avgDraw = ((avgDraw*(loop - 1) + static_cast<double>(_timer->getRealTime() - dtDraw))) / loop;
					if ((static_cast<int>(loop) % 60) == 0)
						std::cerr << "Draw time: " << avgDraw << "ms" << std::endl << "evolve time: " << avgEvolve << std::endl;
					
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

	}
}