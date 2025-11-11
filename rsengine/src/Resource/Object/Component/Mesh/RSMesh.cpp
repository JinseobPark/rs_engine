#include "pch.h"
#include "RSMesh.h"
#include "Util/RSUtilFunctions.h"
#include "Resource/Manager/RSResourceManager.h"

namespace RS_Mesh
{
	RSMesh::RSMesh()
	{
		Initialize();
	}

	RSMesh::~RSMesh()
	= default;

	RSMesh::RSMesh(const RSMesh& other)
	{
    texture_id			= other.texture_id;
    m_mesh_data			= other.m_mesh_data;
    m_bounding_box	= other.m_bounding_box;
    is_texture			= other.is_texture;
		m_material			= other.m_material;
	}

	void RSMesh::Initialize()
	{
		texture_id = RSResourceManager::GetInstance()->GetTextureManager()->GetTexture("default")->GetTexture();
    m_material = RSResourceManager::GetInstance()->GetMaterialManager()->GetMaterial("default");
		SetMeshData(RSResourceManager::GetInstance()->GetMeshManager()->GetMesh("Cube"));
	}

	void RSMesh::SetMeshData(RSMeshData* data)
	{
		m_mesh_data = data;
		m_bounding_box.SetMinMax(m_mesh_data->min, m_mesh_data->max);
	}

	RSMeshData* RSMesh::GetMeshData() const
	{
		return m_mesh_data;
	}

	unsigned int RSMesh::GetTextureID() const
	{
		return texture_id;
	}

	void RSMesh::SetBoundingBox(const glm::vec3& min, const glm::vec3& max)
	{
		m_bounding_box.SetMinMax(min, max);
	}

	RSBoundingBox* RSMesh::GetBoundingBox()
	{
		return &m_bounding_box;
	}

	const std::vector<RSBoundingTriangle>& RSMesh::GetTriangles() const
	{
		return m_bounding_box.GetTriangles();
	}

	float RSMesh::GetRayDistance(const glm::vec3& ray_origin, const glm::vec3& ray_direction,
                               glm::vec3&       hit_position) const
	{
		// Check collision with mesh data
		if (m_mesh_data)
		{
			float result_distance = FLT_MAX;
			for (int i = 0; i < m_mesh_data->indices.size(); i += 3)
			{
				const float distance_tri = RS_Util::GetDistanceRayTriangle(
					                          ray_origin, ray_direction,
				                            m_mesh_data->vertices[m_mesh_data->indices[i]].position,
				                            m_mesh_data->vertices[m_mesh_data->indices[i + 1]].position,
				                            m_mesh_data->vertices[m_mesh_data->indices[i + 2]].position);
				if (distance_tri > 0.0f)
				{
					result_distance = glm::min(result_distance, distance_tri);
					hit_position = ray_origin + ray_direction * result_distance;
				}
			}
			return result_distance;
		}

		return FLT_MAX;
	}
	void RSMesh::SetTextureID(const unsigned int id)
	{
		texture_id = id;
	}
	bool RSMesh::GetIsTexture() const
	{
		return is_texture;
	}
	void RSMesh::SetIsTexture(const bool is)
	{
		is_texture = is;
	}
	void RSMesh::SetBoundingBox(const RSBoundingBox& box)
	{
		m_bounding_box = box;
	}

  RS_Material::RSMaterial* RSMesh::GetMaterial() const
	{
    return m_material;
  }

  void RSMesh::SetMaterial(RS_Material::RSMaterial* material)
  {
    m_material = material;
  }

  void RSMesh::SetMaterial(const std::string& name)
  {
    if (const auto p_material = RSResourceManager::GetInstance()->GetMaterialManager()->GetMaterial(name))
      m_material = p_material;
	}

  bool RSMesh::IsMaterialExist() const
  {
    return m_material != nullptr;
  }

}
