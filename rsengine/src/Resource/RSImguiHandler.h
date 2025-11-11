/******************************************************************************/
/*!
\file   RSImguiHandler.h
\author Jinseob Park
\date   2024/08/01

This file contains imgui debug handler class for Game engine and logic part.
This file will be moved to Logic part.

*/
/******************************************************************************/
#ifndef RS_IMGUI_HANDLER_H_
#define RS_IMGUI_HANDLER_H_

#include "Manager/RSResourceManager.h"
#include "../Systems/Input/RSInputClass.h"
#include <string>

/**
 * @brief Imgui handler class for debug
 */
namespace RS_Handler
{
	using namespace RS_Particle;
	using namespace RS_Object;

	/**
	 * @brief Struct for Object Rendering layer.
	 */
	struct RSImguiLayer
	{
		RSRenderLayer m_layer;
		std::string m_layer_name;
		RSImguiLayer(const RSRenderLayer layer_, const std::string& name_) 
			{ m_layer = layer_; m_layer_name = name_; }
	};

	/**
	 * @brief Imgui handler class for debug
	 */
	class RSImguiHandler
	{
	public:
		/**
		 * @brief Constructor. Set Imgui handler pointer.
		 */
		RSImguiHandler();

		/**
		 * @brief Destructor. Do nothing.
		 */
		~RSImguiHandler();

		/**
		 * @brief Initialize. Set imgui property.
		 * Set font, style, etc.
		 */
		void Initialize();

		/**
		 * @brief Pre Update.
		 * Create Imgui frame.
		 * Render Imgui Frame.
		 * @param dt delta time.
		 */
		void UpdatePre(float dt);

		/**
		 * @brief Post Update.
		 * Docker Update, switch context.
		 * @param dt delta time.
		 */
		void UpdatePost(float dt);

		/**
		 * @brief Shutdown. Release Imgui.
		 */
		void Shutdown() const;

		/**
		 * @brief Open File Dialog.
		 */
		void ShowOpenFileDialog() const;

		/**
		 * @brief Show Manager Items.
		 * It includes Texture, Shader, Object, etc.
		 */
		void ShowManagerItems();

		void ShowObjectList(const RSResourceManager* rm);

		void ShowMeshList(const RSResourceManager* rm);

    void ShowMaterialList(const RSResourceManager* rm);

    void ShowTextureList(const RSResourceManager* rm);

    void ShowObjectControl(const RSResourceManager* rm);

    void ShowModelControl(const RSResourceManager* rm);

    void ShowMaterialControl(const RSResourceManager* rm);

		/**
		 * @brief Show Camera Control Items.
		 * It includes Camera, Light, etc.
		 */
		void ShowCameraItems();

		/**
		 * @brief Show Play Control Items
		 * It Includes Particle simulator controller and clipping.
		 */
		void ShowPlayItems();

    /**
     * @brief Show Point Clipper Items.
		 */
		void ShowPointClipperItems();

    /**
     * @brief Show VTK Controller Items.
		 */
		void ShowVtkControllerItems();


    /**
     * @brief Show Viewport.
		 */
		void ShowViewport();

		/**
		 * @brief Set selected object by mouse picking.
		 * @param obj_ selected object.
		 */
		void SelectedObjectByMouse(RSObject* obj_);

		/**
		 * @brief Get selected object.
		 * @return selected object.
		 */
		RSObject* GetSelectedObject() const {return selected_object;}

		/**
		 * @brief Reset selected object.
		 */
		void ResetSelectedObject() {selected_object = nullptr;}

	private:
    GLFWwindow* p_window; ///< GLFW window pointer
    RSObject* selected_object = nullptr; ///< Selected object pointer
    bool m_file_dialog_open = false; ///< File dialog open flag
    ImGuiFileDialog* file_dialog_helper = nullptr; ///< File dialog helper

    std::string selected_obj_name; ///< Selected object name
    std::string target_object_name; ///< Target object name

    std::vector<RSImguiLayer> m_imgui_layers; ///< Imgui layer list
    int GetLayerIndex(RSRenderLayer layer_); ///< Get layer index
    void SetLayerIndex(int index_); ///< Set layer index

    bool m_show_debug_buffer = false; ///< Show debug buffer flag
    bool m_show_final_viewport = false; ///< Show final viewport flag

    ImFont* m_font = nullptr; ///< Imgui font

		// IMGUI viewport handling
	protected:
    bool IsMouseInFinalViewportImage(float x_pos, float y_pos) const;
		void MouseButtonHandler(RSMouse button, bool down);
    void KeyButtonHandler(RSKey key, bool down);

		void MappingKeyMouseBinding();

		// Match Imgui KEY and RSKey
    std::unordered_map<int, RSKey> m_key_map;
    std::unordered_map<int, RSMouse> m_mouse_map;

	};
	extern RSImguiHandler* mRSIMGUIHANDLER;
}


#endif // RS_IMGUI_HANDLER_H_