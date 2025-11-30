#include "pch.h"
#include "../../RSParticleSimulator.h"

#include <numbers>
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Application/RSApplication.h"
#include "Systems/State/RSState.h"
#include "Util/RSUtilFunctions.h"


namespace RS_Particle
{


	void RSParticleSimulator::SetParticleField()
	{
		m_cube_min = m_init_setting.init_particle_setting_min;
		m_cube_max = m_init_setting.init_particle_setting_max;
		m_space = m_init_setting.init_particle_setting_space;

		// All the values should be positive
		if (m_cube_min.x > m_cube_max.x)
			std::swap(m_cube_min.x, m_cube_max.x);
		if (m_cube_min.y > m_cube_max.y)
			std::swap(m_cube_min.y, m_cube_max.y);
		if (m_cube_min.z > m_cube_max.z)
			std::swap(m_cube_min.z, m_cube_max.z);

		if (m_cube_max.x - m_cube_min.x < m_space)
			m_cube_max.x += m_space;
		if (m_cube_max.y - m_cube_min.y < m_space)
			m_cube_max.y += m_space;
		if (m_cube_max.z - m_cube_min.z < m_space)
			m_cube_max.z += m_space;

		// Calculate the particle count
		const unsigned int x_count = static_cast<unsigned int>((m_cube_max.x - m_cube_min.x) / m_space);
		const unsigned int y_count = static_cast<unsigned int>((m_cube_max.y - m_cube_min.y) / m_space);
		const unsigned int z_count = static_cast<unsigned int>((m_cube_max.z - m_cube_min.z) / m_space);

		m_particle_count = x_count * y_count * z_count;

		if (m_particle_count > m_max_particle_count)
		{
			m_valid_particle_count = false;
			RS_FATAL("Err : Particle count exceed the max particle count");
		}
		else
			m_valid_particle_count = true;

		if (m_solver_type == RSSolverType::R_STATIC_GRID || m_solver_type == RSSolverType::R_DYNAMIC_GRID)
		{
			m_grid_count = static_cast<unsigned int>(m_init_setting.init_grid_resolution.x) * m_init_setting.
                     init_grid_resolution.y * m_init_setting.init_grid_resolution.z;
			m_grid_size = m_init_setting.boundary_max - m_init_setting.boundary_min;
			m_max_grid_count = static_cast<unsigned int>(m_particle_count * m_init_setting.grid_max_count_per_max);
		}


		m_particle_property.particle_radius = m_init_setting.init_particle_setting_space * 0.5f;
		m_particle_property.smoothing_length = m_init_setting.init_particle_setting_space * 2.0f;

		// V = 4/3 * pi * r^3
		const float volume = 4.0f / 3.0f * std::numbers::pi_v<float> * m_particle_property.particle_radius *
                         m_particle_property.particle_radius * m_particle_property.particle_radius;
		// mass = volume * density
		m_particle_property.mass = m_particle_property.density * volume;
	}


	void RSParticleSimulator::SetParticleField(const glm::vec3 min, const glm::vec3 max, const float space)
	{
		//m_cube_min = min;
		//m_cube_max = max;
		//m_space = space;

		//// All the values should be positive
		//if (m_cube_min.x > m_cube_max.x)
		//	std::swap(m_cube_min.x, m_cube_max.x);
		//if (m_cube_min.y > m_cube_max.y)
		//	std::swap(m_cube_min.y, m_cube_max.y);
		//if (m_cube_min.z > m_cube_max.z)
		//	std::swap(m_cube_min.z, m_cube_max.z);

		//if (m_cube_max.x - m_cube_min.x < m_space)
		//	m_cube_max.x += m_space;
		//if (m_cube_max.y - m_cube_min.y < m_space)
		//	m_cube_max.y += m_space;
		//if (m_cube_max.z - m_cube_min.z < m_space)
		//	m_cube_max.z += m_space;

		//// Calculate the particle count
		//const unsigned int x_count = static_cast<unsigned int>((m_cube_max.x - m_cube_min.x) / m_space);
		//const unsigned int y_count = static_cast<unsigned int>((m_cube_max.y - m_cube_min.y) / m_space);
		//const unsigned int z_count = static_cast<unsigned int>((m_cube_max.z - m_cube_min.z) / m_space);

		//m_particle_count = x_count * y_count * z_count;

		//if (m_particle_count > m_max_particle_count)
		//{
		//	m_valid_particle_count = false;
		//	RS_FATAL("Err : Particle count exceed the max particle count");
		//}
		//else
		//	m_valid_particle_count = true;

		//if (m_solver_type == RSSolverType::R_STATIC_GRID || m_solver_type == RSSolverType::R_DYNAMIC_GRID)
		//{
		//	m_grid_count = static_cast<unsigned int>(m_init_setting.init_grid_resolution.x) * m_init_setting.init_grid_resolution.y * m_init_setting.init_grid_resolution.z;
		//	m_grid_size = m_init_setting.boundary_max - m_init_setting.boundary_min;
		//	m_max_grid_count = m_particle_count * static_cast<unsigned int>(m_init_setting.grid_max_count_per_max);
		//}

		//// V = 4/3 * pi * r^3
		//const float volume = 4.0f / 3.0f * std::numbers::pi_v<float> *m_particle_property.particle_radius * m_particle_property.particle_radius * m_particle_property.particle_radius;
		//// mass = volume * density
		//m_particle_property.mass = m_particle_property.density * volume;
	}


	void RSParticleSimulator::ResizeParticleData()
	{
		// Resize the particle data
		m_positions.resize(m_particle_count);
		m_velocities.resize(m_particle_count);
		m_accelerations.resize(m_particle_count);
		instanceIDs.resize(m_particle_count);
		if (m_solver_type == RSSolverType::R_STATIC_GRID || m_solver_type == RSSolverType::R_DYNAMIC_GRID)
		{
			m_grid_debug.resize(m_grid_count * m_max_grid_count, -1);
			m_grid_count_debug.resize(m_grid_count, 0);
		}
	}

	void RSParticleSimulator::CreateParticleBuffer()
	{
		// Initialize the current buffer to zero
		m_current_buffer = 0;
		// if the buffer is already created, delete the buffer
		ResetAllBuffers();

		// Create SSBOs for the particle data
		glGenBuffers(1, &m_position_ssbo);
		glGenBuffers(1, &m_velocity_ssbo);
		glGenBuffers(1, &m_acceleration_ssbo);

		// Determine buffer usage pattern based on solver type
		// Static solvers: Data doesn't change after initialization -> GL_STATIC_DRAW
		// Dynamic solvers: Data changes every frame -> GL_DYNAMIC_DRAW
		GLenum buffer_usage = GL_DYNAMIC_DRAW;
		if (m_solver_type == RSSolverType::R_STATIC_BRUTE_FORCE ||
		    m_solver_type == RSSolverType::R_STATIC_GRID ||
		    m_solver_type == RSSolverType::R_STATIC_KDTREE ||
		    m_solver_type == RSSolverType::R_STATIC_OCTREE)
		{
			// Static solvers update data on GPU via compute shaders, not CPU uploads
			buffer_usage = GL_STATIC_DRAW;
		}

		// Create SSBOs with optimized usage hints
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_position_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_particle_count * sizeof(glm::vec4), m_positions.data(), buffer_usage);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_velocity_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_particle_count * sizeof(glm::vec4), m_velocities.data(), buffer_usage);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_acceleration_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_particle_count * sizeof(glm::vec4), m_accelerations.data(), buffer_usage);

		// Bind the current and result buffers
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_position_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_velocity_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_acceleration_ssbo);


		if (m_solver_type == RSSolverType::R_STATIC_GRID || m_solver_type == RSSolverType::R_DYNAMIC_GRID)
		{
			CreateGridBuffer();
		}

		// Rendering buffer
		// instancing buffer
		glGenBuffers(1, &m_instancing_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_instancing_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_particle_count * sizeof(unsigned int), instanceIDs.data(), GL_STATIC_DRAW);

		// VAO buffer
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_instancing_vbo);
		glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(unsigned int), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribDivisor(0, 1); // Update per instance

		glBindVertexArray(0);


	}

	void RSParticleSimulator::TransferMeshCollisionData()
	{
		// Update Collision Data to mesh vertices (ssbo number 5)
		if (b_any_collision_data_changed)
		{
			// Delete the previous buffer
			if (m_mesh_collision_ssbo)
				glDeleteBuffers(1, &m_mesh_collision_ssbo);


			// Create the SSBO for the mesh collision
			glGenBuffers(1, &m_mesh_collision_ssbo);

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_mesh_collision_ssbo);
			// Collision mesh is static, use GL_STATIC_DRAW for better GPU optimization
			glBufferData(GL_SHADER_STORAGE_BUFFER, m_mesh_vertices.size() * sizeof(glm::vec4), m_mesh_vertices.data(),
			             GL_STATIC_DRAW);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_mesh_collision_ssbo);

			b_any_collision_data_changed = false;
			b_collision_mesh_transferred = true;
		}
	}


	void RSParticleSimulator::AddMeshDataToCollision(const RSMeshData* mesh_data_, RS_Component::RSTransform& transform_,
                                                   const int         property_)
	{
		// Convert the mesh data to the triangle mesh
		RSTriangleMesh tri_mesh = RS_Util::ConvertToTriangleMesh(*mesh_data_);

		// Transform the triangle mesh
		tri_mesh.Transform(transform_.GetTransformMatrix());

		// Get vertex count for adding m_mesh_vertices resize.
		const size_t vertex_count = tri_mesh.triangles.size() * 3;

		// Add the mesh data to the collision
		const size_t mesh_vertex_size = m_mesh_vertices.size();
		m_mesh_vertices.reserve(mesh_vertex_size + vertex_count);
		m_mesh_vertices.resize(mesh_vertex_size + vertex_count);

		// Add the mesh data to the collision. It is already  memory pooling
		size_t adding_mesh_index = 0;
		for (size_t i = 0; i < vertex_count; i += 3, adding_mesh_index += 1)
		{
			m_mesh_vertices[mesh_vertex_size + i + 0] = glm::vec4(tri_mesh.triangles[adding_mesh_index].position[0], property_);
			m_mesh_vertices[mesh_vertex_size + i + 1] = glm::vec4(tri_mesh.triangles[adding_mesh_index].position[1], property_);
			m_mesh_vertices[mesh_vertex_size + i + 2] = glm::vec4(tri_mesh.triangles[adding_mesh_index].position[2], property_);
		}

		// Check the capacity and size of the mesh vertices
    RS_ASSERT(m_mesh_vertices.capacity() == m_mesh_vertices.size(),
              " Error : Mesh vertices vector capacity is not equal with size. ");

		b_any_collision_data_changed = true;
	}

	void RSParticleSimulator::AddMeshDataToCollision(RS_Object::RSObject* object_, const int property_)
	{
		const auto model = dynamic_cast<RS_Component::RSModel*>(object_->GetComponent(RSComponentType::CT_MODEL));
		if (!model)
		{
			RS_WARN("Error : There is no model component in the object");
			return;
		}

		// Get the mesh data from the model
		const RSMeshData* mesh_data = model->GetMesh()->GetMeshData();
		RS_Component::RSTransform transform = *object_->GetTransform();


		// Convert the mesh data to the triangle mesh
		RSTriangleMesh tri_mesh = RS_Util::ConvertToTriangleMesh(*mesh_data);
		tri_mesh.Transform(transform.GetTransformMatrix());

		// Get vertex count for adding m_mesh_vertices resize.
		const size_t vertex_count = tri_mesh.triangles.size() * 3;

		// Add the mesh data to the collision
		const size_t mesh_vertex_size = m_mesh_vertices.size();
		m_mesh_vertices.reserve(mesh_vertex_size + vertex_count);
		m_mesh_vertices.resize(mesh_vertex_size + vertex_count);

		// Add the mesh data to the collision. It is already  memory pooling
		size_t adding_mesh_index = 0;
		for (size_t i = 0; i < vertex_count; i += 3, adding_mesh_index += 1)
		{
			m_mesh_vertices[mesh_vertex_size + i + 0] = glm::vec4(tri_mesh.triangles[adding_mesh_index].position[0], property_);
			m_mesh_vertices[mesh_vertex_size + i + 1] = glm::vec4(tri_mesh.triangles[adding_mesh_index].position[1], property_);
			m_mesh_vertices[mesh_vertex_size + i + 2] = glm::vec4(tri_mesh.triangles[adding_mesh_index].position[2], property_);
		}

		// Check the capacity and size of the mesh vertices
    RS_ASSERT(m_mesh_vertices.capacity() == m_mesh_vertices.size(),
              " Error : Mesh vertices vector capacity is not equal with size. ");

		b_any_collision_data_changed = true;


	}

}