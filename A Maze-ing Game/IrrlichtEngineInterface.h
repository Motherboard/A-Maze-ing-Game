#pragma once

//This class is an interface for an irrlicht game engine.
//Currently it is very primitive - it should be modified to run the world model and rendering procedures
//at different time intervals (possibly even multi-threaded)
//This is on my todo list.

namespace irr
{
	class IrrlichtDevice;
	namespace video
	{
		class IVideoDriver;
	}
	namespace gui
	{
		class IGUIEnvironment;
	}
	namespace scene
	{
		class ISceneManager;
	}
	class ITimer;
}

namespace irr
{
	namespace extra
	{

		class CIrrlichtEngineInterface
		{
		public:
			CIrrlichtEngineInterface();
			virtual ~CIrrlichtEngineInterface();
			//The main function which would start the game - when this function ends, the game exits.
			void startEventLoop();
			//Setup the scenegraph according to the underlying game world model.
			virtual void setupWorld() = 0;
		protected:
			//Initialize the game and set the resolution
			void init(unsigned int in_screenWidth = 800, unsigned int in_screenHeight = 600);
			//Get time in seconds from last frame to determine how much to evolve from previous state.
			float getTimeFromPreviousFrame();
			//Evolve world model (and currently also draw the scene)
			//TODO: split evolveWorld into evolveWorld and renderWorld.
			virtual void evolveWorld() = 0;


			irr::IrrlichtDevice *_device = nullptr;
			irr::video::IVideoDriver *_videoDriver = nullptr;
			irr::scene::ISceneManager *_sceneManager = nullptr;
			irr::gui::IGUIEnvironment *_guiEnvironment = nullptr;
			irr::ITimer * _timer = nullptr;
		private:
			unsigned int _frameTime;

		};

	}
}