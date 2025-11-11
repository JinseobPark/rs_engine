#include "pch.h"
#include "RSPhysics.h"
#include "Resource/Object/Component/Rigidbody/RSRigidbody.h"

namespace RS_Physics
{
	RSPhysics::RSPhysics() = default;

	RSPhysics::~RSPhysics() = default;

	void RSPhysics::Initialize()
	{
	}

	void RSPhysics::PostInitialize()
	{
		m_resource_manager = RSResourceManager::GetInstance();
		m_object_manager = m_resource_manager->GetObjectManager();
		RS_ASSERT(m_object_manager != nullptr, "Object manager is nullptr");
	}

	void RSPhysics::Update(const float dt)
	{
		for (const auto& object : *m_object_manager->GetObjects())
		{
			const auto rigidbody = dynamic_cast<RS_Component::RSRigidbody*>(object.second->
                                                                             GetComponent(RSComponentType::CT_RIGIDBODY));
			if (rigidbody != nullptr)
			{
				rigidbody->Update(dt);
			}
		}
	}

	void RSPhysics::Shutdown()
	{
		m_resource_manager = nullptr;
		m_object_manager = nullptr;
	}
}