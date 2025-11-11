/******************************************************************************/
/*!
\file   RSParticleManager.h
\author Jinseob Park
\date   2024/08/01

This file contains Particle Manager for Game Engine
Have Fun with Rock & Stone
*/
/******************************************************************************/
#ifndef RS_PARTICLE_MANAGER_H_
#define	RS_PARTICLE_MANAGER_H_

#include "../Particle/RSParticleSystem.h"
#include "../Particle/RSParticleSimulator.h"
#include "../Particle/VTKViewer/RSVTKViewer.h"
#include "../Particle/PointClipper/RSPointClipper.h"

/**
* @brief Particle namespace
*/
namespace RS_Particle
{
/**
* @brief Particle Manager
*/
class RSParticleManager
{
public:
	RSParticleManager();
	~RSParticleManager();

	void Initialize();
	void Shutdown();

	/**
	* @brief Update all particle systems
	* include the particle simulator
	* @param dt delta time
	*/
	void Update(float dt);

	/**
	* @brief Draw all particle systems
	* include the particle simulator
	*/
	void Draw();

	void DeferredDraw();

  void ForwardDraw();

	/**
	* @brief Create particle system with names
	* @param[in] name_ particle system name
	* @return void
	*/
	void CreateParticleSystem(const std::string& name_);

	/**
	* @brief Enroll particle system
	* The particle system should be created and initialized. and also have a name and count of particles
	* @param particle_system_ particle system pointer
	* @return void
	*/
	void EnrollParticleSystem(RSParticleSystem* particle_system_);

	/**
	* @brief Get Particle system by name
	* @param name_ Particle system name
	* @return The particle system pointer. If it is not exist, return nullptr
	*/
	RSParticleSystem* GetParticleSystemByName(const std::string& name_) const;

	/**
	* @brief Get the particle system simulator. It only one instance
	* @return The particle system simulator pointer. If it is not exist, return nullptr
	*/
	RS_N_D RSParticleSimulator* GetParticleSimulator() const { return m_particle_simulator; }

	/**
	* @brief Get the particle system is using SPH
	* @return use sph or not
	*/
	RS_N_D bool IsUseSph() const { return b_use_sph; }

	/**
	* @brief Set the particle system is using SPH
	* @param use_sph use sph or not
	* @return void
	*/
	void SetUseSph(const bool use_sph) { b_use_sph = use_sph; }


  /**
   * @brief Create particle simulator
   * @return true if it is created successfully
   */
  bool CreateParticleSimulator();


  /**
   * @brief Shutdown particle simulator
   * @return void
   */
  void ShutdownParticleSimulator();

  /**
   * @brief Check the particle simulator is existed
   * @return true if it is existed
   */
  bool IsExistParticleSimulator() const { return m_particle_simulator != nullptr; }


	//********************************************************************************
  //******************************* Point Clipper **********************************
  //********************************************************************************

  /**
   * @brief Create Point Clipper
   * @return true if it is created successfully
   */
  bool CreatePointClipper();

	
  /**
   * @brief Shutdown Point Clipper
   */
  void ShutdownPointClipper();


  /**
   * @brief Check the point clipper is existed
   * @return true if it is existed
	 */
	bool IsExistPointClipper() const { return m_point_clipper != nullptr; }


  /**
   * @brief Get the point clipper
   * @return The point clipper pointer. If it is not exist, return nullptr
   */
  RS_PointClipper::RSPointClipper* GetPointClipper() const { return m_point_clipper; }

  //********************************************************************************
  //******************************* VTK Viewer *************************************
  //********************************************************************************

  /**
   * @brief Create VTK Viewer
   */
  void CreateVtkViewer();

  /**
   * @brief Shutdown VTK Viewer
	 */
	void ShutdownVtkViewer();

  /**
   * @brief Check the VTK Viewer is existed
   * @return true if it is existed
   */
  bool IsExistVtkViewer() const { return m_vtk_viewer != nullptr; }


  /**
   * @brief Get the VTK Viewer
   * @return The VTK Viewer pointer. If it is not exist, return nullptr
   */
  RS_VTK::RSVTKViewer* GetVtkViewer() const { return m_vtk_viewer; }


private:
  std::list<RSParticleSystem*> m_particle_systems; ///< particle system list

  bool b_use_sph = false; ///< use sph or not
  RSParticleSimulator* m_particle_simulator = nullptr; ///< particle simulator
  RS_PointClipper::RSPointClipper* m_point_clipper = nullptr; ///< point clipper
  RS_VTK::RSVTKViewer* m_vtk_viewer = nullptr; ///< vtk viewer
  std::list< RS_VTK::RSVTKViewer*> m_vtk_viewer_list; ///< vtk viewer list
};
}

#endif // !RS_PARTICLE_MANAGER_H_