/******************************************************************************/
/*!
\file   RSParticleSystem.h
\author Jinseob Park
\date   2024/08/01

This file contains particle system, which includes particle count, texture id, and particle position.
This particle system is not SPH simulation, just particle system for game engine.

*/
/******************************************************************************/
#ifndef RS_PARTICLE_SYSTEM_H_
#define RS_PARTICLE_SYSTEM_H_

#include "glm/glm.hpp"

/**
 * @brief Particle namespace
 */
namespace RS_Particle
{
	/**
	 * @brief Particle System for GameEngine
	 * NOT SPH Simulation
	 * Just Particle System
	 * This class don't have any functions yet.
	 */
	class RSParticleSystem
	{
	public:
		RSParticleSystem();
		~RSParticleSystem();

    /**
     * @brief Initialize the particle system
		 */
		void Initialize();
    /**
     * @brief Deallocate ssbo and buffers
     * Deallocate Particle data
     */
		void ShutDown();

		void Update(float dt);
		void Draw();

		void CreateParticle(unsigned int particle_count);
		RS_N_D unsigned int GetParticleCount() const { return m_particle_count; }


		void SetTexture(const GLuint texture_id) { m_texture_id = texture_id; }

		RS_N_D std::string GetName() const { return m_name; }
		void SetName(const std::string& name) { m_name = name; }



	private:
		std::string m_name;
		unsigned int m_particle_count = 0;
		unsigned int m_current_particle_count = 0;

		unsigned int m_texture_id = 0;
		unsigned int m_vao = 0, m_vbo = 0, m_ebo = 0;


		std::vector<glm::vec3> m_positions;
		std::vector<glm::vec3> m_velocities;


	};
}

#endif // !RS_PARTICLE_SYSTEM_H_