#include "pch.h"
#include "RSObject.h"
#include "../manager/RSResourceManager.h"

namespace RS_Object
{
	using namespace RS_Component;

	RSObject::RSObject()
	= default;

	RSObject::~RSObject()
	{
		for (const auto& component : m_components)
			delete component.second;
		m_name.clear();
	}

	RSObject::RSObject(const RSObject& other)
	{
    m_name = other.m_name;
    m_transform = other.m_transform;
    m_render_layer = other.m_render_layer;
    for (const auto& [fst, snd] : other.m_components)
    {
      // Deep copy constructor
      m_components[fst] = snd->Clone();

      // Set owner
      m_components[fst]->SetOwner(this);

    }
	}

	void RSObject::Initialize() const
	{
		for (auto& component : m_components)
			component.second->Initialize();
	}

	void RSObject::Update(float )
	{
	}

	void RSObject::Draw(const RSRenderLayer layer_)
	{
		if (m_components.contains(RSComponentType::CT_MODEL))
		{
			const auto p_model = dynamic_cast<RSModel*>(m_components[RSComponentType::CT_MODEL]);
			if (GetRenderLayer() == layer_ && p_model->IsVisible())
			{
				if (p_model->GetMesh() != nullptr)
				{
					glBindTexture(GL_TEXTURE_2D, p_model->GetMesh()->GetTextureID());
					glBindVertexArray(p_model->GetMesh()->GetMeshData()->vao);
					glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(p_model->GetMesh()->GetMeshData()->indices.size()),
                         GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
			}
		}
	}

	void RSObject::Destroy()
	{
	}

	bool RSObject::AddComponent(RSComponent* p_component)
	{
		if (p_component == nullptr) { return false; } //RSComponent is nullptr

		const RSComponentType type = p_component->GetType();
		if (m_components.contains(type))
			return false;

		m_components[type] = p_component;
		p_component->SetOwner(this);
		p_component->Initialize();

		return true;
	}

	bool RSObject::RemoveComponent(const RSComponentType type_)
	{
		const auto iter = m_components.find(type_);
		if (iter == m_components.end())
			return false;

		const RSComponent* p_component = iter->second;
		m_components.erase(iter);
		delete p_component;

		return true;
	}


	RSComponent* RSObject::GetComponent(const RSComponentType type_)
	{
		const auto iter = m_components.find(type_);
		if (iter == m_components.end())
			return nullptr;

		return iter->second;
	}

	void RSObject::SetRenderLayer(const RSRenderLayer layer_)
	{
		//If the draw type is already set, return
		if (m_render_layer == layer_) return;

		//	m_draw_type = draw_type;
		const RSResourceManager* resource_manager = RSResourceManager::GetInstance();
		RS_Object::RSObjectManager* object_manager = resource_manager->GetObjectManager();

		// move 
		object_manager->GetObjectLayers(layer_).push_back(this);
		object_manager->GetObjectLayers(m_render_layer).
                    erase(std::ranges::remove(object_manager->GetObjectLayers(m_render_layer), this).begin(),
                          object_manager->GetObjectLayers(m_render_layer).end());
		m_render_layer = layer_;
	}

	glm::mat4 RSObject::GetTransformMatrix()
	{
		return m_transform.GetTransformMatrix();
	}

	void RSObject::SetVisible(const bool visible)
	{
    if (m_components.contains(RSComponentType::CT_MODEL))
    {
      const auto p_model = dynamic_cast<RSModel*>(m_components[RSComponentType::CT_MODEL]);
      p_model->SetVisible(visible);
    }
	}


}