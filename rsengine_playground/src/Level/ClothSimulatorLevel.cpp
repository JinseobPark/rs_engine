#include "pch.h"
#include "ClothSimulatorLevel.h"
#include "GameData.h"

namespace SJG
{
    void ClothSimulatorLoad()
    {

    }

    void ClothSimulatorInitialize()
    {

    }

    void ClothSimulatorUpdate(float dt)
    {
        GameData::GetInstance()->Update(dt);

    }
    void ClothSimulatorShutdown()
    {
        
    }
    void ClothSimulatorUnload()
    {
        
    }
}