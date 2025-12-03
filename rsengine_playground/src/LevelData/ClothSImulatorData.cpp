#include "pch.h"
#include "ClothSimulatorData.h"
#include "GameData.h"

namespace SJG
{
    ClothSimulatorData::ClothSimulatorData()
    {
        m_sphere_object = nullptr;
        m_plane_object = nullptr;
        m_resource_manager = RSResourceManager::GetInstance();
    }

    ClothSimulatorData::~ClothSimulatorData()
    {
    }

    void ClothSimulatorData::Initialize()
    {
        // background color is gray
        m_resource_manager->GetGraphicsData()->clear_color[0] = 0.3f;
        m_resource_manager->GetGraphicsData()->clear_color[1] = 0.3f;
        m_resource_manager->GetGraphicsData()->clear_color[2] = 0.3f;

        // Sphere Object
        m_sphere_object = m_resource_manager->GetObjectManager()->CreateSphereObject("Cloth_Sphere");
        m_sphere_object->GetTransform()->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));

        // Plane Object
        m_plane_object = m_resource_manager->GetObjectManager()->CreatePlaneObject("Cloth_Plane");
        m_plane_object->GetTransform()->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));


    }

    void ClothSimulatorData::Shutdown()
    {
        auto object_manager = m_resource_manager->GetObjectManager();
        if (m_sphere_object != nullptr)
        {
            object_manager->RemoveObject(m_sphere_object);
            m_sphere_object = nullptr;
        }
        if (m_plane_object != nullptr)
        {
            object_manager->RemoveObject(m_plane_object);
            m_plane_object = nullptr;
        }
    }

    void ClothSimulatorData::Update(float dt)
    {
    }
} // namespace SJG