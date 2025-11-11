/******************************************************************************/
/*!
\file   RSParticleSimulator.h
\author Jinseob Park
\date   2024/08/01

This file contains particle simulator class for SPH. Single Particle System with Compute Shader

*/
/******************************************************************************/
#ifndef RS_PARTICLE_SIMULATOR_H_
#define RS_PARTICLE_SIMULATOR_H_

#define INFLOW_MAX_COUNT 1024

#include "glm/glm.hpp"
#include <GL/glew.h>

const std::string INITIAL_ZONE_OBJECT_NAME	= "RS_initial_zone";
const std::string BOUNDARY_BOX_OBJECT_NAME	= "RS_boundary_box";
const std::string INFLOW_SPHERE_OBJECT_NAME = "RS_inflow_sphere";
const std::string INFLOW_ZONE_OBJECT_NAME		= "RS_inflow_zone";
const std::string OUTFLOW_ZONE_OBJECT_NAME	= "RS_outflow_zone";

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

/**
 * @brief Particle Simulator for SPH MINT. Single Particle System with Compute RSShader
 * Not used yet
 */
struct RSParticleState
{
	bool b_velocity = true;
	bool b_density = true;
	bool b_pressure = true;
	bool b_temperature = false;
	bool b_torque = false;
	bool b_stress = false;
	bool b_vorticity = false;
	bool b_viscosity = false;
	bool b_strain = false;
};

/**
 * @brief Particle Rendering Type
 * Not used yet
 */
enum class RSParticleRenderingType : std::uint8_t
{
	R_NONE = 0,			// single color
	R_VELOCITY,			// vec3 velocity
	R_ACCELERATION,		// vec3 acceleration
	R_DENSITY,			// float density
	R_PRESSURE,			// float pressure
	R_TEMPERATURE,		// float temperature
	R_TORQUE,			// vec3 torque
	R_STRESS,			// vec3 stress
	R_VORTICITY,		// vec3 vorticity
	R_VISCOSITY,		// float viscosity
	R_STRAIN			// vec3 strain
};

/**
 * @brief Render Property Type
 */
enum class RSRenderPropertyType : std::uint8_t
{
	R_COLOR = 0,	// not use legend color
	//R_WATER,
	R_VELOCITY,		// Velocity size.		velocities.xyz
	R_ACCELERATION,	// Acceleration size	accelerations.xyz
	R_DENSITY,		// Density				positions.w
	R_PRESSURE 		// Pressure				velocities.w
};

/** 
 * @brief Particle struct.
 * Not used yet
 */
struct RSParticle
{
	glm::vec4 position;
	glm::vec4 velocity;
	glm::vec4 acceleration;
};


/**
 * @brief Solver Type
 */
enum class RSSolverType : int
{
	R_NONE = 0,
	R_STATIC_BRUTE_FORCE = 1,
	R_STATIC_GRID = 2,
	R_DYNAMIC_BRUTE_FORCE = 3,

	R_STATIC_KDTREE = 4,
	R_STATIC_OCTREE = 5,
	R_DYNAMIC_GRID = 6,
	R_DYNAMIC_KDTREE = 7,
	R_DYNAMIC_OCTREE = 8,

  R_TRIPLE_BUFFERING_TEST = 9
};

/**
 * @brief Clipping Type
 */
enum class RSClippingType : int
{
	NONE = 0,
	PLANE = 1,
	CUBE = 2,
	SPHERE = 3
};

/**
 * @brief Clipping Area Inside or Outside
 */
enum class RSClippingArea: int
{
	INSIDE = 0,
	OUTSIDE = 1
};

/**
 * @brief Initial Setting struct for Particle System
 * It contains initial particle setting, boundary, grid resolution, grid max count per max, and space between particles
 */
struct RSInitSettingParticleSystem
{
	glm::vec3 init_particle_setting_min = glm::vec3(-10.0f, 2.0f, -10.0f);
	glm::vec3 init_particle_setting_max = glm::vec3(10.0f, 2.31f, 10.0f);
	glm::vec3 boundary_min = glm::vec3(-10, -30, -10);
	glm::vec3 boundary_max = glm::vec3(10, 30, 10);

	glm::ivec3 init_grid_resolution = glm::vec3(10, 10, 10);

	float grid_max_count_per_max = 0.05f; // * 100 percent

	float init_particle_setting_space = 0.15f;

  // Dynamic Flow
	unsigned int m_inflow_count = 0; // Not control by user.

  // User setting value by GUI.
	unsigned int m_inflow_max_count = 50000;
	float m_inflow_speed = 1.0f;
	glm::vec3 m_inflow_direction = glm::vec3(0, -1, 0);

	// User setting value by object.
	float m_inflow_radius = 2.0f; // Sphere object's x scale.
	glm::vec3 m_inflow_position = glm::vec3(-2.0f, 2.0f, 0.0f);
	glm::vec3 m_outflow_position = glm::vec3(2.0f, 2.0f, 0.0f);
	glm::vec3 m_inflow_zone_size = glm::vec3(1.0f);
	glm::vec3 m_outflow_zone_size = glm::vec3(1.0f);

	//glm::vec3 m_inflow_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
};

/**
 * @brief Particle Property struct for SPH calculation
 * * It contains particle radius, collision stiffness, density, bulk, viscosity, gravity, mass, gas constant, smoothing length
 */
struct RSParticleProperty
{
	float particle_radius = 0.1f;
	float collision_stiffness = 100.0f;

	float density = 1000.0f;
	float bulk = 1000.0f;
	float viscosity = 0.1f;
	float gravity = 9.8f;
	float mass = 1.0f;
	float gas_constant = 2000.0f;
	float smoothing_length = 0.1f;
};

struct RSInflowData
{
  std::array<glm::vec4, INFLOW_MAX_COUNT> m_inflow_positions = { glm::vec4(0.0f) };
  std::array<glm::uvec4, INFLOW_MAX_COUNT> m_inflow_using = { glm::uvec4(0) };
	unsigned int current_count = 0;
	unsigned int max_particle_count = 0;
	unsigned int inflow_creator_size = 0;
	unsigned int padding = 0;
};

/**
 * @brief Particle namespace
 */
namespace RS_Particle
{
	/**
	 * @brief Single Particle System with Compute RSShader
	 */
	class RSParticleSimulator
	{
	public:
		/**
		 * @brief Constructor. Set default values
		 */
		RSParticleSimulator();
		~RSParticleSimulator();

		/**
		 * @brief Initialize the particle system
		 * Set Particle Field, Create Particle Buffer, Create Particle, Create ParticleBuffer
		 * @see SetParticleField
		 * @see CreateParticleBuffer
		 * @see CreateParticle
		 * @see CreateParticleBuffer
		 */
		void Initialize();

		/**
		 * @brief Deallocate ssbo and buffers
		 * Deallocate Particle data
		 * @see ResetAllBuffers
		 * @see DeallocateParticle
		 */
		void Shutdown();

		/**
		 * @brief Update the particle system
		 * If any calculation property changed, update all property to target buffer
		 * If call calculate min-max value, calculate min-max value
		 * 
		 * @see SetTextShouldUpdate
		 * @see CalculateMinMaxValue
		 * 
		 * @param dt delta time
		 */
		void Update(float dt);

		/**
		 * @brief Draw the particle system
		 * If any render property changed, update all property to target buffer
		 * Draw the particle system
		 */
		void ForwardDraw();

		void DeferredDraw();


		//********************************************************************************
		//******************************** Initialize Part *******************************
		//********************************************************************************

		/**
		 * @brief Set init particle field.
		 * This function will calculate particle count and particle mass by min, max, space
		 */
		void SetParticleField();
		/**
		 * @brief Set init particle field.
		 * This function will calculate particle count and particle mass by min, max, space
		 * BUT, not main function. Just for testing
		 * @param min particle space min
		 * @param max particle space max
		 * @param space particle space
		 */
		void SetParticleField(glm::vec3 min, glm::vec3 max, float space);

		/**
		 * @brief Resize and init particle data vectors by particle count
		 */
		void ResizeParticleData();

		/**
		 * @brief Create particle buffer
		 * Create ssbo for position, velocity, acceleration, grid, grid count
		 */
		void CreateParticleBuffer();

		/**
		 * @brief Fill particle data vectors with initial static values
		 */
		void CreateStaticParticle();


    /**
     * @brief Fill particle data vectors with initial dynamic values
     */
		void CreateDynamicParticle();

		/**
		 * @brief Delete all buffers and ssbos
		 */
		void ResetAllBuffers();

		/**
		 * @brief Get particle count
		 * @return particle count
		 */
		RS_N_D unsigned int GetParticleCount() const { return m_particle_count; }

		/**
		 * @brief Set particle count
		 * @param particle_count particle count
		 */
		void SetParticleCount(unsigned int particle_count);

		/**
		 * @brief Set particle state
		 * Not used yet
		 * @param particle_state particle state
		 */
		void SetParticleState(const RSParticleState particle_state) { m_particle_state = particle_state; }

		/**
		 * @brief Get particle state
		 * Not used yet
		 * @return particle state
		 */
		RS_N_D RSParticleState GetParticleState() const { return m_particle_state; }

		/**
		 * @brief Clear all particle data vectors
		 */
		void DeallocateParticle();

		/**
		 * @brief Check if the particle system is initialized
		 * Not used yet
		 * @return true if the particle system is initialized
		 */
		RS_N_D bool IsInitialized() const { return m_initialized; }

		/**
		 * @brief Check if the particle count is valid
		 * @return true if the particle count is valid
		 */
		RS_N_D bool IsValidParticleCount() const { return m_valid_particle_count; }

		/**
		 * @brief Check if the particle system is playing
		 * @return true if the particle system is playing
		 */
		bool IsPlaying() const { return b_playing; }

		/**
		 * @brief Set playing
		 * @param playing playing
		 */
		void SetPlaying(const bool playing) { b_playing = playing; }


    /**
		 * @brief Start the simulation. 
		 * This function include sync data with objects.
		 */
		void StartSimulation();

		/**
		 * @brief Switch playing
		 */
		void SwitchPlaying() { b_playing = !b_playing; }

		/**
		 * @brief Set init setting particle system
		 * @param init_setting_ init setting particle system
		 */
		void SetInitSettingParticleSystem(const RSInitSettingParticleSystem& init_setting_);

		/**
		 * @brief Get init setting particle system
		 * @return init setting particle system
		 */
		RS_N_D RSInitSettingParticleSystem GetInitSettingParticleSystem() const { return m_init_setting; }


		/**
		 * @brief Get Solver Type
		 * @return solver type
		 */
		RS_N_D RSSolverType GetSolverType() const { return m_solver_type; }

		/**
		 * @brief Set Solver Type. 
		 *	When Solver type is changed, the level should be reinitialized.
     *  This function include that the current level restart.
		 * @param solver_type_ solver type
		 */
		void SetSolverType(const RSSolverType solver_type_);


    /**
     * @brief Set Solver Type before the level initialized.
     * @param solver_type_ solver type
     */
    void InitSetSolverType(const RSSolverType solver_type_);


    /**
		 * @brief Set hide objects when start solver
		 */
		void SetHideObjectsStartToSolve() const;

    /**
		 * @brief Sync Data From Objects
		 * 
		 */
		void SyncDataFromObjects();

		//////////////////////
		// Static Flow Part //
		//////////////////////

		void CreateStaticFlowObjects() const;

		///////////////
		// Grid Part //
		///////////////
		 
		/**
		 * @brief Create Grid Buffer when solver use grid.
		 */
    void CreateGridBuffer();


		///////////////////////
		// Dynamic Flow Part //
		///////////////////////
		
    /**
     * @brief Create Inflow Buffer when solver use dynamic flow.
     */
		void CreateInFlowBuffer();


    /**
     * @brief Create Dynamic Flow Objects
		 */
		void SetDynamicParticleField();


    /**
     * @brief Set Inflow Grid By Direction
     */
    void SetInflowGridByDirection();


    /**
     * @brief Create Inflow Objects
     */
    void CreateInOutFlowObjects() const;


    /**
		 * @brief Remove tool objects
     * It includes inflow, outflow, initial zone, boundary box
		 */
		void RemoveToolObjects();


    /**
     * @brief Get inflow data
     * @return inflow data
     */
    const RSInflowData* GetInflowData() const { return &m_inflow_data; }


		 



		//***************************************************************************
		//******************************** Calculate Part ***************************
		//***************************************************************************

		/////////////////////////////
		// Static Brute Force Part //
		/////////////////////////////

    /**
     * @brief Compute static brute force solver.
     * @param dt delta time
     */
    void ComputeStaticBruteForce(float dt);


    /**
     * @brief Compute static brute force solver status.
     */
    void ComputeStaticBruteForce_Density() const;


    /**
     * @brief Compute static brute force solver force.
     * @param dt delta time
     */
    void ComputeStaticBruteForce_Force(float dt);


    /**
     * @brief Compute static brute force solver post.
     * @param dt delta time
     */
    void ComputeStaticBruteForce_Post(float dt);



    //////////////////////
		// Static Grid Part //
		//////////////////////

    /**
     * @brief Compute static grid solver.
     * @param dt delta time.
     */
    void ComputeStaticGrid(float dt);


    /**
     * @brief Initialize static grid.
     */
    void InitializeStaticGrid() const;


		
    /**
     * @brief Compute static grid batch.
     */
		void ComputeStaticGrid_Batch() const;


    /**
     * @brief Compute static grid status.
     */
    void ComputeStaticGrid_Density() const;


    /**
     * @brief Compute static grid force.
     */
    void ComputeStaticGrid_Force();


    /**
     * @brief Compute static grid post.
     * @param dt delta time.
     */
    void ComputeStaticGrid_Post(float dt);

		////////////////////////////////
		// Triple Buffering Test Part //
    ////////////////////////////////

    /**
     * @brief Compute triple buffering test solver.
     * @param dt delta time.
     */
    void ComputeTripleBufferingTest(float dt);

    /**
     * @brief Initialize triple buffering test.
     */
    void InitializeTripleBufferingTest();


		//////////////////////////////
		// Dynamic Brute Force Part //
		//////////////////////////////

		void UpdateInOutInform();


    /**
     * @brief Compute dynamic brute force solver.
     * @param dt delta time.
     */
		void ComputeDynamicBruteForce(float dt);


    /**
     * @brief Compute dynamic brute force solver status.
     */
    void ComputeDynamicBruteForce_Status();


    /**
     * @brief Compute dynamic brute force solver density.
     */
    void ComputeDynamicBruteForce_Density() const;


    /**
     * @brief Compute dynamic brute force solver force.
     */
    void ComputeDynamicBruteForce_Force();


    /**
     * @brief Compute dynamic brute force solver post.
     * @param dt delta time.
     */
    void ComputeDynamicBruteForce_Post(float dt);


    /**
     * @brief Update particle count.
		 */
		void UpdateInflowParticleCount();

		/////////////////
		// Common Part //
		/////////////////
		
		/**
		 * @brief Transfer mesh collision data to mesh collision ssbo.
		 */
		void TransferMeshCollisionData();

		/**
		 * @brief Add mesh data to collision by mesh data with transform matrix
		 * @param mesh_data_ mesh data pointer to add collision check
		 * @param transform_ transform of the mesh data
		 * @param property_ property of the mesh data
		 */
		void AddMeshDataToCollision(const RSMeshData* mesh_data_, RS_Component::RSTransform& transform_, int property_ = 0);

		/**
		 * @brief Add mesh data to collision by object
		 * @param object_ object pointer to add collision check
		 * @param property_ property of the mesh data
		 */
		void AddMeshDataToCollision(RS_Object::RSObject* object_, int property_ = 0);

		// Not used yet. Will be use later
		//void UpdateMeshDataToCollision(RSMeshData* mesh_data_, RS_Component::RSTransform transform_ = RS_Component::RSTransform(), int property_ = 0);

		/**
		 * @brief Update All property. 
		 * All property will be updated to target buffer
		 * @see UpdateAllProperty
		 * @see UpdateInitProperty
		 * @see UpdateCalculateProperty`
		 */
		void UpdateAllProperty();
		/**
		 * @brief Update Init property.
		 * When boundary condition, player circle radius are changed, call this function
		 */
		void UpdateInitProperty();

		/**
		 * @brief Update Calculate property.
		 * When any calculate property changed, call this function
		 * particle radius, collision stiffness, density, bulk, viscosity, gravity, mass, gas constant, smoothing length
		 */
		void UpdateCalculateProperty();

		/**
		 * @brief Set any calculate property changed
		 */
		void SetAnyCalculatePropertyChanged() { b_any_calculate_property_changed = true; }
		/**
		 * @brief Set init boundary condition changed
		 */
		void SetInitBoundaryConditionChanged() { b_init_boundary_condition_changed = true; }
		/**
		 * @brief Set any collision data changed
		 */
		void SetAnyCollisionDataChanged() { b_any_collision_data_changed = true; }

		/**
		 * @brief Set Text should be updated.
		 * When any property changed, text should be updated for property text rendering
		 * @param update_ text should be updated
		 */
		void SetTextShouldUpdate(const bool update_) { b_text_should_update = update_; }

		/**
		 * @brief Get Text should be updated
		 * @return text should be updated or not
		 */
		RS_N_D bool GetTextShouldUpdate() const { return b_text_should_update; }

		/**
		 * @brief Calculate minmax value.
		 * This function will calculate min and max from ssbo data
		 * This function is expensive. Call this function only when you need to update minmax value
		 */
		void CalculateMinMaxValue();

		/**
		 * @brief Call calculate minmax value.
		 * @see CalculateMinMaxValue
		 */
		void CallCalculateMinMaxValue();

		/**
		 * @brief Set particle property
		 * @param particle_property_ particle property
		 */
		void SetParticleProperty(const RSParticleProperty& particle_property_) { m_particle_property = particle_property_; }

		/**
		 * @brief Get particle property
		 * @return particle property
		 */
		RS_N_D RSParticleProperty GetParticleProperty() const { return m_particle_property; }

    void SetInflowDirectionUpdated() { b_inflow_direction_changed = true; }


    /**
     * @brief Set Inflow Position
     * @param direction_ inflow position
     */
    void SetInflowDirection(const glm::vec3& direction_);

    /**
     * @brief Update Inflow Direction(transform)
     */
    void UpdateInflowDirection();

    /**
		 * @brief Get all ssbo data from gpu.
		 */
		void GetSsboDataFromGPU();

		//********************************************************************************
		//******************************** Render Part ***********************************
		//********************************************************************************

		/**
		 * @brief Get Point size for point rendering
		 * @return point size (float)
		 */
		RS_N_D float GetPointSize() const;

		/**
		 * @brief Set Point size for point rendering
		 * @param point_size point size (float)
		 */
		void SetPointSize(float point_size);

		/**
		 * @brief Set Particle Rendering Type
		 * This Render type is not used yet
		 * @param particle_rendering_type 
		 */
		void SetParticleRenderingType(RSParticleRenderingType particle_rendering_type);

		/**
		 * @brief Get Particle Rendering Type
		 * This Render type is not used yet
		 * @return particle rendering type
		 */
		RS_N_D RSParticleRenderingType GetParticleRenderingType() const;

		/**
		 * @brief Get Clipping Type by float
		 * @return clipping type (float)
		 */
		float GetClippingTypeF();

		/**
		 * @brief Get Clipping Type by int
		 * @return clipping type (int)
		 */
		int GetClippingTypeI();

		/**
		 * @brief Set Clipping Type
		 * @param clipping_type_ clipping type
		 */
		void SetClippingType(int clipping_type_);

		/**
		 * @brief Get Clipping Area by float  FOR SHADER
		 * @return 1.0f : inside, -1.0f : outside
		 */
		float GetClippingAreaF();

		/**
		 * @brief Get Clipping Area by int FOR IMGUI HANDLER
		 * @return 0 : inside, 1 : outside
		 */
		int GetClippingAreaI();

		/**
		 * @brief Set Clipping Area inside or outside 
		 * @param clipping_area_ clipping area. 0 : inside, 1 : outside
		 */
		void SetClippingArea(int clipping_area_);

		/**
		 * @brief Get divided value for legend
		 * @return divided value
		 */
		RS_N_D unsigned int GetDividedValue() const;

		/**
		 * @brief Set Divided value for legend
		 * @param divided_value divided value
		 */
		void SetDividedValue(unsigned int divided_value);

		/**
		 * @brief Set Using Legend
		 * @param use_legend_ using legend
		 */
		void SetUsingLegend(bool use_legend_);

		/**
		 * @brief Get Using Legend
		 * @return using legend
		 */
		RS_N_D bool GetUsingLegend() const;

		/**
		 * @brief Set Using Texture
		 * @param use_texture_ using texture
		 */
		void SetUsingTexture(bool use_texture_);

		/**
		 * @brief Get Using Texture
		 * @return using texture
		 */
		RS_N_D bool GetUsingTexture() const;

		/**
		 * @brief Set any render property changed
		 */
		void UpdateRenderProperty();

		/**
		 * @brief Set any render property changed
		 */
		void SetAnyRenderPropertyChanged() { b_any_render_property_changed = true; }

		/**
		 * @brief Set Next Render Property Type. 
		 * This function is temporary. Will be removed later
		 */
		void SetNextRenderPropertyType();

		/**
		 * @brief Set Render Property Type
		 * @param render_property_type_ render property type
		 */
		void SetRenderPropertyType(RSRenderPropertyType render_property_type_);
		
		/**
		 * @brief Get Render Property Type
		 * @return render property type
		 */
		RS_N_D RSRenderPropertyType GetRenderPropertyType() const;

		/**
		 * @brief Get Render Property Type by int
		 * @return render property type (int)
		 */
		RS_N_D int GetRenderPropertyTypeI() const;

		/**
		 * @brief Set Min value legend for legend color
		 * @param value_ min value of legend
		 */
		void SetMinValueLegend(float value_);

		/**
		 * @brief Get Min value legend for legend color
		 * @return min value of legend
		 */
		RS_N_D float GetMinValueLegend() const;

		/**
		 * @brief Set Max value legend for legend color
		 * @param value_ max value of legend
		 */
		void SetMaxValueLegend(float value_);
		
		/**
		 * @brief Get Max value legend for legend color
		 * @return max value of legend
		 */
		RS_N_D float GetMaxValueLegend() const;


    /**
     * @brief Get legend texture id
     * @return legend texture id
     */
    unsigned int GetLegendTextureId() const { return m_target_legend_texture; }

    /**
     * @brief Set legend texture id
     * @param texture_ texture id
     */
    void SetLegendTextureId(unsigned int texture_);

	protected:
		void ActivateProperties();


	protected:
//*****************************************************************************
//******************************** Initialize Part ****************************
//*****************************************************************************
		const unsigned int m_max_particle_count = UINT_MAX; ///< Max particle count (unsigned int max)
		unsigned int m_particle_count = 0; ///< Particle count
		unsigned int m_current_particle_count = 0; ///< Current particle count

		float m_point_size = 1.0f; ///< Point size for point rendering
		bool m_initialized = false; ///< Initialized or not
		bool m_valid_particle_count = false; ///< Valid particle count or not

		RSParticleState m_particle_state; ///< Particle state. Not used yet
		RSParticleRenderingType m_particle_rendering_type = RSParticleRenderingType::R_NONE; ///< Particle rendering type. Not used yet

		std::vector<glm::vec4> m_positions; ///< Particle positions. xyz : position, w : density
		std::vector<glm::vec4> m_velocities; ///< Particle velocities. xyz : velocity, w : pressure
		std::vector<glm::vec4> m_accelerations; ///< Particle accelerations. xyz : acceleration, w : activated. 0 : not activated, 1 : activated

		std::vector<int> m_grid_debug; ///< Grid debug. Use for later optimization
		std::vector<int> m_grid_count_debug; ///< Grid count debug. Use for later optimization

		std::vector<glm::vec4> m_mesh_vertices; ///< xyz : vertex, w : property.
		unsigned int m_mesh_triangle_count = 0; ///< Mesh triangle count

		std::vector<unsigned int> instanceIDs; ///< Instance IDs

		unsigned int m_current_buffer = 0; ///< Current buffer.  Use for later optimization


		// SSBO
		GLuint m_position_ssbo = 0; ///< Position and density ssbo
		GLuint m_velocity_ssbo = 0; ///< Velocity and pressure ssbo
		GLuint m_acceleration_ssbo = 0; ///< Acceleration ssbo

		// GRID
		GLuint m_grid_ssbo = 0; ///< Grid ssbo
		GLuint m_grid_count_ssbo = 0; ///< Grid count ssbo

    GLuint m_inflow_ssbo = 0; ///< Inflow ssbo

    // Mesh Collision
		GLuint m_mesh_collision_ssbo = 0; ///< Mesh collision ssbo

		unsigned int m_max_grid_count = 0; ///< Max grid count
    unsigned int m_grid_count = 0; ///< Grid count
    glm::vec3 m_grid_size = glm::vec3(0.0f); ///< Grid size


		// Not used yet
		GLuint m_density_ssbo = 0;
		GLuint m_pressure_ssbo = 0;
		GLuint m_temperature_ssbo = 0;
		GLuint m_torque_ssbo = 0;
		GLuint m_stress_ssbo = 0;
		GLuint m_vorticity_ssbo = 0;
		GLuint m_viscosity_ssbo = 0;
		GLuint m_strain_ssbo = 0;

    unsigned int m_vao = 0; ///< Vertex Array Object
    unsigned int m_instancing_vbo = 0; ///< Instancing Vertex Buffer Object
    unsigned int m_instance_position_vbo = 0, m_instance_value_vbo = 0; ///< Instance Position and Value Vertex Buffer Object

    //***********************************************************************//
    //************************* Dynamic Flow ********************************//
    //***********************************************************************//

    // For Dynamic Flow (Inflow, Outflow)
    //std::array<bool, INFLOW_MAX_COUNT> m_inflow_active; // max particle count is 1024
    //std::array<glm::vec3, INFLOW_MAX_COUNT> m_inflow_positions;
    RSInflowData m_inflow_data; ///< Inflow data

  //  float m_inflow_radius = 2.0f;
  //  unsigned int m_inflow_max_count = 50000;
  //  unsigned int m_inflow_count = 0;
  //  glm::vec3 m_inflow_position = glm::vec3(0.0f, 0.0f, 0.0f);
		//glm::vec3 m_inflow_direction = glm::vec3(0, -1, 0);
		//float m_inflow_speed = 1.0f;
  //  //glm::vec3 m_inflow_velocity = glm::vec3(0.0f, 0.0f, 0.0f);


  //  glm::vec3 m_outflow_position = glm::vec3(0.0f, 0.0f, 0.0f);
  //  glm::vec3 m_inflow_zone_size = glm::vec3(1.0f);
  //  glm::vec3 m_outflow_zone_size = glm::vec3(1.0f);

    bool b_inflow_radius_changed = true; ///< Inflow radius changed or not
    bool b_inflow_position_changed = true; ///< Inflow position changed or not
    bool b_inflow_speed_changed = true; ///< Inflow speed changed or not
    bool b_outflow_position_changed = true; ///< Outflow position changed or not

    bool b_zone_changed = true; ///< Zone changed or not

    glm::mat4 m_inflow_transform = glm::mat4(1.0f); ///< Inflow transform
    bool b_inflow_direction_changed = true; ///< Inflow direction changed or not

		//********************************************************************************
		//******************************** Calculate Part ********************************
		//********************************************************************************
    bool b_any_calculate_property_changed = true; ///< Any calculate property changed or not
    bool b_init_boundary_condition_changed = true; ///< Init boundary condition changed or not
    bool b_any_collision_data_changed = true; ///< Any collision data changed or not

    bool b_text_should_update = true; ///< Text should be updated or not

    bool b_call_calculate_minmax = false; ///< Call calculate minmax or not
    float value_min_legend = FLT_MAX; ///< Min value legend for legend color
    float value_max_legend = FLT_MIN; ///< Max value legend for legend color

    bool b_playing = true; ///< Playing or not

		// A cube for initialization the particles
    glm::vec3 m_cube_min = glm::vec3(-50, 10, -50); ///< Cube min for initialization the particles
    glm::vec3 m_cube_max = glm::vec3(50, 11, 50); ///< Cube max for initialization the particles
    float m_space = 0.3f; ///< Space for initialization the particles


    RSInitSettingParticleSystem m_init_setting; ///< Init setting particle system
    RSParticleProperty m_particle_property; ///< Particle property
    RSSolverType m_solver_type = RSSolverType::R_STATIC_BRUTE_FORCE; ///< Solver type

    RS_PipelineList::RSComputeShaderNames m_compute_shader_name; ///< Compute shader name

		//********************************************************************************
		//******************************** Render Part ***********************************
		//********************************************************************************

    RSClippingType m_clipping_type = RSClippingType::NONE; ///< Clipping type
    RSClippingArea m_clipping_area = RSClippingArea::INSIDE; ///< Clipping area

    glm::vec3 m_clipping_PMP = glm::vec3(0.0f, 0.0f, 0.0f); ///< Clipping PMP
    glm::vec3 m_clipping_VMR = glm::vec3(0.0f, 0.0f, 0.0f); ///< Clipping VMR

    bool b_any_render_property_changed = true; ///< Any render property changed or not
    unsigned int m_divided_value = 0; ///< Divided value for legend
    bool b_use_legend = false; ///< Using legend or not
    bool b_use_texture = false; ///< Using texture or not
    RSRenderPropertyType m_render_property_type = RSRenderPropertyType::R_COLOR; ///< Render property type

    GLuint m_target_legend_texture = 0; ///< Target legend texture

	private:
    friend class RS_Handler::RSImguiHandler; ///< Friend class RSImguiHandler
	};
}

#endif // !RS_PARTICLE_SIMULATOR_H_