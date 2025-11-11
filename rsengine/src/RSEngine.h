#pragma once
#include "Systems/RSSystemHeader.h"
#include <string>
#include <vector>

/**
@class		RSEngine <STRONG>R</STRONG>ealtime-<STRONG>S</STRONG>imulation Engine (Rock & Stone Engine) 
@version	0.0.6
@author		Jinseob Park
@brief		Main Engine Class for <STRONG>R</STRONG>ealtime-<STRONG>S</STRONG>imulation Engine
*/

/**
 * @brief Engine namespace 
 */
namespace RS_Engine
{
	using namespace RS_Event;

	/**
	 * @brief RS Engine class
	 */
	class RSEngine
	{
		public:
			/**
			* @brief Constructor for RSEngine
			* Check if there is only one engine
			*/
			RSEngine();

			/**
			 * @brief Destructor for RSEngine
			 * Destroy all systems
			 */
			~RSEngine();

			/**
			 * @brief Initialize Engine
			 * Add Systems to Engine
			 * Initialize Systems
			 * Initialize ResourceManager
			 * @return true if success
			 * @see AddSystem
			 */
			bool Initialize();

			/**
			 * @brief Run Engine
			 * Update Engine
			 * @see Update
			 */
			void Run();

			/**
			 * @brief Destroy All Systems
			 */
			void DestroyAllSystems();

			/**
			 * @brief Shutdown systems
			 * Shutdown and release resource manager
			 */
			void Shutdown() const;

			/**
			 * @brief Update Engine
			 * @return true if success. If false, stop the engine
			 */
			bool Update();

			/**
			 * @brief Get Window
			 * @return GLFWwindow* window
			 */
			static GLFWwindow* GetWindow() { return RS_Application::mRSAPPLICATION->GetWindow(); }

			/**
			 * @brief Resize Application
			 * @param width Window width
			 * @param height Window height
			 */
			static void ResizeApp(const int width, const int height) { RS_Application::mRSAPPLICATION->ResizeApp(width, height); }

			/**
				 * @brief Get Fixed FPS status.
				 * @return true if fixed fps.
				 */
			bool IsFixedFps() const { return b_fixed_fps; }

			/**
			 * @brief Set Fixed FPS status.
			 * @param fixed_fps true if fixed fps.
			 */
			void SetFixedFps(const bool fixed_fps);

			/**
			 * @brief Get FPS.
			 * @return FPS.
			 */
			float GetFps() const { return m_fixed_fps; }

			/**
			 * @brief Set FPS.
			 * @param fps FPS.
			 */
			void SetFps(const float fps);

      /**
       * @brief Get Delta time
       * @return Delta time
       */
      float GetDeltaTime() const { return m_fixed_delta_time; }

			/**
			 * @brief Set Delta time
			 * @param dt Delta time
			 */
			void SetDeltaTime(const float dt);


			/**
			 * @brief When current time is bigger than last time, return true.
			 * @return Updateable status.
			 */
			bool IsUpdateable();


			// Add Event with RSEventItem
			void AddEvent(const RSEventItem& event_) { mRSEVENTHANDLER->AddEvent(event_); }

			// Add Event with type, name, data
			void AddEvent(const EventType type_, const std::string& name_, const EventData& data_) { mRSEVENTHANDLER->AddEvent(type_, name_, data_); }

			// Add Event with type, name, path
			void AddEvent(const EventType type_, const std::string& name_, const std::string& path_) { mRSEVENTHANDLER->AddEvent(type_, name_, path_); }

			// Add Event with type, name
			void AddEvent(const EventType type_, const std::string& name_) { mRSEVENTHANDLER->AddEvent(type_, name_); }

			// Add Event with type, data
			void AddEvent(const EventType type_, const EventData& data_) { mRSEVENTHANDLER->AddEvent(type_, data_); }

			// Add Event with type
			void AddEvent(const EventType type_) { mRSEVENTHANDLER->AddEvent(type_); }

		protected:
			/**
			 * @brief Add System to Engine
			 * @param system system to add
			 */
			void AddSystem(RSSystemComponent* system);

		private:
      std::vector<RSSystemComponent*> m_systems; ///< Systems
      bool is_running = true; ///< Running status
      bool b_update_able = true; ///< Updateable status

      bool b_fixed_fps = true; ///< Fixed FPS status
      float m_fixed_fps = 100.0f; ///< Fixed FPS
      float m_fixed_delta_time = 0.01f; ///< Fixed Delta time

      float m_engine_last_time = 0; ///< Last time
			
      RS_Logger::RSLogger* m_logger = nullptr; ///< Logger
      friend class RS_Handler::RSImguiHandler; ///< Imgui handler
	}; // class RSEngine

  extern RSEngine* mRSENGINE; ///< Engine pointer
}