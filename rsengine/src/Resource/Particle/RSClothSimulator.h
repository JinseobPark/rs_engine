/******************************************************************************/
/*!
\file   RSClothSimulator.h
\author Jinseob Park
\date   2025/11/30

This file contains cloth simulator class for Mass-Spring / PBD cloth simulation.
Single Cloth System with Compute Shader for CLO Virtual Fashion Portfolio.

*/
/******************************************************************************/
#ifndef RS_CLOTH_SIMULATOR_H_
#define RS_CLOTH_SIMULATOR_H_

#include "glm/glm.hpp"
#include <GL/glew.h>
#include <vector>

//********************************************************************************
// SSBO Binding Constants for Cloth Simulation
// SPH uses binding 0-6, Cloth uses binding 10-15 to avoid conflicts
//********************************************************************************
constexpr int CLOTH_POSITION_BINDING      = 10;  ///< Current position buffer
constexpr int CLOTH_PREV_POSITION_BINDING = 11;  ///< Previous position buffer (for Verlet)
constexpr int CLOTH_VELOCITY_BINDING      = 12;  ///< Velocity buffer
constexpr int CLOTH_SPRING_BINDING        = 13;  ///< Spring constraints buffer
constexpr int CLOTH_NORMAL_BINDING        = 14;  ///< Normal buffer for rendering
constexpr int CLOTH_COLLISION_BINDING     = 5;   ///< Shared with SPH collision mesh

/**
 * @brief GUI Handler namespace
 */
namespace RS_Handler
{
	class RSImguiHandler;
}

/**
 * @brief Pipeline namespace
 */
namespace RS_PipelineList
{
	enum class RSComputeShaderNames;
}

/**
 * @brief Component namespace
 */
namespace RS_Component
{
	class RSTransform;
}

/**
 * @brief Object namespace
 */
namespace RS_Object
{
	class RSObject;
}

struct RSMeshData;

//********************************************************************************
// Cloth Simulation Data Structures
//********************************************************************************

/**
 * @brief Spring type for cloth simulation
 */
enum class RSSpringType : int
{
	STRUCTURAL = 0,  ///< Horizontal/Vertical springs (maintain grid structure)
	SHEAR = 1,       ///< Diagonal springs (prevent shearing)
	BEND = 2         ///< Skip-one springs (prevent excessive bending)
};

/**
 * @brief Cloth solver type
 */
enum class RSClothSolverType : int
{
	MASS_SPRING = 0,  ///< Traditional Mass-Spring with Verlet integration
	PBD = 1           ///< Position Based Dynamics (more stable)
};

/**
 * @brief Cloth particle structure for GPU (std430 layout compatible)
 * Packed as vec4 for efficient GPU access
 */
struct RSClothParticle
{
	glm::vec4 position;       ///< xyz: position, w: inverse mass (0 = fixed)
	glm::vec4 prev_position;  ///< xyz: previous position (for Verlet), w: unused
	glm::vec4 velocity;       ///< xyz: velocity, w: unused
	glm::vec4 normal;         ///< xyz: normal (for rendering), w: unused
};

/**
 * @brief Spring constraint structure for GPU (std430 layout compatible)
 */
struct RSClothSpring
{
	int p1;              ///< First particle index
	int p2;              ///< Second particle index
	float rest_length;   ///< Rest length of spring
	float stiffness;     ///< Spring stiffness coefficient
};

/**
 * @brief Cloth simulation properties
 */
struct RSClothProperty
{
	// Spring properties
	float structural_stiffness = 50.0f;   ///< Structural spring stiffness
	float shear_stiffness = 30.0f;        ///< Shear spring stiffness
	float bend_stiffness = 10.0f;         ///< Bending spring stiffness
	float damping = 0.99f;                ///< Velocity damping factor

	// Physics properties
	float gravity = 9.8f;                 ///< Gravity acceleration
	float mass = 1.0f;                    ///< Mass per particle
	float time_step = 0.016f;             ///< Fixed time step for simulation

	// Collision properties
	float collision_radius = 0.05f;       ///< Collision detection radius
	float collision_stiffness = 100.0f;   ///< Collision response stiffness

	// PBD specific
	int solver_iterations = 10;           ///< Constraint solver iterations
	float compliance = 0.0f;              ///< XPBD compliance (0 = stiff)
};

/**
 * @brief Initial cloth setup settings
 */
struct RSClothInitSetting
{
	glm::vec3 start_position = glm::vec3(-5.0f, 10.0f, -5.0f);  ///< Top-left corner position
	int width = 32;              ///< Number of particles in width
	int height = 32;             ///< Number of particles in height
	float spacing = 0.3f;        ///< Space between particles

	// Fixed corners (pin constraints)
	bool fix_top_left = true;
	bool fix_top_right = true;
	bool fix_bottom_left = false;
	bool fix_bottom_right = false;
	bool fix_top_edge = false;   ///< Fix entire top edge
};

/**
 * @brief Collision sphere for simple collision detection
 */
struct RSCollisionSphere
{
	glm::vec3 center = glm::vec3(0.0f, 5.0f, 0.0f);
	float radius = 2.0f;
};

/**
 * @brief Collision plane for ground collision
 */
struct RSCollisionPlane
{
	glm::vec3 point = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
};

/**
 * @brief Cloth namespace
 */
namespace RS_Cloth
{
	/**
	 * @brief Cloth Simulator class with Compute Shader support
	 * Implements Mass-Spring and PBD cloth simulation
	 */
	class RSClothSimulator
	{
	public:
		/**
		 * @brief Constructor. Set default values
		 */
		RSClothSimulator();
		~RSClothSimulator();

		//********************************************************************************
		// Core Functions
		//********************************************************************************

		/**
		 * @brief Initialize the cloth simulation
		 * Creates cloth mesh, springs, and GPU buffers
		 */
		void Initialize();

		/**
		 * @brief Shutdown and release all resources
		 */
		void Shutdown();

		/**
		 * @brief Update cloth simulation
		 * @param dt delta time
		 */
		void Update(float dt);

		/**
		 * @brief Forward rendering pass
		 */
		void ForwardDraw();

		/**
		 * @brief Deferred rendering pass
		 */
		void DeferredDraw();

		//********************************************************************************
		// Initialization Functions
		//********************************************************************************

		/**
		 * @brief Set cloth field (create particles in grid)
		 */
		void SetClothField();

		/**
		 * @brief Set cloth field with custom parameters
		 * @param width number of particles in width
		 * @param height number of particles in height
		 * @param spacing space between particles
		 */
		void SetClothField(int width, int height, float spacing);

		/**
		 * @brief Create all spring constraints
		 * Structural, Shear, and Bending springs
		 */
		void CreateSprings();

		/**
		 * @brief Create GPU buffers (SSBOs)
		 */
		void CreateClothBuffers();

		/**
		 * @brief Reset all buffers and data
		 */
		void ResetAllBuffers();

		/**
		 * @brief Deallocate cloth data
		 */
		void DeallocateCloth();

		//********************************************************************************
		// Simulation Functions
		//********************************************************************************

		/**
		 * @brief Compute cloth simulation step (Mass-Spring)
		 * @param dt delta time
		 */
		void ComputeMassSpring(float dt);

		/**
		 * @brief Compute cloth simulation step (PBD)
		 * @param dt delta time
		 */
		void ComputePBD(float dt);

		/**
		 * @brief Apply external force to all particles
		 * @param force force vector
		 */
		void ApplyExternalForce(const glm::vec3& force);

		/**
		 * @brief Apply wind force
		 * @param wind_direction wind direction
		 * @param wind_strength wind strength
		 */
		void ApplyWind(const glm::vec3& wind_direction, float wind_strength);

		/**
		 * @brief Handle collision with sphere
		 * @param sphere collision sphere
		 */
		void HandleSphereCollision(const RSCollisionSphere& sphere);

		/**
		 * @brief Handle collision with plane
		 * @param plane collision plane
		 */
		void HandlePlaneCollision(const RSCollisionPlane& plane);

		/**
		 * @brief Calculate normals for rendering
		 */
		void CalculateNormals();

		//********************************************************************************
		// Getters and Setters
		//********************************************************************************

		RS_N_D unsigned int GetParticleCount() const { return m_particle_count; }
		RS_N_D unsigned int GetSpringCount() const { return m_spring_count; }
		RS_N_D unsigned int GetTriangleCount() const { return m_triangle_count; }

		RS_N_D bool IsInitialized() const { return m_initialized; }
		RS_N_D bool IsPlaying() const { return b_playing; }
		void SetPlaying(bool playing) { b_playing = playing; }
		void SwitchPlaying() { b_playing = !b_playing; }

		RS_N_D RSClothSolverType GetSolverType() const { return m_solver_type; }
		void SetSolverType(RSClothSolverType type) { m_solver_type = type; }

		void SetClothProperty(const RSClothProperty& property) { m_cloth_property = property; }
		RS_N_D RSClothProperty GetClothProperty() const { return m_cloth_property; }

		void SetInitSetting(const RSClothInitSetting& setting) { m_init_setting = setting; }
		RS_N_D RSClothInitSetting GetInitSetting() const { return m_init_setting; }

		// Collision objects
		void SetCollisionSphere(const RSCollisionSphere& sphere) { m_collision_sphere = sphere; b_use_sphere_collision = true; }
		void SetCollisionPlane(const RSCollisionPlane& plane) { m_collision_plane = plane; b_use_plane_collision = true; }
		void EnableSphereCollision(bool enable) { b_use_sphere_collision = enable; }
		void EnablePlaneCollision(bool enable) { b_use_plane_collision = enable; }

		// Wind
		void SetWindDirection(const glm::vec3& dir) { m_wind_direction = glm::normalize(dir); }
		void SetWindStrength(float strength) { m_wind_strength = strength; }
		RS_N_D glm::vec3 GetWindDirection() const { return m_wind_direction; }
		RS_N_D float GetWindStrength() const { return m_wind_strength; }

		// GPU Buffer access
		RS_N_D GLuint GetPositionSSBO() const { return m_position_ssbo; }
		RS_N_D GLuint GetNormalSSBO() const { return m_normal_ssbo; }

		/**
		 * @brief Start simulation
		 */
		void StartSimulation();

		/**
		 * @brief Reset simulation to initial state
		 */
		void ResetSimulation();

	protected:
		//********************************************************************************
		// Internal Helper Functions
		//********************************************************************************

		/**
		 * @brief Get particle index from grid coordinates
		 */
		int GetParticleIndex(int x, int y) const;

		/**
		 * @brief Add spring constraint between two particles
		 */
		void AddSpring(int p1, int p2, RSSpringType type);

		/**
		 * @brief CPU fallback for Mass-Spring simulation
		 */
		void CPUSimulateMassSpring(float dt);

		/**
		 * @brief CPU fallback for PBD simulation
		 */
		void CPUSimulatePBD(float dt);

		/**
		 * @brief Transfer particle data to GPU
		 */
		void TransferParticleDataToGPU();

		/**
		 * @brief Transfer spring data to GPU
		 */
		void TransferSpringDataToGPU();

		/**
		 * @brief Create rendering resources (VAO, VBO, EBO)
		 */
		void CreateRenderResources();

		/**
		 * @brief Update index buffer for mesh rendering
		 */
		void CreateIndexBuffer();

	protected:
		//********************************************************************************
		// Cloth Data
		//********************************************************************************

		unsigned int m_particle_count = 0;      ///< Total particle count
		unsigned int m_spring_count = 0;        ///< Total spring count
		unsigned int m_triangle_count = 0;      ///< Triangle count for rendering

		int m_cloth_width = 0;                  ///< Particles in width
		int m_cloth_height = 0;                 ///< Particles in height

		std::vector<RSClothParticle> m_particles;  ///< Particle data (CPU)
		std::vector<RSClothSpring> m_springs;      ///< Spring data (CPU)
		std::vector<unsigned int> m_indices;       ///< Index buffer for rendering

		//********************************************************************************
		// GPU Buffers (SSBOs)
		//********************************************************************************

		GLuint m_position_ssbo = 0;       ///< Position buffer (binding 10)
		GLuint m_prev_position_ssbo = 0;  ///< Previous position buffer (binding 11)
		GLuint m_velocity_ssbo = 0;       ///< Velocity buffer (binding 12)
		GLuint m_spring_ssbo = 0;         ///< Spring constraints buffer (binding 13)
		GLuint m_normal_ssbo = 0;         ///< Normal buffer (binding 14)

		//********************************************************************************
		// Rendering Resources
		//********************************************************************************

		GLuint m_vao = 0;                 ///< Vertex Array Object
		GLuint m_vbo = 0;                 ///< Vertex Buffer Object
		GLuint m_ebo = 0;                 ///< Element Buffer Object (indices)

		//********************************************************************************
		// Simulation State
		//********************************************************************************

		bool m_initialized = false;       ///< Initialization flag
		bool b_playing = false;           ///< Playing flag
		float m_accumulated_time = 0.0f;  ///< Accumulated time for fixed timestep

		RSClothSolverType m_solver_type = RSClothSolverType::MASS_SPRING;
		RSClothProperty m_cloth_property;
		RSClothInitSetting m_init_setting;

		//********************************************************************************
		// Collision Objects
		//********************************************************************************

		RSCollisionSphere m_collision_sphere;
		RSCollisionPlane m_collision_plane;
		bool b_use_sphere_collision = true;
		bool b_use_plane_collision = true;

		//********************************************************************************
		// External Forces
		//********************************************************************************

		glm::vec3 m_wind_direction = glm::vec3(1.0f, 0.0f, 0.0f);
		float m_wind_strength = 0.0f;
		glm::vec3 m_external_force = glm::vec3(0.0f);

		//********************************************************************************
		// Flags
		//********************************************************************************

		bool b_property_changed = true;   ///< Property changed flag

	private:
		friend class RS_Handler::RSImguiHandler;  ///< Friend class for GUI access
	};

} // namespace RS_Cloth

#endif // !RS_CLOTH_SIMULATOR_H_
