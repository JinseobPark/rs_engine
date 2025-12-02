#include "pch.h"
#include "ClothSimulatorLevel.h"
#include "GameData.h"
#include "../LevelData/ClothSimulatorData.h"

namespace SJG
{
    ClothSimulatorData* m_cloth_simulator_data = nullptr;
    RS_Cloth::RSClothSimulator* m_cloth_simulator = nullptr;

    void ClothSimulatorLoad()
    {
        m_resource_manager = RSResourceManager::GetInstance();
        m_resource_manager->GetParticleManager()->CreateClothSimulator();
        m_cloth_simulator_data = new ClothSimulatorData();
        m_cloth_simulator = m_resource_manager->GetParticleManager()->GetClothSimulator();
    }

    void ClothSimulatorInitialize()
    {
        m_cloth_simulator_data->Initialize();
        m_cloth_simulator->Initialize();

    }

    void ClothSimulatorUpdate(float dt)
    {
        m_cloth_simulator_data->Update(dt);
        m_cloth_simulator->Update(dt);


        GameData::GetInstance()->Update(dt);

    }
    void ClothSimulatorShutdown()
    {
        m_cloth_simulator_data->Shutdown();
        m_cloth_simulator->Shutdown();
    }
    void ClothSimulatorUnload()
    {
        m_resource_manager->GetParticleManager()->ShutdownClothSimulator();
        delete m_cloth_simulator_data;
        m_cloth_simulator_data = nullptr;
        m_cloth_simulator = nullptr;
    }
}