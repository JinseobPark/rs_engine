#include "pch.h"
#include "RSParticleManager.h"	

namespace RS_Particle
{
	RSParticleManager::RSParticleManager()
	{
		m_particle_simulator = nullptr;
		m_cloth_simulator = nullptr;
	    m_point_clipper = nullptr;
	    m_vtk_viewer = nullptr;
	}

	RSParticleManager::~RSParticleManager()
	{
	    delete m_particle_simulator;
	    delete m_cloth_simulator;
	    delete m_point_clipper;
	    delete m_vtk_viewer;
	}

	void RSParticleManager::Initialize()
	{

	}

	void RSParticleManager::Shutdown()
	{
		ShutdownParticleSimulator();
		ShutdownClothSimulator();
		ShutdownPointClipper();
		ShutdownVtkViewer();
	}

	void RSParticleManager::Update(const float dt)
	{
    if (b_use_sph && m_particle_simulator)
      m_particle_simulator->Update(dt);

    if (b_use_cloth && m_cloth_simulator)
      m_cloth_simulator->Update(dt);

    if (m_point_clipper)
      m_point_clipper->Update(dt);

    if (m_vtk_viewer)
      m_vtk_viewer->Update(dt);

	}

	void RSParticleManager::Draw()
	{
		//if (b_use_sph && m_particle_simulator)
		//	m_particle_simulator->Draw();

		if (m_point_clipper)
			m_point_clipper->Draw();

	    if (m_vtk_viewer)
	      m_vtk_viewer->Draw();
	}

	void RSParticleManager::DeferredDraw()
	{
		// SPH Particle Rendering (Point rendering)
		if (b_use_sph && m_particle_simulator)
			m_particle_simulator->DeferredDraw();

		if (m_point_clipper)
			m_point_clipper->Draw();

		if (m_vtk_viewer)
			m_vtk_viewer->Draw(RSDataRenderType::DEFERRED);
	}

	void RSParticleManager::ForwardDraw()
	{
		// SPH Particle Rendering (Point rendering)
		if (b_use_sph && m_particle_simulator)
			m_particle_simulator->ForwardDraw();

		if (m_point_clipper)
			m_point_clipper->Draw();

		if (m_vtk_viewer)
			m_vtk_viewer->Draw(RSDataRenderType::FORWARD);
	}

	void RSParticleManager::DeferredDrawCloth()
	{
		// Cloth Mesh Rendering (Triangle mesh - GL_FILL mode)
		if (b_use_cloth && m_cloth_simulator)
			m_cloth_simulator->DeferredDraw();
	}

	void RSParticleManager::ForwardDrawCloth()
	{
		// Cloth Mesh Rendering (Triangle mesh - GL_FILL mode)
		if (b_use_cloth && m_cloth_simulator)
			m_cloth_simulator->ForwardDraw();
	}

	void RSParticleManager::CreateParticleSystem(const std::string& name_)
	{
		// check if the name is already exist
		for (const auto& particle_system : m_particle_systems)
		{
			if (particle_system->GetName() == name_)
			{
				RS_WARN("Particle System with name %s already exist", name_.c_str());
				return;
			}
		}

		const auto particle_system = new RSParticleSystem();
		particle_system->SetName(name_);
		m_particle_systems.push_back(particle_system);
	}

	void RSParticleManager::EnrollParticleSystem(RSParticleSystem* particle_system_)
	{
		if (!particle_system_) 
			return;
		if (particle_system_->GetName().empty()) 
			return;
		if (particle_system_->GetParticleCount() == 0)
			return;

		m_particle_systems.push_back(particle_system_);
	}

	RSParticleSystem* RSParticleManager::GetParticleSystemByName(const std::string& name_) const
	{
		for (auto& particle_system : m_particle_systems)
		{
			if (particle_system->GetName() == name_)
			{
				return particle_system;
			}
		}
		RS_WARN("Particle System with name %s not found", name_.c_str());
		return nullptr;
	}

  bool RSParticleManager::CreateParticleSimulator()
  {
    if (m_particle_simulator)
    {
      RS_WARN("Particle Simulator already exist");
      return false;
    }

    m_particle_simulator = new RSParticleSimulator();
    return true;
  }

  void RSParticleManager::ShutdownParticleSimulator()
  {
    if (m_particle_simulator)
    {
			m_particle_simulator->Shutdown();
      delete m_particle_simulator;
      m_particle_simulator = nullptr;
    }
  }

	bool RSParticleManager::CreateClothSimulator()
	{
		if (m_cloth_simulator)
		{
		RS_WARN("Cloth Simulator already exist");
		return false;
		}

		m_cloth_simulator = new RS_Cloth::RSClothSimulator();
		return true;
	}

	void RSParticleManager::ShutdownClothSimulator()
	{
    if (m_cloth_simulator)
    {
      m_cloth_simulator->Shutdown();
      delete m_cloth_simulator;
      m_cloth_simulator = nullptr;
    }
	}

	void RSParticleManager::CreateVtkViewer()
	{
    if (m_vtk_viewer)
    {
      RS_WARN("VTK Viewer already exist");
      return;
    }

    m_vtk_viewer = new RS_VTK::RSVTKViewer();
	}


	void RSParticleManager::ShutdownVtkViewer()
	{
    if (m_vtk_viewer)
    {
      m_vtk_viewer->ShutDown();
      delete m_vtk_viewer;
      m_vtk_viewer = nullptr;
    }
	}


	bool RSParticleManager::CreatePointClipper()
	{
    if (m_point_clipper)
    {
      RS_WARN("Point Clipper already exist");
      return false;
    }
    m_point_clipper = new RS_PointClipper::RSPointClipper();
    return true;
	}

	void RSParticleManager::ShutdownPointClipper()
	{
    if (m_point_clipper)
    {
      m_point_clipper->ShutDown();
      delete m_point_clipper;
      m_point_clipper = nullptr;
    }
	}


}