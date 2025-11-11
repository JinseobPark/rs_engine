#include "pch.h"
#include "RSMeshWidget.h"
#include "Resource/Manager/RSResourceManager.h"

namespace RS_Widget
{
	using RS_Shader::RSShaderNames;
	RSMeshWidget::RSMeshWidget()
	{
		m_mesh = new RSMesh();
	}

	RSMeshWidget::~RSMeshWidget()
	{
		delete m_mesh;
	}

	void RSMeshWidget::Initialize()
	{
		RSWidgetBase::Initialize();
	}

	void RSMeshWidget::PostInitialize()
	{
	}

	void RSMeshWidget::Update(float dt)
	{
		RSWidgetBase::Update(dt);
	}

	void RSMeshWidget::Draw()
	{
	}

	void RSMeshWidget::DrawVisible()
	{
		glBindTexture(GL_TEXTURE_2D, m_texture_id);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "model",
                                                                  GetModelMatrix() * m_fixed_rotation);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "color", m_color);
		glBindVertexArray(m_mesh->GetMeshData()->vao);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_mesh->GetMeshData()->indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void RSMeshWidget::DrawHittable()
	{
	}



}