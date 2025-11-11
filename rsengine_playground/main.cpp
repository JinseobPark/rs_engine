#include "pch.h"
#include "GameStateManager.h"
#include "GameData.h"
#define GLEW_STATIC // This must be defined when statically linking to GLEW
#pragma comment (lib, "glew32s.lib") // Link to glew32s.lib

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

int main(void)
{
#if defined(DEBUG) || defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1993289);
	//_crtBreakAlloc( number); //number = memory leak number
#endif
	RS_Engine::RSEngine my_game;

	// ENGINE INIT

	RS_MSG("ROCK & STONE ENGINE OPERATIONAL");


	my_game.Initialize();
	SJG::GameData::GetInstance()->Initialize();
	// Add State
	SJG::GameData::GetInstance()->GetGameStateManager()->EnrollStates();

	// ENGINE RUN
	my_game.Run();


	SJG::GameData::GetInstance()->Shutdown();

	SJG::GameData::Release();
	my_game.Shutdown();

	return 0;
}