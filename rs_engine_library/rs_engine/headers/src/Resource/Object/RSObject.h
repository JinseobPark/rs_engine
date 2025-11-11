/******************************************************************************/
/*!
\file   RSObject.h
\author Jinseob Park
\date   2024/08/01

This file contains Object class for object. 

*/
/******************************************************************************/
#ifndef RS_OBJECT_H_
#define RS_OBJECT_H_

#include <map>
#include "glm/glm.hpp"
#include "Component/RSComponentHeader.h"

/**
 * @brief Object namespace
 */
namespace RS_Object
{
	using namespace RS_Component;
	using RS_PipelineList::RSShaderNames;
	using RS_PipelineList::RSRenderLayer;
	using RS_PipelineList::RSPipelineType;

	/**
	 * @brief Object class
	 * This class is derived from RSObjectBase class.
	 */
	class RSObject : public RSObjectBase
	{
	public:
		RSObject();
		/**
		 * @brief All components are deleted.
		 */
		~RSObject() override;

    // Deep copy constructor include all components
    RSObject(const RSObject& other);

		/**
		 * @brief Initialize. All components are initialized.
		 */
		void Initialize() const;
		/**
		 * @brief Update. All components are updated. Nothing to do yet in this class.
		 * @param dt Delta time
		 */
		void Update(float dt);
		/**
		 * @brief When the object's render layer is same with the input layer, the object is drawn.
		 * @param layer_ Render layer
		 */
		void Draw(RSRenderLayer layer_);

		/**
		 * @brief Destroy. Nothing to do yet in this class.
		 */
		void Destroy();

		/**
		 * @brief Add component to the object.
		 * If the component pointer is null, return false.
		 * If the component is already added, return false.
		 * @param p_component Component pointer to add
		 * @return If the component is added, return true. Otherwise, return false.
		 */
		bool AddComponent(RSComponent* p_component);

		/**
		 * @brief Remove component from the object.
		 * If the component is not found, return false.
		 * @param type_ Component type to remove
		 * @return If the component is removed, return true. Otherwise, return false.
		 */
		bool RemoveComponent(RSComponentType type_);

		/**
		 * @brief Get component from the object.
		 * If the component is not found, return nullptr.
		 * @param type_ Component type to get
		 * @return If the component is found, return the component pointer. Otherwise, return nullptr.
		 */
		RSComponent* GetComponent(RSComponentType type_);

		/**
		 * @brief Get all components from the object by map
		 * @return All components map
		 */
		std::map<RSComponentType, RSComponent*>* GetComponents() { return &m_components; }	

		// Not Use Shader Type. This be substituted by Render Layer
		void SetShaderType(const RSShaderNames type) { m_shader_type = type; }
		// Not Use Shader Type. This be substituted by Render Layer
		RS_N_D RSShaderNames GetShaderType() const { return m_shader_type; }

		// Not Use Pipeline Type. This be substituted by Render Layer
		void SetPipelineType(const RSPipelineType type) { m_pipeline_types = type; }
		// Not Use Pipeline Type. This be substituted by Render Layer
		RS_N_D RSPipelineType GetPipelineType() const { return m_pipeline_types; }

		/**
		 * @brief Set Render Layer.
		 * @param layer Render layer
		 */
		void SetRenderLayer(RSRenderLayer layer);
		/**
		 * @brief Get Render Layer
		 * @return Render layer
		 */
		RSRenderLayer GetRenderLayer() const { return m_render_layer; }

		/**
		 * @brief Get transform matrix of the object
		 * @return Transform matrix of the object (mat4)
		 */
		glm::mat4 GetTransformMatrix();

		/**
		 * @brief Get transform of the object
		 * @return Transform of the object (RSTransform)
		 */
		RSTransform* GetTransform() { return &m_transform; }

		/**
		 * @brief Set transform of the object
		 * @param transform_ Transform of the object (RSTransform)
		 */
		void SetTransform(const RSTransform& transform_) { m_transform = transform_; }


    /**
     * @brief Set visible from model component.
     * @param visible visible
     */
		void SetVisible(bool visible);


	private:
    RSTransform m_transform; ///< Transform of the object
    std::map<RSComponentType, RSComponent*> m_components; ///< All components of the object
    RSShaderNames m_shader_type = RSShaderNames::DEFAULT_MVPLT; // Not Use Shader Type. This be substituted by Render Layer
    RSPipelineType m_pipeline_types = RSPipelineType::DEFAULT; // Not Use Pipeline Type. This be substituted by Render Layer
    RSRenderLayer m_render_layer = RSRenderLayer::R_SURFACE_OPAQUE; // Render Layer

		friend class RS_Handler::RSImguiHandler;

	};
}

#endif // !RS_OBJECT_H_