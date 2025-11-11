#include "pch.h"
#include "RSMeshManager.h"
#include "GL/glew.h"
#include "../IO/RSSTLIO.h"
#include "../IO/RSOBJIO.h"


namespace RS_Mesh
{
	RSMeshManager::RSMeshManager()
	{
		GenerateInitMeshes();
	}

	RSMeshManager::~RSMeshManager() = default;

	void RSMeshManager::Initialize()
	{
	}

	void RSMeshManager::Shutdown()
	{
		for (auto it = m_meshes.begin(); it != m_meshes.end(); ++it)
		{
			glDeleteVertexArrays(1, &it->second->vao);
			it->second->vao = 0;
			glDeleteBuffers(1, &it->second->vbo);
			it->second->vbo = 0;
			delete it->second;
		}
		m_meshes.clear();
	}


	bool RSMeshManager::LoadMesh(const std::string& name_, std::string path_)
	{
		auto mesh = new RSMeshData();
		if (RS_FILE_IO::RSOBJIO::ImportObj(path_, *mesh))
		{
			glGenVertexArrays(1, &mesh->vao);
			glGenBuffers(1, &mesh->vbo);
			glGenBuffers(1, &mesh->ebo);
			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(RSVertex), mesh->vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), mesh->indices.data(),
                   GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, position));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, normal));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RSVertex),(void*)offsetof(RSVertex, texture_coordinates));
			glBindVertexArray(0);

			std::wstring path_w;
			path_w.assign(path_.begin(), path_.end());
			mesh->mesh_data_path = path_w;
			m_meshes[name_] = mesh;
		}
		else
		{
			delete mesh;
			RS_WARN("Failed to load mesh: %s", path_.c_str());
			return false;
		}
		return true;
	}

	bool RSMeshManager::LoadMeshStl2(const std::string& name_, std::string path_)
	{
		auto mesh = new RSMeshData();
		if (RS_FILE_IO::RSSTLIO::ImportStl(path_, *mesh))
		{
			glGenVertexArrays(1, &mesh->vao);
			glGenBuffers(1, &mesh->vbo);
			glGenBuffers(1, &mesh->ebo);
			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(RSVertex), mesh->vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), mesh->indices.data(),
                   GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, position));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, normal));
			glBindVertexArray(0);

			std::wstring path_w;
			path_w.assign(path_.begin(), path_.end());
			mesh->mesh_data_path = path_w;
			m_meshes[name_] = mesh;
		}
		else
		{
			delete mesh;
			RS_WARN("Failed to load mesh: %s", path_.c_str());
			return false;
		}
		return true;
	}

	bool RSMeshManager::LoadMesh(const std::string& name_, const std::wstring& path_)
	{
		auto mesh = new RSMeshData();
		if (RS_FILE_IO::RSOBJIO::ImportObj(path_, *mesh))
		{
			glGenVertexArrays(1, &mesh->vao);
			glGenBuffers(1, &mesh->vbo);
			glGenBuffers(1, &mesh->ebo);
			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(RSVertex), mesh->vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), mesh->indices.data(),
                   GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, position));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, normal));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, texture_coordinates));
			glBindVertexArray(0);

			mesh->mesh_data_path = path_;
			m_meshes[name_] = mesh;
		}
		else
		{
			delete mesh;
			return false;
		}
		return true;
	}

	bool RSMeshManager::LoadMeshStl2(const std::string& name_, const std::wstring& path_)
	{
		auto mesh = new RSMeshData();
		if (RS_FILE_IO::RSSTLIO::ImportStl(path_, *mesh))
		{
			glGenVertexArrays(1, &mesh->vao);
			glGenBuffers(1, &mesh->vbo);
			glGenBuffers(1, &mesh->ebo);
			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(RSVertex), mesh->vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), mesh->indices.data(),
                   GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, position));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, normal));
			glBindVertexArray(0);

			mesh->mesh_data_path = path_;
			m_meshes[name_] = mesh;
		}
		else
		{
			delete mesh;
			return false;
		}
		return true;
	}

	bool RSMeshManager::IsExistMesh(const std::string& name_) const
	{
		return m_meshes.contains(name_);
	}

	void RSMeshManager::ReleaseMesh(const std::string& name_)
	{
		if (const auto it = m_meshes.find(name_); it != m_meshes.end())
		{
			glDeleteVertexArrays(1, &it->second->vao);
			it->second->vao = 0;
			glDeleteBuffers(1, &it->second->vbo);
			it->second->vbo = 0;
			delete it->second;
			m_meshes.erase(it);
		}
	}

	void RSMeshManager::GenerateInitMeshes()
	{
		const auto mesh_null = new RSMeshData();
		m_meshes["NULL"] = mesh_null;

		m_meshes["Cube"] = CreateCubeMesh();
		m_meshes["Plane"] = CreatePlaneMesh();
		m_meshes["Grid"] = CreateGrid();
		m_meshes["Sphere"] = CreateSphereMesh();
		m_meshes["Sphere2"] = CreateSphereTeMesh(3);
		m_meshes["Cube2"] = CreateCubeMesh(3);
	}

	void RSMeshManager::InitGizmoMeshes()
	{
		GenerateInitGizmoMesh(GIZMO_TRANSLATE_PLANE_NAME, GIZMO_TRANSLATE_PLANE_AXIS_DATA);
    GenerateInitGizmoMesh(GIZMO_ROTATE_AXIS_NAME, GIZMO_ROTATE_AXIS_DATA);
    GenerateInitGizmoMesh(GIZMO_SCALE_AXIS_NAME, GIZMO_SCALE_AXIS_DATA);
    GenerateInitGizmoMesh(GIZMO_TRANSLATE_CENTER_NAME, GIZMO_TRANSLATE_CENTER_DATA);
    GenerateInitGizmoMesh(GIZMO_TRANSLATE_AXIS_NAME, GIZMO_TRANSLATE_AXIS_DATA);
    GenerateInitGizmoMesh(GIZMO_SCALE_CENTER_NAME, GIZMO_SCALE_CENTER_DATA);
	}

	void RSMeshManager::GenerateInitGizmoMesh(const std::string& mesh_name, const unsigned char mesh_data[])
	{
		const auto mesh = new RSMeshData();
		if (RS_FILE_IO::RSSTLIO::ImportEmbeddedMeshData(mesh_data, *mesh))
		{
			glGenVertexArrays(1, &mesh->vao);
			glGenBuffers(1, &mesh->vbo);
			glGenBuffers(1, &mesh->ebo);
			glBindVertexArray(mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
			glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(RSVertex), mesh->vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), mesh->indices.data(),
                   GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, position));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RSVertex), (void*)offsetof(RSVertex, normal));
			glBindVertexArray(0);

			// mesh_name convert to wstring
      std::wstring mesh_name_w;
      mesh_name_w.assign(mesh_name.begin(), mesh_name.end());

			mesh->mesh_data_path = mesh_name_w;

			m_meshes[mesh_name] = mesh;
		}
		else
		{
			delete mesh;
			RS_WARN("Failed to load mesh: %s", mesh_name.c_str());
			return ;
		}
	}

}
