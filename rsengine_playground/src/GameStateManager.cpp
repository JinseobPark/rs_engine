#include "pch.h"
#include "GameStateManager.h"
#include "Systems/State/RSState.h"
#include "RSEngine.h"
#include "Level/JordyRollLevel.h"
#include "Level/ParticleSimulatorLevel.h"
#include "Level/VTKLoaderLevel.h"
#include "Level/PointClipperLevel.h"

namespace SJG
{
	void GameStateManager::EnrollStates()
	{
		RSStatement jordy_level;
		jordy_level.Load = &JordyLoad;
		jordy_level.Init = &JordyInitialize;
		jordy_level.Update = &JordyUpdate;
		jordy_level.Shutdown = &JordyShutdown;
		jordy_level.Unload = &JordyUnload;
		RS_State::mRSSTATE->AddState(static_cast<unsigned int>(GameStateID::SRG_JORDY), jordy_level);

		RSStatement particle_simulator_level;
		particle_simulator_level.Load = &ParticleSimulatorLoad;
		particle_simulator_level.Init = &ParticleSimulatorInitialize;
		particle_simulator_level.Update = &ParticleSimulatorUpdate;
		particle_simulator_level.Shutdown = &ParticleSimulatorShutdown;
		particle_simulator_level.Unload = &ParticleSimulatorUnload;
		RS_State::mRSSTATE->AddState(static_cast<unsigned int>(GameStateID::SRG_PARTICLE_SIMULATOR), particle_simulator_level);

		RSStatement vtk_load_level;
    vtk_load_level.Load = &VTKLoaderLoad;
    vtk_load_level.Init = &VTKLoaderInitialize;
    vtk_load_level.Update = &VTKLoaderUpdate;
    vtk_load_level.Shutdown = &VTKLoaderShutdown;
    vtk_load_level.Unload = &VTKLoaderUnload;
    RS_State::mRSSTATE->AddState(static_cast<unsigned int>(GameStateID::SRG_VTK_LOADER), vtk_load_level);

		RSStatement point_clipper_level;
    point_clipper_level.Load = &PointClipperLoad;
    point_clipper_level.Init = &PointClipperInitialize;
    point_clipper_level.Update = &PointClipperUpdate;
    point_clipper_level.Shutdown = &PointClipperShutdown;
    point_clipper_level.Unload = &PointClipperUnload;
		RS_State::mRSSTATE->AddState(static_cast<unsigned int>(GameStateID::SRG_POINT_CLIPPER), point_clipper_level);


		RS_State::mRSSTATE->SetStartState(static_cast<unsigned int>(GameStateID::SRG_JORDY));

	}

}
