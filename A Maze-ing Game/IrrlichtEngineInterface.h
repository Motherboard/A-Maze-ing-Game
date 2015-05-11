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
class CIrrlichtEngineInterface
{
public:
	CIrrlichtEngineInterface();
	virtual ~CIrrlichtEngineInterface();
	void startEventLoop();
	virtual void setupWorld() = 0;
protected:
	void init();
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

