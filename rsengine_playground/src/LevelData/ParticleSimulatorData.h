/******************************************************************************/
/*!
\file   ParticleSimulatorData.h
\author Jinseob Park
\date   2024/08/06

This file is the game data of Jordy Roll level.
*/
/******************************************************************************/
#ifndef PARTICLE_SIMULATOR_DATA_H_
#define PARTICLE_SIMULATOR_DATA_H_
#include <stack>

namespace SJG
{
	class ParticleSimulatorData
	{
	public:
		ParticleSimulatorData();
		~ParticleSimulatorData();

		void Initialize();
		void InitializePlayer();
		void Shutdown();

		void Update(float dt);

		void KeyUpdate(float dt);

		void SetInitSettingParticleSystem(const RSInitSettingParticleSystem& init_setting);
		void SetParticleProperty(const RSParticleProperty& property);

		RSInitSettingParticleSystem GetInitSettingParticleSystem() const { return m_particle_init_setting; }
		RSParticleProperty GetParticleProperty() const { return m_particle_property; }

	private:
		float m_player_speed = 5.0f;
		bool b_player_on_ground = true;

		bool is_auto_play = true;
		float auto_jump_time = 2.0f;

		glm::vec3 auto_player_direction = glm::vec3(1.0f, 0.0f, 0.0f);

		//RSResourceManager* m_resource_manager = nullptr;
		RS_Object::RSObject* m_player;

		RS_Widget::RSLegendWidget* m_legend_widget = nullptr;

		RSInitSettingParticleSystem m_particle_init_setting;
		RSParticleProperty m_particle_property;

		std::stack<RS_Object::RSObject*> m_object_list;

	};

#endif // !PARTICLE_SIMULATOR_DATA_H_
}
