/******************************************************************************/
/*!
\file   RSGraphics.h
\author Jinseob Park
\date   2024/08/01

This file contains Graphics System for Game engine and logic part.

*/
/******************************************************************************/
#ifndef RS_GRAPHICS_H_
#define RS_GRAPHICS_H_

#include "Systems/RSSystemComponent.h"
#include "Systems/RSSystemHeader.h"
#include "Resource/Manager/RSResourceManager.h"



/**
 * @brief Graphics namespace.
 */
namespace RS_Graphics
{
	/**
	 * @brief Graphics system class.
	 * This class derived from System Component.
	 */
	class RSGraphics : public RSSystemComponent
	{
	public:
		/**
		 * @brief Constructor
		 */
		RSGraphics();
		/**
		 * @brief Destructor. Do nothing.
		 * 
		 */
		~RSGraphics() override;

		/**
		 * @brief Opengl Graphics setting.
		 */
		void Initialize() override;

		/**
		 * @brief Post Initialize.
		 * Link with resource manager.
		 */
		void PostInitialize() override;

		/**
		 * @brief Call Draw Function. 
		 * Clear frame buffers to draw.
		 * Mapping shadow map.
		 * Mapping HUD for picking.
		 * @param dt Delta time.
		 * @see ShadowMapping()
		 * @see HUDMapping()
		 * @see Draw()
		 */
		void Update(float dt) override;

		/**
		 * @brief Swap buffer. This function was made for IMGUI.
		 * @param dt Delta time. Commonly, this value is not used.
		 */
		void UpdatePost(float dt);

		/**
		 * @brief Shutdown Graphics system.
		 */
		void Shutdown() override;


		//********************************************************************************//
		//*************************** Public  Rendering Flags ****************************//
		//********************************************************************************//


    /**
     * @brief Add rendering flag.
     * @param flag_ Rendering flag.
		 */
		void AddRenderingFlag(const RenderingFlag flag_) { m_rendering_flag |= flag_; }

    /**
     * @brief Remove rendering flag.
     * @param flag_ Rendering flag.
		 */
		void RemoveRenderingFlag(const RenderingFlag flag_) { m_rendering_flag &= ~flag_; }

    /**
     * @brief Set rendering flag.
     * @param flag_ Rendering flag.
     * @param set_ Set flag.
		 */
		void SetRenderingFlag(const RenderingFlag flag_, const bool set_);

    /**
     * @brief Set rendering flag dirty to true
		 */
		void SetRenderingFlagDirty();

    /**
     * @brief Get rendering flag.
     * @return Rendering flag.
		 */
		RenderingFlag GetRenderingFlag() const { return m_rendering_flag; }

    /**
     * @brief Get rendering flag bool.
     * @param flag_ Rendering flag.
     * @return bool
		 */
		bool GetRenderingFlagBool(const RenderingFlag flag_) const { return m_rendering_flag & flag_; }

    /**
     * @brief Get kernel post-process data reference for ImGui editing
     * @return Reference to kernel data
		 */
		RSKernelPostProcessData& GetKernelData() { return m_kernel_data; }

    /**
     * @brief Get kernel post-process data const reference
     * @return Const reference to kernel data
		 */
		const RSKernelPostProcessData& GetKernelDataConst() const { return m_kernel_data; }

	protected:

    /**
     * @brief Clear buffer with glClear.
		 */
		void ClearBuffer();

		/**
		 * @brief Draw function.
		 * Draw all objects in the scene.
		 */
		void Draw();

		/**
		 * @brief Draw Opaque objects.
		 */
		void OpaqueDraw() const;

    /**
     * @brief Draw PBR objects.
		 */
		void OpaqueDrawPbr() const;

		/**
		 * @brief Draw Line objects.
		 */
		void LineDraw() const;

		/**
		 * @brief Draw Wireframe objects.
		 */
		void WireFrameDraw() const;

		/**
		 * @brief Draw Point objects.
		 */
		void PointDrawDeferred() const;

		void PointDrawForward() const;

		/**
		 * @brief Draw Picked object.
		 */
		void DrawPickedObject() const;

		/**
		 * @brief Draw Transparent objects.
		 */
		void TransparentDraw() const;


    /**
     * @brief Draw Transparent objects with PBR.
		 */
		void TransparentDrawPbr() const;

		/**
		 * @brief Draw HUD.
		 */
		void HUDDraw();

		/**
		 * @brief Draw Skybox.
		 */
		void SkyboxDraw() const;

		/**
		 * @brief Draw Shadow.
		 */
		void ShadowMapping();

		/**
		 * @brief Draw Stencil Buffer Object.
		 */
		void HUDMapping() const;

		/**
		 * @brief Update Light information.
		 */
		void UpdateLight() const;

    //********************************************************************************//
    //******************************* Rendering Flags ********************************//
    //********************************************************************************//

    /**
     * @brief Deferred Rendering.
		 */
		void DeferredRendering();

    /**
     * @brief Forward Rendering.
		 */
		void ForwardRendering();

    /**
     * @brief Deferred and forward rendering.
		 */
		void Render();

		/**
		 * @brief Composite HUD.
		 */
		void CompositeHUD() const;

    /**
     * @brief Draw Opaque objects to GBuffer.
		 */
		void DrawOpaqueToGBuffer();

    /**
     * @brief Draw Particle Simulator Point to GBuffer.
		 */
		void DrawParticleSimulatorPointToGBuffer();

    /**
     * @brief Composite GBuffer.
		 */
		void CompositeGBuffer();

    /**
     * @brief Draw SSAO.
		 */
		void DrawSSAO();

    /**
     * @brief Draw SSAO Blur.
		 */
		void DrawSSAOBlur();

    /**
     * @brief Clear SSAO.
		 */
		void ClearSSAO();

    /**
     * @brief Draw Screen space reflection.
		 */
		void DrawSSR();

    /**
     * @brief Draw Screen space reflection blur.
		 */
		void DrawSSRBlur();

    /**
     * @brief Clear SSR fbo
		 */
		void ClearSSR();

    /**
     * @brief Draw kernel-based image post-processing
     * Applies 3x3 convolution kernel to final image (excludes HUD)
     * Supports multi-pass chaining with ping-pong buffers
		 */
		void DrawKernelPostProcess();

    /**
     * @brief Clear kernel post-process FBOs
		 */
		void ClearKernelPostProcess();

private:
    /**
		 * @brief Draw shadow map for debug
		 */
		void DrawShadowMapDebug() const;

    /**
     * @brief Draw HUD for debug
		 */
		void HudDrawDebug() const;

    /**
     * @brief Set Viewport
     * @param width width
     * @param height height
     */
    void SetViewport(int width, int height) const;


    /**
		 * @brief Draw Picking HUD for debug
		 */
		void DrawPickingHudDebug() const;

		void StencilBufferObjectDraw() const;
		void StencilBufferFinish() const;


	private:
    RenderingFlag m_rendering_flag = RenderingFlag::OPAQUE_DEFERRED; ///< Rendering flag.

    bool b_rendering_flag_dirty = true; ///< Rendering flag dirty flag.

	  RSResourceManager* p_resource_manager = nullptr; ///< Resource manager pointer.

    RSKernelPostProcessData m_kernel_data; ///< Kernel post-process data for image processing

		
	};
	extern RSGraphics* mRSGRAPHICS;
}

#endif // !RS_GRAPHICS_H_#endif // !RS_GRAPHICS_H_