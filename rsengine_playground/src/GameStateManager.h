#pragma once

#include "Systems/State/RSState.h"

namespace SJG
{
	enum class GameStateID : unsigned int
	{
		SRG_JORDY = 1,
		SRG_PARTICLE_SIMULATOR = 2,
    SRG_VTK_LOADER = 3,
    SRG_POINT_CLIPPER = 4,
		SRG_GAME = 4,



	};

	class GameStateManager
	{
	public:
		GameStateManager() {}
		~GameStateManager() {}

		void EnrollStates();


	private:

	};

}