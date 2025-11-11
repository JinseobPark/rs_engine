/******************************************************************************/
/*!
\file   JordyRollData.h
\author Jinseob Park
\date   2024/08/06

This file is the game data of Jordy Roll level.
*/
/******************************************************************************/
#ifndef JORDY_ROLL_DATA_H_
#define JORDY_ROLL_DATA_H_

namespace SJG
{
	class JordyRollData
	{
	public:
		JordyRollData();
		~JordyRollData();

		void Initialize();
		void InitializePlayer();
		void Shutdown();

		void Update(float dt);

		void KeyUpdate(float dt);



	private:
		float m_player_speed = 5.0f;
		bool b_player_on_ground = true;

		bool is_auto_play = true;
		float auto_jump_time = 2.0f;

		glm::vec3 auto_player_direction = glm::vec3(1.0f, 0.0f, 0.0f);

		RS_Object::RSObject* m_player;
	};

#endif // !JORDY_ROLL_DATA_H_
}
