#pragma once

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
			void startEventLoop();
			virtual void setupWorld() = 0;
		protected:
			void init(unsigned int in_screenWidth = 800, unsigned int in_screenHeight = 600);
			float getTimeFromPreviousFrame();
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