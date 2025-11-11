#include "pch.h"
#include "RSModel.h"
#include "Resource/Manager/RSResourceManager.h"

namespace RS_Component
{
	RSModel::~RSModel()
	{
		if (m_mesh != nullptr)
		{
			delete m_mesh;
			m_mesh = nullptr;
		}
	}
	RSModel::RSModel(const RSModel& other): RSComponent(RSComponentType::CT_MODEL)
  {
    m_is_visible   = other.m_is_visible;
    m_color        = other.m_color;
    is_opaque      = other.is_opaque;
    m_transparency = other.m_transparency;
    m_mesh				 = new RSMesh(*other.m_mesh);
  }

  void RSModel::Draw() 
	{
		if (GetMesh() == nullptr) return;
		if (!GetMesh()->IsMaterialExist()) return;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GetMesh()->GetTextureID());
		glBindVertexArray(GetMesh()->GetMeshData()->vao);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(GetMesh()->GetMeshData()->indices.size()), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		
	}
	void RSModel::DrawWireFrame() 
	{
		if (GetMesh() == nullptr) return;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RSResourceManager::GetInstance()->GetTextureManager()->
				            GetTexture("default")->GetTexture());
		glBindVertexArray(GetMesh()->GetMeshData()->vao);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(GetMesh()->GetMeshData()->indices.size()), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	void RSModel::DrawPbr()
	{
		if (GetMesh() == nullptr) return;
		if (!GetMesh()->IsMaterialExist()) return;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GetMesh()->GetMaterial()->GetDiffuseMap());
		glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, GetMesh()->GetMaterial()->GetNormalMap());

    glBindVertexArray(GetMesh()->GetMeshData()->vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(GetMesh()->GetMeshData()->indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
	}

	void RSModel::SetTransparency(const float transparency_)
	{
		m_transparency = glm::clamp(transparency_, 0.0f, 1.0f);
		if (m_transparency < 1.0f)
			SetIsOpaqueObject(false);
		else
			SetIsOpaqueObject(true);
	}

	void RSModel::DrawBoundingBox()
  {
    if (GetMesh() == nullptr) return;
    GetMesh()->GetBoundingBox()->DrawBoundingBox();
	}

  void RSModel::SetTiling(const glm::vec2& tiling_)
  {
    m_tiling = tiling_;
    UpdateTexTransform();
  }

  void RSModel::SetTilling(const float x_, const float y_)
  {
    m_tiling = glm::vec2(x_, y_);
    UpdateTexTransform();
  }

  void RSModel::SetOffset(const glm::vec2& offset_)
  {
    m_offset = offset_;
    UpdateTexTransform();
  }

  void RSModel::SetOffset(const float x_, const float y_)
  {
    m_offset = glm::vec2(x_, y_);
    UpdateTexTransform();
  }

  void RSModel::UpdateTexTransform()
  {
    m_tex_transform = glm::mat4(1.0f);
    m_tex_transform = glm::translate(m_tex_transform, glm::vec3(m_offset, 0.0f));
    m_tex_transform = glm::scale(m_tex_transform, glm::vec3(m_tiling, 1.0f));
  }

	//void RSModel::SetDrawType(DrawType draw_type)
	//{
	//	// If the draw type is already set, return
	//	if (m_draw_type == draw_type) return;

	//	m_draw_type = draw_type;
	//	RSResourceManager* resource_manager = RSResourceManager::GetInstance();
	//	RSObjectManager* object_manager = resource_manager->GetObjectManager();

	//	// Set the render layer based on the draw type
	//	switch (draw_type)
	//	{
	//		case DrawType::SURFACE:
	//			GetOwner()->SetRenderLayer(RSRenderLayer::R_SURFACE_OPAQUE);

	//			break;
	//	}
	//	/*
	//	m_render_layer = RSRenderLayer::R_DEFAULT;
	//	*/
	//}
}
