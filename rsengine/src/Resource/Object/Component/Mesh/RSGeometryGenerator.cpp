#include "pch.h"
#include "RSGeometryGenerator.h"
#include "glm/gtc/constants.hpp"


namespace RS_Mesh
{
	RSMeshData* RSGeometryGenerator::CreateGrid(const unsigned int rows_, const unsigned int cols_)
	{
		const auto mesh = new RSMeshData();
		const UINT32 vertex_count = rows_ * cols_;
		const size_t face_count = (rows_ - 1) * (cols_ - 1) * 2;

		constexpr float half_width = 0.5f;
		constexpr float half_depth = 0.5f;

		const float dx = 1.0f / static_cast<float>(rows_ - 1);
		const float dz = 1.0f / static_cast<float>(cols_ - 1);

		mesh->vertices.resize(vertex_count);
		for (UINT32 i = 0; i < rows_; i++)
		{
			const float z = half_depth - static_cast<float>(i) * dz;
			for (UINT32 j = 0; j < cols_; j++)
			{
				const float x                                     = -half_width + static_cast<float>(j) * dx;
				mesh->vertices[i * cols_ + j].position            = glm::vec3(x, 0.0f, z);
				mesh->vertices[i * cols_ + j].normal              = glm::vec3(0.0f, 1.0f, 0.0f);
				mesh->vertices[i * cols_ + j].texture_coordinates = glm::vec2(static_cast<float>(j) * dx,
                                                                      static_cast<float>(i) * dz);
			}
		}

		// Create the indices.

		mesh->indices.resize(face_count * 3);
		UINT32 k = 0;
		for (UINT32 i = 0; i < rows_ - 1; i++)
		{
			for (UINT32 j = 0; j < cols_ - 1; j++)
			{
				mesh->indices[k] = i * cols_ + j;
				mesh->indices[k + 1] = i * cols_ + j + 1;
				mesh->indices[k + 2] = (i + 1) * cols_ + j;

				mesh->indices[k + 3] = (i + 1) * cols_ + j;
				mesh->indices[k + 4] = i * cols_ + j + 1;
				mesh->indices[k + 5] = (i + 1) * cols_ + j + 1;

				k += 6; // next quad
			}
		}
		mesh->min = glm::vec3(-half_width, 0.0f, -half_depth);
		mesh->max = glm::vec3(half_width, 0.0f, half_depth);
		mesh->has_texcoord = true;

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

		//m_meshes["Grid"] = mesh;
		return mesh;
	}

	RSMeshData* RSGeometryGenerator::CreatePlaneMesh()
	{
		const auto mesh = new RSMeshData();
		mesh->vertices.resize(4);
		mesh->indices.resize(6);
		mesh->has_texcoord = true;

		mesh->vertices = {
			{glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 0.0)},
			{glm::vec3(0.5, 0.0, -0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 0.0)},
			{glm::vec3(0.5, 0.0, 0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 1.0)},
			{glm::vec3(-0.5, 0.0, 0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 1.0)}
		};
		mesh->indices = {
			0, 1, 2,
			2, 3, 0
		};
		mesh->min = glm::vec3(-0.5, 0.0, -0.5);
		mesh->max = glm::vec3(0.5, 0.0, 0.5);

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

		//m_meshes["Plane"] = mesh;
		return mesh;
	}

	RSMeshData* RSGeometryGenerator::CreateCubeMesh(UINT32 num_sub_divisions_)
	{
		const auto mesh = new RSMeshData();
		mesh->vertices.resize(24);
		mesh->indices.resize(36);
		mesh->has_texcoord = true;

		mesh->vertices = {
			// Front face
			{glm::vec3(-0.5, -0.5,  0.5), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.0, 0.0)},
			{glm::vec3(0.5, -0.5,  0.5), glm::vec3(0.0, 0.0, 1.0), glm::vec2(1.0, 0.0)},
			{glm::vec3(0.5,  0.5,  0.5), glm::vec3(0.0, 0.0, 1.0), glm::vec2(1.0, 1.0)},
			{glm::vec3(-0.5,  0.5,  0.5), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.0, 1.0)},

			// Back face
			{glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.0, 0.0)},
			{glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.0, 0.0, -1.0), glm::vec2(1.0, 0.0)},
			{glm::vec3(-0.5,  0.5, -0.5), glm::vec3(0.0, 0.0, -1.0), glm::vec2(1.0, 1.0)},
			{glm::vec3(0.5,  0.5, -0.5), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.0, 1.0)},

			// Top face
			{glm::vec3(-0.5,  0.5,  0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 0.0)},
			{glm::vec3(0.5,  0.5,  0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 0.0)},
			{glm::vec3(0.5,  0.5, -0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 1.0)},
			{glm::vec3(-0.5,  0.5, -0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 1.0)},

			// Bottom face
			{glm::vec3(-0.5, -0.5,  0.5), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.0, 0.0)},
			{glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.0, 1.0)},
			{glm::vec3(0.5, -0.5, -0.5), glm::vec3(0.0, -1.0, 0.0), glm::vec2(1.0, 1.0)},
			{glm::vec3(0.5, -0.5,  0.5), glm::vec3(0.0, -1.0, 0.0), glm::vec2(1.0, 0.0)},

			// Right face
			{glm::vec3(0.5,  0.5, -0.5), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 0.0)},
			{glm::vec3(0.5,  0.5,  0.5), glm::vec3(1.0, 0.0, 0.0), glm::vec2(1.0, 0.0)},
			{glm::vec3(0.5, -0.5,  0.5), glm::vec3(1.0, 0.0, 0.0), glm::vec2(1.0, 1.0)},
			{glm::vec3(0.5, -0.5, -0.5), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 1.0)},

			// Left face
			{glm::vec3(-0.5,  0.5,  0.5), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.0, 0.0)},
			{glm::vec3(-0.5,  0.5, -0.5), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(1.0, 0.0)},
			{glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(1.0, 1.0)},
			{glm::vec3(-0.5, -0.5,  0.5), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.0, 1.0)},
		};
		mesh->indices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4,
			8, 9, 10, 10, 11, 8,
			12, 13, 14, 14, 15, 12,
			16, 17, 18, 18, 19, 16,
			20, 21, 22, 22, 23, 20
		};
		mesh->min = glm::vec3(-0.5, -0.5, -0.5);
		mesh->max = glm::vec3(0.5, 0.5, 0.5);

		// self tessellation
		if (num_sub_divisions_ != 0)
		{
			num_sub_divisions_ = glm::min<UINT32>(num_sub_divisions_, 6u);

			for (UINT32 i = 0; i < num_sub_divisions_; ++i)
			{
				Subdivide(mesh);
			}
		}

		// Load to buffer
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

		return mesh;
	}

	// default sphere mesh
	RSMeshData* RSGeometryGenerator::CreateSphereMesh(UINT32 slice_count_, UINT32 stack_count_)
	{
		const auto mesh = new RSMeshData();
		constexpr float radius = 0.5f;
		RSVertex top_vertex = { glm::vec3(0.0f, +radius, 0.0f), glm::vec3(0.0f, +1.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
		RSVertex bottom_vertex = { glm::vec3(0.0f, -radius, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) };

		mesh->vertices.push_back(top_vertex);

		float phi_step = glm::pi<float>() / static_cast<float>(stack_count_);
		float theta_step = 2.0f * glm::pi<float>() / static_cast<float>(slice_count_);

		for (UINT32 i = 1; i <= stack_count_ - 1; ++i)
		{
			float phi = static_cast<float>(i) * phi_step;

			for (UINT32 j = 0; j <= slice_count_; ++j)
			{
				float theta = static_cast<float>(j) * theta_step;

				RSVertex v;
				v.position.x = radius * sinf(phi) * cosf(theta);
				v.position.y = radius * cosf(phi);
				v.position.z = radius * sinf(phi) * sinf(theta);

				v.normal = glm::normalize(v.position);
				v.texture_coordinates.x = theta / glm::two_pi<float>();
				v.texture_coordinates.y = phi / glm::pi<float>();

				mesh->vertices.push_back(v);
			}
		}
		mesh->vertices.push_back(bottom_vertex);

		for (UINT32 i = 1; i <= slice_count_; ++i)
		{
			mesh->indices.push_back(0);
			mesh->indices.push_back(i + 1);
			mesh->indices.push_back(i);
		}

		UINT32 base_index = 1;
		UINT32 ring_vertex_count = slice_count_ + 1;
		for (UINT32 i = 0; i < stack_count_ - 2; ++i)
		{
			for (UINT32 j = 0; j < slice_count_; ++j)
			{
				mesh->indices.push_back(base_index + i * ring_vertex_count + j);
				mesh->indices.push_back(base_index + i * ring_vertex_count + j + 1);
				mesh->indices.push_back(base_index + (i + 1) * ring_vertex_count + j);

				mesh->indices.push_back(base_index + (i + 1) * ring_vertex_count + j);
				mesh->indices.push_back(base_index + i * ring_vertex_count + j + 1);
				mesh->indices.push_back(base_index + (i + 1) * ring_vertex_count + j + 1);
			}
		}

		UINT32 south_pole_index = static_cast<UINT32>(mesh->vertices.size()) - 1;
		base_index = south_pole_index - ring_vertex_count;

		for (UINT32 i = 0; i < slice_count_; ++i)
		{
			mesh->indices.push_back(south_pole_index);
			mesh->indices.push_back(base_index + i);
			mesh->indices.push_back(base_index + i + 1);
		}

		mesh->max = glm::vec3(radius, radius, radius);
		mesh->min = glm::vec3(-radius, -radius, -radius);
		mesh->has_texcoord = true;



		// Load to buffer
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

		return mesh;
	}

	// Tessellate a sphere
	RSMeshData* RSGeometryGenerator::CreateSphereTeMesh(UINT32 num_sub_divisions_)
	{
		const auto mesh = new RSMeshData();
		constexpr float radius = 1.0f;

		num_sub_divisions_ = glm::min<UINT32>(num_sub_divisions_, 6u);

		constexpr float x = 0.525731f;
		constexpr float z = 0.850651f;

		constexpr glm::vec3 pos[12] = {
			glm::vec3(-x, 0.0f, z),  glm::vec3(x, 0.0f, z),
			glm::vec3(-x, 0.0f, -z), glm::vec3(x, 0.0f, -z),
			glm::vec3(0.0f, z, x),   glm::vec3(0.0f, z, -x),
			glm::vec3(0.0f, -z, x),  glm::vec3(0.0f, -z, -x),
			glm::vec3(z, x, 0.0f),   glm::vec3(-z, x, 0.0f),
			glm::vec3(z, -x, 0.0f),  glm::vec3(-z, -x, 0.0f)
		};

		//UINT32 k[60] = {
		//	1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		//	1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		//	3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		//	10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
		//};

		mesh->vertices.resize(12);
		mesh->indices = {
			1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
			1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
			3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
			10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
		};

		for (UINT32 i = 0; i < 12; ++i)
			mesh->vertices[i].position = glm::normalize(pos[i]);

		for (UINT32 i = 0; i < num_sub_divisions_; ++i)
			Subdivide(mesh);

		for (auto& vertex : mesh->vertices)
		{
			vertex.position *= radius;
			vertex.normal = glm::normalize(vertex.position);

			float theta = atan2f(vertex.position.z, vertex.position.x);
			if (theta < 0.0f)
				theta += 2.0f * glm::pi<float>();

			const float phi = acosf(vertex.position.y);
			vertex.texture_coordinates.x = theta / (2.0f * glm::pi<float>());
			vertex.texture_coordinates.y = phi / glm::pi<float>();
		}


		mesh->max = glm::vec3(radius, radius, radius);
		mesh->min = glm::vec3(-radius, -radius, -radius);
		mesh->has_texcoord = true;

		// Load to buffer
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

		return mesh;
	}

	void RSGeometryGenerator::Subdivide(RSMeshData* mesh_)
	{
		auto [vertices, indices, min, max, vao, vbo, ebo, has_texcoord, mesh_data_path] = *mesh_;

		mesh_->vertices.resize(0);
		mesh_->indices.resize(0);

		UINT32 num_tris = static_cast<UINT32>(indices.size()) / 3;
		for (UINT32 i = 0; i < num_tris; ++i)
		{
			RSVertex v0 = vertices[indices[i * 3 + 0]];
			RSVertex v1 = vertices[indices[i * 3 + 1]];
			RSVertex v2 = vertices[indices[i * 3 + 2]];

			RSVertex m0 = MidPoint(v0, v1);
			RSVertex m1 = MidPoint(v1, v2);
			RSVertex m2 = MidPoint(v2, v0);

			mesh_->vertices.push_back(v0); // 0
			mesh_->vertices.push_back(v1); // 1
			mesh_->vertices.push_back(v2); // 2
			mesh_->vertices.push_back(m0); // 3
			mesh_->vertices.push_back(m1); // 4
			mesh_->vertices.push_back(m2); // 5

			mesh_->indices.push_back(i * 6 + 0);
			mesh_->indices.push_back(i * 6 + 3);
			mesh_->indices.push_back(i * 6 + 5);

			mesh_->indices.push_back(i * 6 + 3);
			mesh_->indices.push_back(i * 6 + 4);
			mesh_->indices.push_back(i * 6 + 5);

			mesh_->indices.push_back(i * 6 + 5);
			mesh_->indices.push_back(i * 6 + 4);
			mesh_->indices.push_back(i * 6 + 2);

			mesh_->indices.push_back(i * 6 + 3);
			mesh_->indices.push_back(i * 6 + 1);
			mesh_->indices.push_back(i * 6 + 4);
		}
	}

	RSVertex RSGeometryGenerator::MidPoint(const RSVertex& v0, const RSVertex& v1)
	{
		RSVertex result;
		result.position= 0.5f * (v0.position + v1.position);
		result.normal = glm::normalize(0.5f * (v0.normal + v1.normal));
		result.texture_coordinates = 0.5f * (v0.texture_coordinates + v1.texture_coordinates);

		return result;
	}

}