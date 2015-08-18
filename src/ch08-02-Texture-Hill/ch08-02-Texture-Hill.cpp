#ifdef _DEBUG
#pragma comment( linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#ifdef _WIN32
#define _XM_NO_INTRINSICS_
#endif 

#include "RenderSystem.h"
#include <memory>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	auto app = std::make_shared<byhj::RenderSystem>();
	app->Run();

	return 0;
}