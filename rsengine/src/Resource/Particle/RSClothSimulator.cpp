/******************************************************************************/
/*!
\file   RSClothSimulator.cpp
\author Jinseob Park
\date   2025/11/30

This file contains cloth simulator implementation for Mass-Spring / PBD simulation.

*/
/******************************************************************************/
#include "pch.h"
#include "RSClothSimulator.h"
#include "../Manager/RSShaderManager.h"
#include "../Manager/RSResourceManager.h"

namespace RS_Cloth
{
	//********************************************************************************
	// Constructor / Destructor
	//********************************************************************************

	RSClothSimulator::RSClothSimulator()
	{
		// Default initialization in header
	}

	RSClothSimulator::~RSClothSimulator()
	{
		Shutdown();
	}

	//********************************************************************************
	// Core Functions
	//********************************************************************************

	void RSClothSimulator::Initialize()
	{
		if (m_initialized)
		{
			RS_WARN("Cloth Simulator already initialized");
			return;
		}

		RS_INFO("Initializing Cloth Simulator...");

		// Create cloth particle grid
		SetClothField();

		// Create spring constraints
		CreateSprings();

		// Create GPU buffers
		CreateClothBuffers();

		// Create rendering resources
		CreateRenderResources();

		// Transfer data to GPU
		TransferParticleDataToGPU();
		TransferSpringDataToGPU();

		m_initialized = true;
		RS_INFO("Cloth Simulator initialized: %d particles, %d springs", m_particle_count, m_spring_count);
	}

	void RSClothSimulator::Shutdown()
	{
		if (!m_initialized)
			return;

		RS_INFO("Shutting down Cloth Simulator...");

		ResetAllBuffers();
		DeallocateCloth();

		m_initialized = false;
	}

	void RSClothSimulator::Update(float dt)
	{
		if (!m_initialized || !b_playing)
			return;

		// Fixed timestep accumulation
		m_accumulated_time += dt;
		const float fixed_dt = m_cloth_property.time_step;

		while (m_accumulated_time >= fixed_dt)
		{
			// Choose solver
			switch (m_solver_type)
			{
			case RSClothSolverType::MASS_SPRING:
				ComputeMassSpring(fixed_dt);
				break;
			case RSClothSolverType::PBD:
				ComputePBD(fixed_dt);
				break;
			}

			m_accumulated_time -= fixed_dt;
		}

		// Calculate normals for rendering
		CalculateNormals();
	}

	void RSClothSimulator::ForwardDraw()
	{
		if (!m_initialized)
			return;

		// Use Cloth Render shader
		auto shader_manager = RSResourceManager::GetInstance()->GetShaderManager();
		auto camera = RSResourceManager::GetInstance()->GetCamera();

		shader_manager->Use(RS_PipelineList::RSShaderNames::CLOTH_RENDER);

		// Set transformation matrices
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_model", glm::mat4(1.0f));
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_view", camera->GetView());
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_projection", camera->GetProj());
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "cloth_width", m_cloth_width);

		// Set material properties
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_cloth_color", glm::vec3(0.8f, 0.2f, 0.2f));
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_roughness", 0.5f);
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_metallic", 0.0f);

		// Set lighting
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_light_pos", glm::vec3(10.0f, 20.0f, 10.0f));
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_light_color", glm::vec3(1.0f, 1.0f, 1.0f));
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_view_pos", camera->GetPosition());
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_ambient_strength", 0.3f);
		shader_manager->SetData(RS_PipelineList::RSShaderNames::CLOTH_RENDER, "u_use_texture", false);

		// Bind SSBOs for vertex shader
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_POSITION_BINDING, m_position_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_NORMAL_BINDING, m_normal_ssbo);

		// Draw cloth mesh
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glDrawElements(GL_TRIANGLES, m_triangle_count * 3, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	void RSClothSimulator::DeferredDraw()
	{
		if (!m_initialized)
			return;

		// Use Deferred Cloth Render shader
		auto shader_manager = RSResourceManager::GetInstance()->GetShaderManager();
		auto camera = RSResourceManager::GetInstance()->GetCamera();

		shader_manager->Use(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER);

		// Set transformation matrices
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_model", glm::mat4(1.0f));
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_view", camera->GetView());
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_projection", camera->GetProj());
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "cloth_width", m_cloth_width);

		// Set material properties
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_cloth_color", glm::vec3(0.8f, 0.2f, 0.2f));
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_roughness", 0.5f);
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_metallic", 0.0f);

		// Set lighting (for G-Buffer, minimal lighting info needed)
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_light_pos", glm::vec3(10.0f, 20.0f, 10.0f));
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_light_color", glm::vec3(1.0f, 1.0f, 1.0f));
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_view_pos", camera->GetPosition());
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_ambient_strength", 0.3f);
		shader_manager->SetData(RS_PipelineList::RSShaderNames::DEFERRED_CLOTH_RENDER, "u_use_texture", false);

		// Bind SSBOs for vertex shader
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_POSITION_BINDING, m_position_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_NORMAL_BINDING, m_normal_ssbo);

		// Draw cloth mesh
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glDrawElements(GL_TRIANGLES, m_triangle_count * 3, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}

	//********************************************************************************
	// Initialization Functions
	//********************************************************************************

	void RSClothSimulator::SetClothField()
	{
		SetClothField(m_init_setting.width, m_init_setting.height, m_init_setting.spacing);
	}

	void RSClothSimulator::SetClothField(int width, int height, float spacing)
	{
		m_cloth_width = width;
		m_cloth_height = height;
		m_particle_count = width * height;

		m_particles.clear();
		m_particles.resize(m_particle_count);

		const glm::vec3 start_pos = m_init_setting.start_position;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				const int index = GetParticleIndex(x, y);
				RSClothParticle& p = m_particles[index];

				// Position
				p.position = glm::vec4(
					start_pos.x + x * spacing,
					start_pos.y,
					start_pos.z + y * spacing,
					1.0f / m_cloth_property.mass  // w = inverse mass
				);

				// Previous position (same as current initially)
				p.prev_position = p.position;
				p.prev_position.w = 0.0f;

				// Velocity (zero initially)
				p.velocity = glm::vec4(0.0f);

				// Normal (up initially)
				p.normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
			}
		}

		// Set fixed particles (pin constraints)
		// Top-left corner
		if (m_init_setting.fix_top_left)
		{
			m_particles[GetParticleIndex(0, 0)].position.w = 0.0f;
		}

		// Top-right corner
		if (m_init_setting.fix_top_right)
		{
			m_particles[GetParticleIndex(width - 1, 0)].position.w = 0.0f;
		}

		// Bottom-left corner
		if (m_init_setting.fix_bottom_left)
		{
			m_particles[GetParticleIndex(0, height - 1)].position.w = 0.0f;
		}

		// Bottom-right corner
		if (m_init_setting.fix_bottom_right)
		{
			m_particles[GetParticleIndex(width - 1, height - 1)].position.w = 0.0f;
		}

		// Fix entire top edge
		if (m_init_setting.fix_top_edge)
		{
			for (int x = 0; x < width; ++x)
			{
				m_particles[GetParticleIndex(x, 0)].position.w = 0.0f;
			}
		}

		RS_INFO("Cloth field created: %dx%d = %d particles", width, height, m_particle_count);
	}

	void RSClothSimulator::CreateSprings()
	{
		m_springs.clear();

		// Structural springs (horizontal and vertical)
		for (int y = 0; y < m_cloth_height; ++y)
		{
			for (int x = 0; x < m_cloth_width; ++x)
			{
				// Horizontal spring (right)
				if (x < m_cloth_width - 1)
				{
					AddSpring(GetParticleIndex(x, y), GetParticleIndex(x + 1, y), RSSpringType::STRUCTURAL);
				}

				// Vertical spring (down)
				if (y < m_cloth_height - 1)
				{
					AddSpring(GetParticleIndex(x, y), GetParticleIndex(x, y + 1), RSSpringType::STRUCTURAL);
				}
			}
		}

		// Shear springs (diagonal)
		for (int y = 0; y < m_cloth_height - 1; ++y)
		{
			for (int x = 0; x < m_cloth_width - 1; ++x)
			{
				// Diagonal (top-left to bottom-right)
				AddSpring(GetParticleIndex(x, y), GetParticleIndex(x + 1, y + 1), RSSpringType::SHEAR);

				// Diagonal (top-right to bottom-left)
				AddSpring(GetParticleIndex(x + 1, y), GetParticleIndex(x, y + 1), RSSpringType::SHEAR);
			}
		}

		// Bending springs (skip one)
		for (int y = 0; y < m_cloth_height; ++y)
		{
			for (int x = 0; x < m_cloth_width; ++x)
			{
				// Horizontal bend (skip one)
				if (x < m_cloth_width - 2)
				{
					AddSpring(GetParticleIndex(x, y), GetParticleIndex(x + 2, y), RSSpringType::BEND);
				}

				// Vertical bend (skip one)
				if (y < m_cloth_height - 2)
				{
					AddSpring(GetParticleIndex(x, y), GetParticleIndex(x, y + 2), RSSpringType::BEND);
				}
			}
		}

		m_spring_count = static_cast<unsigned int>(m_springs.size());
		RS_INFO("Springs created: %d total (Structural + Shear + Bend)", m_spring_count);
	}

	void RSClothSimulator::CreateClothBuffers()
	{
		// Position SSBO (binding 10)
		glGenBuffers(1, &m_position_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_position_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 
			m_particle_count * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

		// Previous Position SSBO (binding 11)
		glGenBuffers(1, &m_prev_position_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_prev_position_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 
			m_particle_count * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

		// Velocity SSBO (binding 12)
		glGenBuffers(1, &m_velocity_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_velocity_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 
			m_particle_count * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

		// Spring SSBO (binding 13)
		glGenBuffers(1, &m_spring_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_spring_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 
			m_spring_count * sizeof(RSClothSpring), nullptr, GL_DYNAMIC_DRAW);

		// Normal SSBO (binding 14)
		glGenBuffers(1, &m_normal_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_normal_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 
			m_particle_count * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		RS_INFO("Cloth GPU buffers created");
	}

	void RSClothSimulator::ResetAllBuffers()
	{
		if (m_position_ssbo) { glDeleteBuffers(1, &m_position_ssbo); m_position_ssbo = 0; }
		if (m_prev_position_ssbo) { glDeleteBuffers(1, &m_prev_position_ssbo); m_prev_position_ssbo = 0; }
		if (m_velocity_ssbo) { glDeleteBuffers(1, &m_velocity_ssbo); m_velocity_ssbo = 0; }
		if (m_spring_ssbo) { glDeleteBuffers(1, &m_spring_ssbo); m_spring_ssbo = 0; }
		if (m_normal_ssbo) { glDeleteBuffers(1, &m_normal_ssbo); m_normal_ssbo = 0; }

		if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
		if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
		if (m_ebo) { glDeleteBuffers(1, &m_ebo); m_ebo = 0; }
	}

	void RSClothSimulator::DeallocateCloth()
	{
		m_particles.clear();
		m_springs.clear();
		m_indices.clear();

		m_particle_count = 0;
		m_spring_count = 0;
		m_triangle_count = 0;
	}

	//********************************************************************************
	// Simulation Functions (CPU fallback - GPU compute shader version later)
	//********************************************************************************

	void RSClothSimulator::ComputeMassSpring(float dt)
	{
		// Bind SSBOs for compute shader
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_POSITION_BINDING, m_position_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_PREV_POSITION_BINDING, m_prev_position_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_VELOCITY_BINDING, m_velocity_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_SPRING_BINDING, m_spring_ssbo);

		// TODO: Dispatch compute shaders
		// 1. ComputeClothForce.comp - Calculate spring forces + gravity + wind
		// 2. ComputeClothIntegration.comp - Verlet integration
		// 3. ComputeClothCollision.comp - Handle collisions

		// For now, CPU simulation fallback
		CPUSimulateMassSpring(dt);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void RSClothSimulator::ComputePBD(float dt)
	{
		// Bind SSBOs
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_POSITION_BINDING, m_position_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_PREV_POSITION_BINDING, m_prev_position_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_VELOCITY_BINDING, m_velocity_ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, CLOTH_SPRING_BINDING, m_spring_ssbo);

		// TODO: Dispatch PBD compute shaders
		// 1. Predict positions
		// 2. Solve distance constraints (iterate)
		// 3. Update velocities

		// For now, CPU simulation fallback
		CPUSimulatePBD(dt);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	void RSClothSimulator::ApplyExternalForce(const glm::vec3& force)
	{
		m_external_force = force;
	}

	void RSClothSimulator::ApplyWind(const glm::vec3& wind_direction, float wind_strength)
	{
		m_wind_direction = glm::normalize(wind_direction);
		m_wind_strength = wind_strength;
	}

	void RSClothSimulator::HandleSphereCollision(const RSCollisionSphere& sphere)
	{
		m_collision_sphere = sphere;
	}

	void RSClothSimulator::HandlePlaneCollision(const RSCollisionPlane& plane)
	{
		m_collision_plane = plane;
	}

	void RSClothSimulator::CalculateNormals()
	{
		// CPU-based normal calculation
		// Read positions from GPU
		std::vector<glm::vec4> positions(m_particle_count);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_position_ssbo);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), positions.data());

		// Initialize normals to zero
		std::vector<glm::vec4> normals(m_particle_count, glm::vec4(0.0f));

		// Calculate face normals and accumulate to vertices
		for (int y = 0; y < m_cloth_height - 1; ++y)
		{
			for (int x = 0; x < m_cloth_width - 1; ++x)
			{
				int top_left = GetParticleIndex(x, y);
				int top_right = GetParticleIndex(x + 1, y);
				int bottom_left = GetParticleIndex(x, y + 1);
				int bottom_right = GetParticleIndex(x + 1, y + 1);

				glm::vec3 p0 = glm::vec3(positions[top_left]);
				glm::vec3 p1 = glm::vec3(positions[bottom_left]);
				glm::vec3 p2 = glm::vec3(positions[top_right]);
				glm::vec3 p3 = glm::vec3(positions[bottom_right]);

				// First triangle normal
				glm::vec3 edge1 = p1 - p0;
				glm::vec3 edge2 = p2 - p0;
				glm::vec3 n1 = glm::cross(edge1, edge2);

				// Second triangle normal
				glm::vec3 edge3 = p1 - p2;
				glm::vec3 edge4 = p3 - p2;
				glm::vec3 n2 = glm::cross(edge3, edge4);

				// Accumulate normals
				normals[top_left] += glm::vec4(n1, 0.0f);
				normals[bottom_left] += glm::vec4(n1 + n2, 0.0f);
				normals[top_right] += glm::vec4(n1 + n2, 0.0f);
				normals[bottom_right] += glm::vec4(n2, 0.0f);
			}
		}

		// Normalize all normals
		for (unsigned int i = 0; i < m_particle_count; ++i)
		{
			glm::vec3 n = glm::vec3(normals[i]);
			float len = glm::length(n);
			if (len > 0.0001f)
				normals[i] = glm::vec4(n / len, 0.0f);
			else
				normals[i] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Default up normal
		}

		// Upload to GPU
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_normal_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), normals.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	//********************************************************************************
	// CPU Simulation (Fallback before compute shaders are ready)
	//********************************************************************************

	void RSClothSimulator::CPUSimulateMassSpring(float dt)
	{
		const float damping = m_cloth_property.damping;
		const glm::vec3 gravity(0.0f, -m_cloth_property.gravity, 0.0f);

		// Read positions from GPU
		std::vector<glm::vec4> positions(m_particle_count);
		std::vector<glm::vec4> prev_positions(m_particle_count);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_position_ssbo);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), positions.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_prev_position_ssbo);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), prev_positions.data());

		// Calculate accelerations from springs
		std::vector<glm::vec3> accelerations(m_particle_count, glm::vec3(0.0f));

		// Add gravity
		for (unsigned int i = 0; i < m_particle_count; ++i)
		{
			if (positions[i].w > 0.0f)  // Not fixed
			{
				accelerations[i] += gravity;
			}
		}

		// Add wind force
		if (m_wind_strength > 0.0f)
		{
			glm::vec3 wind_force = m_wind_direction * m_wind_strength;
			for (unsigned int i = 0; i < m_particle_count; ++i)
			{
				if (positions[i].w > 0.0f)
				{
					accelerations[i] += wind_force;
				}
			}
		}

		// Spring forces
		for (const auto& spring : m_springs)
		{
			glm::vec3 p1 = glm::vec3(positions[spring.p1]);
			glm::vec3 p2 = glm::vec3(positions[spring.p2]);

			glm::vec3 delta = p2 - p1;
			float current_length = glm::length(delta);

			if (current_length > 0.0001f)
			{
				glm::vec3 direction = delta / current_length;
				float displacement = current_length - spring.rest_length;
				glm::vec3 force = spring.stiffness * displacement * direction;

				if (positions[spring.p1].w > 0.0f)
					accelerations[spring.p1] += force * positions[spring.p1].w;

				if (positions[spring.p2].w > 0.0f)
					accelerations[spring.p2] -= force * positions[spring.p2].w;
			}
		}

		// Verlet integration
		for (unsigned int i = 0; i < m_particle_count; ++i)
		{
			if (positions[i].w > 0.0f)  // Not fixed
			{
				glm::vec3 current = glm::vec3(positions[i]);
				glm::vec3 prev = glm::vec3(prev_positions[i]);

				glm::vec3 new_pos = current + damping * (current - prev) + accelerations[i] * dt * dt;

				prev_positions[i] = glm::vec4(current, prev_positions[i].w);
				positions[i] = glm::vec4(new_pos, positions[i].w);
			}
		}

		// Collision detection
		if (b_use_sphere_collision)
		{
			for (unsigned int i = 0; i < m_particle_count; ++i)
			{
				if (positions[i].w > 0.0f)
				{
					glm::vec3 pos = glm::vec3(positions[i]);
					glm::vec3 to_center = pos - m_collision_sphere.center;
					float dist = glm::length(to_center);

					if (dist < m_collision_sphere.radius + m_cloth_property.collision_radius)
					{
						glm::vec3 normal = to_center / dist;
						positions[i] = glm::vec4(
							m_collision_sphere.center + normal * (m_collision_sphere.radius + m_cloth_property.collision_radius),
							positions[i].w
						);
					}
				}
			}
		}

		if (b_use_plane_collision)
		{
			for (unsigned int i = 0; i < m_particle_count; ++i)
			{
				if (positions[i].w > 0.0f)
				{
					glm::vec3 pos = glm::vec3(positions[i]);
					float dist = glm::dot(pos - m_collision_plane.point, m_collision_plane.normal);

					if (dist < m_cloth_property.collision_radius)
					{
						positions[i] = glm::vec4(
							pos + m_collision_plane.normal * (m_cloth_property.collision_radius - dist),
							positions[i].w
						);
					}
				}
			}
		}

		// Write back to GPU
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_position_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), positions.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_prev_position_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), prev_positions.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void RSClothSimulator::CPUSimulatePBD(float dt)
	{
		const float damping = m_cloth_property.damping;
		const glm::vec3 gravity(0.0f, -m_cloth_property.gravity, 0.0f);
		const int iterations = m_cloth_property.solver_iterations;

		// Read positions from GPU
		std::vector<glm::vec4> positions(m_particle_count);
		std::vector<glm::vec4> prev_positions(m_particle_count);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_position_ssbo);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), positions.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_prev_position_ssbo);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), prev_positions.data());

		// Store old positions
		std::vector<glm::vec3> old_positions(m_particle_count);
		for (unsigned int i = 0; i < m_particle_count; ++i)
		{
			old_positions[i] = glm::vec3(positions[i]);
		}

		// Predict positions (explicit Euler)
		for (unsigned int i = 0; i < m_particle_count; ++i)
		{
			if (positions[i].w > 0.0f)  // Not fixed
			{
				glm::vec3 vel = glm::vec3(positions[i]) - glm::vec3(prev_positions[i]);
				vel *= damping;

				// Add external forces
				glm::vec3 acceleration = gravity;
				if (m_wind_strength > 0.0f)
				{
					acceleration += m_wind_direction * m_wind_strength;
				}

				glm::vec3 predicted = glm::vec3(positions[i]) + vel + acceleration * dt * dt;
				positions[i] = glm::vec4(predicted, positions[i].w);
			}
		}

		// Solve constraints (Gauss-Seidel iteration)
		for (int iter = 0; iter < iterations; ++iter)
		{
			for (const auto& spring : m_springs)
			{
				float w1 = positions[spring.p1].w;
				float w2 = positions[spring.p2].w;
				float w_sum = w1 + w2;

				if (w_sum < 0.0001f) continue;  // Both fixed

				glm::vec3 p1 = glm::vec3(positions[spring.p1]);
				glm::vec3 p2 = glm::vec3(positions[spring.p2]);

				glm::vec3 delta = p2 - p1;
				float current_length = glm::length(delta);

				if (current_length < 0.0001f) continue;

				float diff = (current_length - spring.rest_length) / current_length;
				glm::vec3 correction = delta * diff * 0.5f;

				if (w1 > 0.0f)
				{
					positions[spring.p1] = glm::vec4(p1 + correction * (w1 / w_sum), w1);
				}
				if (w2 > 0.0f)
				{
					positions[spring.p2] = glm::vec4(p2 - correction * (w2 / w_sum), w2);
				}
			}
		}

		// Collision constraints
		if (b_use_sphere_collision)
		{
			for (unsigned int i = 0; i < m_particle_count; ++i)
			{
				if (positions[i].w > 0.0f)
				{
					glm::vec3 pos = glm::vec3(positions[i]);
					glm::vec3 to_center = pos - m_collision_sphere.center;
					float dist = glm::length(to_center);

					if (dist < m_collision_sphere.radius + m_cloth_property.collision_radius)
					{
						glm::vec3 normal = to_center / dist;
						positions[i] = glm::vec4(
							m_collision_sphere.center + normal * (m_collision_sphere.radius + m_cloth_property.collision_radius),
							positions[i].w
						);
					}
				}
			}
		}

		if (b_use_plane_collision)
		{
			for (unsigned int i = 0; i < m_particle_count; ++i)
			{
				if (positions[i].w > 0.0f)
				{
					glm::vec3 pos = glm::vec3(positions[i]);
					float dist = glm::dot(pos - m_collision_plane.point, m_collision_plane.normal);

					if (dist < m_cloth_property.collision_radius)
					{
						positions[i] = glm::vec4(
							pos + m_collision_plane.normal * (m_cloth_property.collision_radius - dist),
							positions[i].w
						);
					}
				}
			}
		}

		// Update previous positions
		for (unsigned int i = 0; i < m_particle_count; ++i)
		{
			prev_positions[i] = glm::vec4(old_positions[i], prev_positions[i].w);
		}

		// Write back to GPU
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_position_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), positions.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_prev_position_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), prev_positions.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	//********************************************************************************
	// Helper Functions
	//********************************************************************************

	int RSClothSimulator::GetParticleIndex(int x, int y) const
	{
		return y * m_cloth_width + x;
	}

	void RSClothSimulator::AddSpring(int p1, int p2, RSSpringType type)
	{
		RSClothSpring spring;
		spring.p1 = p1;
		spring.p2 = p2;

		// Calculate rest length
		glm::vec3 pos1 = glm::vec3(m_particles[p1].position);
		glm::vec3 pos2 = glm::vec3(m_particles[p2].position);
		spring.rest_length = glm::length(pos2 - pos1);

		// Set stiffness based on type
		switch (type)
		{
		case RSSpringType::STRUCTURAL:
			spring.stiffness = m_cloth_property.structural_stiffness;
			break;
		case RSSpringType::SHEAR:
			spring.stiffness = m_cloth_property.shear_stiffness;
			break;
		case RSSpringType::BEND:
			spring.stiffness = m_cloth_property.bend_stiffness;
			break;
		}

		m_springs.push_back(spring);
	}

	void RSClothSimulator::TransferParticleDataToGPU()
	{
		// Extract position data
		std::vector<glm::vec4> positions(m_particle_count);
		std::vector<glm::vec4> prev_positions(m_particle_count);
		std::vector<glm::vec4> velocities(m_particle_count);
		std::vector<glm::vec4> normals(m_particle_count);

		for (unsigned int i = 0; i < m_particle_count; ++i)
		{
			positions[i] = m_particles[i].position;
			prev_positions[i] = m_particles[i].prev_position;
			velocities[i] = m_particles[i].velocity;
			normals[i] = m_particles[i].normal;
		}

		// Upload to GPU
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_position_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), positions.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_prev_position_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), prev_positions.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_velocity_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), velocities.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_normal_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_particle_count * sizeof(glm::vec4), normals.data());

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void RSClothSimulator::TransferSpringDataToGPU()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_spring_ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_spring_count * sizeof(RSClothSpring), m_springs.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void RSClothSimulator::CreateRenderResources()
	{
		// Create VAO
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ebo);

		// Create index buffer for mesh rendering
		CreateIndexBuffer();

		glBindVertexArray(m_vao);

		// VBO will be linked to position SSBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_particle_count * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
		glEnableVertexAttribArray(0);

		// EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	void RSClothSimulator::CreateIndexBuffer()
	{
		m_indices.clear();

		// Create triangles for rendering
		for (int y = 0; y < m_cloth_height - 1; ++y)
		{
			for (int x = 0; x < m_cloth_width - 1; ++x)
			{
				int top_left = GetParticleIndex(x, y);
				int top_right = GetParticleIndex(x + 1, y);
				int bottom_left = GetParticleIndex(x, y + 1);
				int bottom_right = GetParticleIndex(x + 1, y + 1);

				// First triangle
				m_indices.push_back(top_left);
				m_indices.push_back(bottom_left);
				m_indices.push_back(top_right);

				// Second triangle
				m_indices.push_back(top_right);
				m_indices.push_back(bottom_left);
				m_indices.push_back(bottom_right);
			}
		}

		m_triangle_count = static_cast<unsigned int>(m_indices.size()) / 3;
	}

	void RSClothSimulator::StartSimulation()
	{
		b_playing = true;
		RS_INFO("Cloth simulation started");
	}

	void RSClothSimulator::ResetSimulation()
	{
		b_playing = false;
		m_accumulated_time = 0.0f;

		// Recreate cloth with initial settings
		SetClothField();
		CreateSprings();
		TransferParticleDataToGPU();
		TransferSpringDataToGPU();

		RS_INFO("Cloth simulation reset");
	}

} // namespace RS_Cloth
