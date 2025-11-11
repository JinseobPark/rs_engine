#include "pch.h"
#include <iostream>
#include "RSEngine.h"

#if defined(DEBUG) || defined(_DEBUG)
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif


    // NVIDIA
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

// AMD
extern "C" {
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main()
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1275);
	//_crtBreakAlloc( number); //number = memory leak number
#endif

	try
	{
		RS_Engine::RSEngine renderer;

		const bool result = renderer.Initialize();
		assert(result);

		renderer.Run();
	}
	catch (_exception)
	{
		std::cout << "failed. program down\n";
		return 0;
	}
	return 0;

}