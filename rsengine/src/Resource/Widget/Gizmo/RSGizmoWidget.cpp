#include "pch.h"
#include "RSGizmoWidget.h"
#include "Resource/Manager/RSResourceManager.h"

namespace RS_Widget
{
	using RS_Shader::RSShaderNames;

	RSGizmoWidget::RSGizmoWidget()
	{
		m_mesh = new RSMesh();
		SetSize(100, 100, 100);
	}

	RSGizmoWidget::~RSGizmoWidget()
	{
		delete m_mesh;
	}

	void RSGizmoWidget::Initialize()
	{
		RS_Widget::RSButtonWidget::Initialize();
	}

	void RSGizmoWidget::PostInitialize()
	{
	}

	void RSGizmoWidget::Update(const float dt)
	{
		RS_Widget::RSButtonWidget::Update(dt);
		is_hovered = (RSResourceManager::GetInstance()->GetGraphicsData()->m_pixel_id == widget_id);

		m_color = is_hovered ? selected_color : default_color;
	}

	void RSGizmoWidget::DrawHittable()
	{
		if(is_draw)
		{
			glBindTexture(GL_TEXTURE_2D, m_texture_id);
			//RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_MAPPING, "model", m_transform.GetTransformMatrix());
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_MAPPING, "model",
                                                                    GetModelMatrix() * m_fixed_rotation);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_MAPPING, "color_id", widget_id);
			glBindVertexArray(m_mesh->GetMeshData()->vao);
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_mesh->GetMeshData()->indices.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	void RSGizmoWidget::DrawVisible()
	{

		if (is_draw)
		{
			glBindTexture(GL_TEXTURE_2D, m_texture_id);
			//RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "model", m_transform.GetTransformMatrix());
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "model",
                                                                    GetModelMatrix() * m_fixed_rotation);
			RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "color", m_color);
			glBindVertexArray(m_mesh->GetMeshData()->vao);
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_mesh->GetMeshData()->indices.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}


	void RSGizmoWidget::Draw()
	{
	}

	void RSGizmoWidget::Destroy()
	{
	}

	void RSGizmoWidget::SetIsDraw(const bool is_draw_)
	{
		if(is_draw_)
		{
			is_draw = true;
			m_visibility = RSVisibility::HIT_VISIBLE;
		}
		else
		{
			is_draw = false;
			m_visibility = RSVisibility::NONE_HIT_INVISIBLE;
		}
	}

  void RSGizmoWidget::SetGizmoAxisType(const GizmoAxisType axis)
  {
    // All gizmo object meshes are x-axis oriented. So y, z axis objects should be rotated.
		switch (axis)
		{
    case GizmoAxisType::Y_AXIS:
    {
      m_fixed_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      break;
    }
    case GizmoAxisType::Z_AXIS:
    {
      m_fixed_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      break;
    }
		case GizmoAxisType::Y_AXIS_INV:
		{
			m_fixed_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		}
    case GizmoAxisType::Z_AXIS_INV:
    {
      m_fixed_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      break;
    }
    default:
      m_fixed_rotation = glm::mat4(1.0f);
      break;
		}
  }
} // namespace SPH_MINT