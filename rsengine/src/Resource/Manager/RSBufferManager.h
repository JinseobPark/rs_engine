/***************************************************************************************************************/
/*!
\file	RSBufferManager.h 
\author Jinseob Park
\date   2024/08/01

This file contains buffer manager class for rendering system.
*/
/**************************************************************************************************************/
#ifndef RS_BUFFER_MANAGER_H_
#define RS_BUFFER_MANAGER_H_

#include "Resource/Pipeline/FBO/RSFBOHeader.h"

/**
  * @brief Rendering System Internal namespace
  * This namespace is for internal use only.
*/
namespace _RS_Internal
{

  enum class SkyboxTheme : uint8_t
	{
	  DEFAULT = 0,
		TICLE,
		MINT_VERSE,
		CITY,
		CLOUDY,
		OFFICE
	};


/**
 * @brief Rendering Buffer Manager
 * @note This class manages rendering buffers for rendering system.
 * @include Skybox, Shadow Mapping, HUD, Picking HUD, Quad Object.
 * 
 */
class RSBufferManager
{
public:
	RSBufferManager();
	~RSBufferManager();

	void Initialize();
	void PostInitialize();
	void Update(float dt);
	void Shutdown();

	/**
	 * @brief Get Opaque Texture Buffer (not used)
	 * @return opaque texture buffer id (uint) 
	 */
	RS_N_D unsigned int GetOpaqueTextureBuffer() const { return opaque_texture_buffer; }

	/**
	 * @brief Get Transparent Texture Buffer (not used)
	 * @return transparent texture buffer id (uint)
	 */
	RS_N_D unsigned int GetTransparentTextureBuffer() const { return transparent_texture_buffer; }


	/**
	 * @brief Link Shader texture id. 
	 * @brief When add shader with using texture, add texture id to shader on this function.
	 * @return void
	 */
	static void InitializeConfiguration();
	
  /**
   * @brief Release all buffers
	 */
	void ReleaseAllBuffers();

  /**
	 * @brief Release all textures
	 */
	void ReleaseAllTextures();

	// ***************************//
	// ******* Fbo items *********//
	// ****************************//

  /**
   * @brief Create Fbo items
	 */
	void AllocateFboItems();

  /**
   * @brief Initialize Fbo items
	 */
	void CreateFboItems(const int width_, const int height_);

  /**
   * @brief Release Fbo items
	 */
	void ReleaseFboItems();

  /**
   * @brief Resize Fbo items about main window size. It will be called when window size changed.
   * This function exclude the shadow mapping buffer.
   * @param width width
   * @param height height
   */
  void ResizeFboItems(int width, int height);

  /**
   * @brief Get Fbo item
   * @param type Fbo type
   * @return RSFBO* Fbo item
   */
  RSFBO* GetFboItem(const FboType type) { return m_fbo_map[type]; }

  /**
   * @brief Bind Fbo item
   * @param type Fbo type
   */
  void BindFbo(FboType type);

  /**
   * @brief Unbind Fbo item
   * @param type Fbo type
	 */
	void UnbindFbo(FboType type);


	// *********************//
	// ****** Shadow *******//
	// *********************//

  /**
   * @brief Clear shadow map buffer
   * It will be called when shadow mapping is not used.
   * This function includes clear depth buffer.
	 */
	void ClearShadowMap();

	// ***********************//
	// ****** Deferred *******//
	// ***********************//


  /**
   * @brief Draw Final Fbo
   * It will be called when deferred rendering is done.
   * This function includes binding final fbo, drawing the quad, unbinding final fbo.
	 */
	void DrawFinalFbo();

	// ***************************//
	// ****** Get Textures *******//
	// ***************************//

  /**
   * @brief Get Final Texture Buffer from Final Fbo
   * @return final texture buffer id (uint)
   */
  unsigned int GetFinalTexture();

  /**
   * @brief Get Position Texture Buffer from Deferred Fbo
   * @return position texture buffer id (uint)
	 */
	unsigned int GetDeferredPositionTexture();

  /**
   * @brief Get Normal Texture Buffer from Deferred Fbo
   * @return normal texture buffer id (uint)
	 */
	unsigned int GetDeferredNormalTexture();

  /**
   * @brief Get Albedo Texture Buffer from Deferred Fbo
   * @return albedo texture buffer id (uint)
	 */
	unsigned int GetDeferredAlbedoTexture();

	/**
  * @brief Get Depth Texture Buffer from Deferred Fbo
  * @return depth texture buffer id (uint)
  */
	unsigned int GetCustomDepthTexture();

  /**
   * @brief Get Fluid Position Texture Buffer from Deferred Fbo
   * @return fluid position texture buffer id (uint)
	 */
	unsigned int GetFluidDeferredPositionTexture();

  /**
   * @brief Get Fluid Normal Texture Buffer from Deferred Fbo
   * @return fluid normal texture buffer id (uint)
   */
  unsigned int GetFluidDeferredNormalTexture();

  /**
   * @brief Get Fluid Albedo Texture Buffer from Deferred Fbo
   * @return fluid albedo texture buffer id (uint)
	 */
	unsigned int GetFluidDeferredAlbedoTexture();

  /**
   * @brief Get Fluid Depth Texture Buffer from Deferred Fbo
   * @return fluid depth texture buffer id (uint)
   */
  unsigned int GetFluidDeferredDepthTexture();


  /**
   * @brief Get Custom Index Texture Buffer from Deferred Fbo
   * @return custom index texture buffer id (uint)
	 */
	unsigned int GetCustomIndexTexture();

  /**
   * @brief Get Depth Texture Buffer from Deferred Fbo
   * @return depth texture buffer id (uint)
	 */
	unsigned int GetDepthMap();

	/**
   * @brief Get shadow depth map texture buffer from Shadow Map Fbo
	 * @return shadow depth map id (uint)
	 */
	unsigned int GetShadowMap();

	/**
   * @brief Get Picking Texture Buffer from Picking Fbo
	 * @return Picking texture buffer id (uint)
	 */
	RS_N_D unsigned int GetPickingTexture();

	/**
  * @brief Get HUD texture buffer
  * @return HUD texture buffer id (uint)
  */
	RS_N_D unsigned int GetHudTexture();

  /**
   * @brief Get SSAO texture buffer
   * @return SSAO texture buffer id (uint)
	 */
	unsigned int GetSSAOTexture();

  /**
   * @brief	Get SSAO Noise texture buffer
   * @return SSAO noise texture buffer id (uint)
	 */
	unsigned int GetSSAONoiseTexture();

  /**
   * @brief Get SSAO Blur texture buffer
   * @return SSAO blur texture buffer id (uint)
   */
  unsigned int GetSSAOBlurTexture();

  /**
   * @brief Get SSR texture buffer
   * @return SSR texture buffer id (uint)
   */
  unsigned int GetSSRTexture();

  /**
   * @brief Get SSR Blur texture buffer
   * @return SSR blur texture buffer id (uint)
   */
  unsigned int GetSSRBlurTexture();

  /**
   * @brief Get Post-Process A texture buffer
   * @return Post-process A texture buffer id (uint)
   */
  unsigned int GetPostProcessATexture();

  /**
   * @brief Get Post-Process B texture buffer
   * @return Post-process B texture buffer id (uint)
   */
  unsigned int GetPostProcessBTexture();

  // ********************************//
  // ****** Kernel UBO Control ******//
  // ********************************//

  /**
   * @brief Create kernel UBO for image post-processing
   */
  void CreateKernelUBO();

  /**
   * @brief Update kernel UBO data
   * @param kernel 3x3 kernel array (9 floats)
   * @param divisor Normalization divisor
   * @param offset Color offset
   */
  void UpdateKernelUBO(const float* kernel, float divisor, float offset);

  /**
   * @brief Release kernel UBO
   */
  void ReleaseKernelUBO();

  /**
   * @brief Get kernel UBO id
   * @return Kernel UBO id (uint)
   */
  unsigned int GetKernelUBO() const { return m_kernel_ubo; }

	// *********************//
	// ****** Skybox *******//
	// *********************//

	/**
	 * @brief Load cubemap texture
	 * @param[in] faces_ - vector of string for cubemap texture path
	 * @return cubemap texture buffer uint id
	 */
	unsigned int LoadCubemap(const std::vector<std::string>& faces_);
	/**
	 * @brief Get skybox texture buffer
	 * @return skybox texture buffer uint id
	 */
	RS_N_D unsigned int GetSkyboxTextureBuffer() const { return skybox_texture_buffer; }

	/**
	 * @brief Get skybox VAO
	 * @return skybox VAO uint id
	 */
	RS_N_D unsigned int GetSkyboxVAO() const { return skybox_vao; }
	/**
	 * @brief Create skybox VAO
	 * @brief Load skybox texture (LoadCubeMap)
	 * @see LoadCubemap()
	 * @return void
	 */
	void InitializeSkybox();

	/////////////////
	// Quad Object //
	/////////////////

	/**
	 * @brief Create Quad object for Frame buffer texture
	 */
	void InitializeQuad();

	/**
	 * @brief Draw bound frame buffer texture on quad object
	 * @return void
	 */
	void DrawQuad() const;

	/**
	 * @brief Resize Buffers when window size changed
	 * @param width changed window width
	 * @param height changed window height
	 * @return void
	 * @see RSApplication::ResizeApp
	 * @see RSResourceManager::Resize
	 */
	void ResizeBuffers(int width, int height);

private:
	int buffer_width = 1024, buffer_height = 1024;  ///< Window size for buffer

	unsigned int opaque_texture_buffer = 0;					///< Opaque texture buffer id
	unsigned int transparent_texture_buffer = 0;			///< Transparent texture buffer id
	unsigned int skybox_texture_buffer = 0;					///< Skybox texture buffer id

	unsigned int quad_vao = 0, quad_vbo = 0;				///< Quad object vao, vbo id
	unsigned int skybox_vao = 0, skybox_vbo = 0;			///< Skybox vao, vbo id

  std::map<FboType, RSFBO*> m_fbo_map;	///< FBO map for FBO type

  SkyboxTheme skybox_theme = SkyboxTheme::DEFAULT;	///< Skybox theme

  unsigned int m_kernel_ubo = 0;  ///< Kernel UBO for image post-processing
};
} // namespace _RS_Internal

#endif // !RS_BUFFER_MANAGER_H_