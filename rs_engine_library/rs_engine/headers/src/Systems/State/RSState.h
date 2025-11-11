/******************************************************************************/
/*!
\file   RSState.h
\author Jinseob Park
\date   2024/08/01

This file contains the declaration of the class RSState

*/
/******************************************************************************/
#ifndef RSSTATE_H
#define RSSTATE_H

#include "Systems/RSSystemComponent.h"


/**
 * @brief Function pointer for the state.
 */
typedef void(*RSFunc)(void);

/**
 * @brief Function pointer for the state.
 */
typedef void(*RSUpdateFunc)(float dt);

/**
 * @brief Statement struct for the state.
 */
struct RSStatement
{
	RSFunc       Load;    
	RSFunc       Init;    
	RSUpdateFunc Update;  
	RSFunc       Shutdown;
	RSFunc       Unload;  
};


/**
 * @brief State namespace
 */
namespace RS_State
{
	/**
	 * @brief State class
	 * This class derived from RSSystemComponent class.
	 */
	class RSState : public RSSystemComponent
	{
	public:
		/**
		 * @brief Constructor
		 * Set the current state to nullptr.
		 */
		RSState();


		/**
		 * @brief Copy Constructor
		 */
		RSState(const RSState&) = delete;

		/**
		* @brief Copy Assignment
		* @return RSState&
		*/
		RSState& operator=(const RSState&) = delete;

		/**
		 * @brief Destructor
		 */
		~RSState() override;

		
		/**
		 * @brief Initialize the current state. Don't touch this function.
		 */
		void Initialize() override;


		/**
		 * @brief Post Initialize the current state. Don't touch this function.
		 */
		void PostInitialize() override;


		/**
		 * @brief Shutdown the current state. Don't touch this function.
		 */
		void Shutdown() override;

		/**
		 * @brief Update the current state. Don't touch this function.
		 * @param dt Delta Time
		 */
		void Update(float dt) override;


		/**
		 * @brief Restart the current state.
		 * Include Shutdown and Initialize functions of the current state.
		 */
		void Restart() const;

		/**
		 * @brief Add the state to the state map.
		 * @param state_id_ State ID (Enum)
		 * @param state_ State struct. It should contain Load, Init, Update, Shutdown, Unload functions.
		 */
		void AddState(unsigned int state_id_, const RSStatement& state_);

		
		/**
		 * @brief Set the next state to the state with the given ID.
		 * Include Shutdown and Unload functions of the current state.
		 * Include Load and Init functions of the next state.
		 * @param state_id_ State ID (Enum)
		 */
		void SetNextState(unsigned int state_id_);


		/**
		 * @brief Set the start state to the state with the given ID.
		 * Include Load and Init functions of the start state.
		 * @param state_id_ 
		 */
		void SetStartState(unsigned int state_id_);


		/**
		 * @brief Print the state list.
		 */
		void PrintStateList() const;

		/**
		 * @brief  Get the current state ID.
		 * @return Current state ID (Enum)
		 */
		RS_N_D unsigned int GetCurrentStateID() const { return m_current_state_id; }

		/**
		 * @brief Check the state with the given ID exist.
		 * @param state_id_ State ID (Enum)
		 * @return True if the state exist.
		 */
		RS_N_D bool IsExistState(unsigned int state_id_) const ;


	private:
		//unsigned int m_last_state_id = 0;
    unsigned int m_current_state_id = 0; ///< Current state ID
    RSStatement m_current_state; ///< Current state

    std::map<unsigned int, RSStatement> m_state_map; ///< State map
	};

	extern RSState* mRSSTATE;

}

#endif // !RSSTATE_H#endif // !RSSTATE_H