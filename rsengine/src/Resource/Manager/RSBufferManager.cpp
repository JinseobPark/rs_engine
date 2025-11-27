#include "pch.h"
#include "RSBufferManager.h"
#include "Systems/Application/RSApplication.h"
#include "RSResourceManager.h"
#include <image/stb_image.h>
#include <filesystem>

namespace _RS_Internal
{
	using RS_Shader::RSShaderNames;

	RSBufferManager::RSBufferManager()
	= default;

	RSBufferManager::~RSBufferManager()
	= default;

	void RSBufferManager::Initialize()
	{
    skybox_theme = SkyboxTheme::CITY;
		buffer_width = static_cast<int>(RS_Application::mRSAPPLICATION->GetWidth());
		buffer_height = static_cast<int>(RS_Application::mRSAPPLICATION->GetHeight());

		InitializeSkybox();

		InitializeQuad();


		AllocateFboItems();
		CreateFboItems(buffer_width, buffer_height);

		CreateKernelUBO();

		InitializeConfiguration();
	}

	void RSBufferManager::PostInitialize()
	{

	}

	void RSBufferManager::Update(float dt)
	{
	}

	void RSBufferManager::Shutdown()
	{
		ReleaseAllBuffers();
		ReleaseAllTextures();

		ReleaseFboItems();
		ReleaseKernelUBO();
	}
	
	void RSBufferManager::InitializeSkybox()
	{
		constexpr float skybox_vertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};


		// skybox VAO
		glGenVertexArrays(1, &skybox_vao);
		glGenBuffers(1, &skybox_vbo);
		glBindVertexArray(skybox_vao);
		glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces(6);

		switch(skybox_theme)
		{
    case SkyboxTheme::DEFAULT:
      {
      faces =
      {
        "resources/InitLoadFiles/Skybox/right.jpg",
        "resources/InitLoadFiles/Skybox/left.jpg",
        "resources/InitLoadFiles/Skybox/top.jpg",
        "resources/InitLoadFiles/Skybox/bottom.jpg",
        "resources/InitLoadFiles/Skybox/front.jpg",
        "resources/InitLoadFiles/Skybox/back.jpg"
      };
        break;
      }
		case SkyboxTheme::TICLE:
		{
			faces =
			{
				"resources/InitLoadFiles/Skybox/top.bin",
				"resources/InitLoadFiles/Skybox/top.bin",
				"resources/InitLoadFiles/Skybox/top.bin",
				"resources/InitLoadFiles/Skybox/top.bin",
				"resources/InitLoadFiles/Skybox/top.bin",
				"resources/InitLoadFiles/Skybox/top.bin"
			};
			break;
		}
    case SkyboxTheme::MINT_VERSE:
    {
      faces =
      {
        "resources/InitLoadFiles/Skybox/mintverse_left.png",
        "resources/InitLoadFiles/Skybox/mintverse_right.png",
        "resources/InitLoadFiles/Skybox/mintverse_up.png",
        "resources/InitLoadFiles/Skybox/mintverse_down.png",
        "resources/InitLoadFiles/Skybox/mintverse_front.png",
        "resources/InitLoadFiles/Skybox/mintverse_back.png"
      };
			break;
    }
		case SkyboxTheme::CITY:
    {
      faces =
      {
        "resources/InitLoadFiles/Skybox/city_right.png",
        "resources/InitLoadFiles/Skybox/city_left.png",
        "resources/InitLoadFiles/Skybox/city_top.png",
        "resources/InitLoadFiles/Skybox/city_bottom.png",
        "resources/InitLoadFiles/Skybox/city_back.png",
        "resources/InitLoadFiles/Skybox/city_front.png"
      };
      break;
    }
		case SkyboxTheme::CLOUDY:
    {
      faces =
      {
        "resources/InitLoadFiles/Skybox/cloudy_right.png",
        "resources/InitLoadFiles/Skybox/cloudy_left.png",
        "resources/InitLoadFiles/Skybox/cloudy_top.png",
        "resources/InitLoadFiles/Skybox/cloudy_bottom.png",
        "resources/InitLoadFiles/Skybox/cloudy_back.png",
        "resources/InitLoadFiles/Skybox/cloudy_front.png"
      };
      break;
    }
		case SkyboxTheme::OFFICE:
    {
      faces =
      {
        "resources/InitLoadFiles/Skybox/office_right.png",
        "resources/InitLoadFiles/Skybox/office_left.png",
        "resources/InitLoadFiles/Skybox/office_top.png",
        "resources/InitLoadFiles/Skybox/office_bottom.png",
        "resources/InitLoadFiles/Skybox/office_back.png",
        "resources/InitLoadFiles/Skybox/office_front.png"
      };
      break;
    }
		}
//#if 0
//		std::vector<std::string> faces
//		{
//			"resources/InitLoadFiles/Skybox/right.jpg",
//			"resources/InitLoadFiles/Skybox/left.jpg",
//			"resources/InitLoadFiles/Skybox/top.jpg",
//			"resources/InitLoadFiles/Skybox/bottom.jpg",
//			"resources/InitLoadFiles/Skybox/front.jpg",
//			"resources/InitLoadFiles/Skybox/back.jpg"
//		};
//#endif
//#if 0
//		std::vector<std::string> faces
//		{
//			"resources/InitLoadFiles/Skybox/top.bin",
//			"resources/InitLoadFiles/Skybox/top.bin",
//			"resources/InitLoadFiles/Skybox/top.bin",
//			"resources/InitLoadFiles/Skybox/top.bin",
//			"resources/InitLoadFiles/Skybox/top.bin",
//			"resources/InitLoadFiles/Skybox/top.bin"
//		};
//#endif
//#if 0
//		std::vector<std::string> faces
//		{
//			"resources/InitLoadFiles/Skybox/mintverse_left.png",
//			"resources/InitLoadFiles/Skybox/mintverse_right.png",
//			"resources/InitLoadFiles/Skybox/mintverse_up.png",
//			"resources/InitLoadFiles/Skybox/mintverse_down.png",
//			"resources/InitLoadFiles/Skybox/mintverse_front.png",
//			"resources/InitLoadFiles/Skybox/mintverse_back.png"
//		};
//#endif

		skybox_texture_buffer = LoadCubemap(faces);

	}

	void RSBufferManager::AllocateFboItems()
	{
    m_fbo_map[FboType::SHADOW_MAP] = new RSShadowMapFbo();
    m_fbo_map[FboType::HUD] = new RSHudFbo();
    m_fbo_map[FboType::PICKING] = new RSPickingFbo();
    //m_fbo_map[FboType::SKYBOX] = new RSSkyboxFbo();
    m_fbo_map[FboType::DEFERRED] = new RSDeferredFbo();
    m_fbo_map[FboType::SSAO] = new RSSsaoFbo();
    m_fbo_map[FboType::SSAO_BLUR] = new RSSsaoBlurFbo();
    m_fbo_map[FboType::SSR] = new RSSsrFbo();
    m_fbo_map[FboType::SSR_BLUR] = new RSSsrBlurFbo();
    m_fbo_map[FboType::POST_PROCESS_A] = new RSPostProcessFbo();
    m_fbo_map[FboType::POST_PROCESS_B] = new RSPostProcessFbo();
    m_fbo_map[FboType::FINAL] = new RSFinalFbo();
	}

	void RSBufferManager::CreateFboItems(const int width_, const int height_)
	{
    for (const auto& [fbo_type, rbo_item] : m_fbo_map)
    {
      rbo_item->CreateFbo(width_, height_);
    }
	}

	void RSBufferManager::ReleaseFboItems()
	{
    for (const auto& [fbo_type, rbo_item] : m_fbo_map)
    {
      rbo_item->ReleaseFbo();
    }

    // deallocate memory
    for (const auto& [fbo_type, rbo_item] : m_fbo_map)
    {
      delete rbo_item;
    }
	}

	void RSBufferManager::ResizeFboItems(const int width, const int height)
	{
    for (auto& [fbo_type, rbo_item] : m_fbo_map)
    {
      rbo_item->Resize(width, height);
    }
	}

	void RSBufferManager::BindFbo(const FboType type)
	{
    m_fbo_map[type]->BindFbo();
	}

	void RSBufferManager::UnbindFbo(const FboType type)
	{
    m_fbo_map[type]->UnbindFbo();
	}

	void RSBufferManager::ClearShadowMap()
	{
    dynamic_cast<RSShadowMapFbo*>(m_fbo_map[FboType::SHADOW_MAP])->ClearShadowMap();
	}

	void RSBufferManager::DrawFinalFbo()
	{
    const auto p_resource_manager = RSResourceManager::GetInstance();

		// Set shader to draw final texture
		p_resource_manager->GetShaderManager()->Use(RSShaderNames::QUAD_TEXTURE);

		glActiveTexture(GL_TEXTURE0);
		// Get final texture

		glBindTexture(GL_TEXTURE_2D, GetFinalTexture());

		DrawQuad();
	}

	unsigned int RSBufferManager::GetFinalTexture()
  {
    return dynamic_cast<RSFinalFbo*>(m_fbo_map[FboType::FINAL])->GetFinalTexture();
  }

	unsigned int RSBufferManager::GetDeferredPositionTexture()
  {
    return dynamic_cast<RSDeferredFbo*>(m_fbo_map[FboType::DEFERRED])->GetDeferredPositionTexture();
  }

  unsigned int RSBufferManager::GetDeferredNormalTexture()
  {
    return dynamic_cast<RSDeferredFbo*>(m_fbo_map[FboType::DEFERRED])->GetDeferredNormalTexture();
  }

	unsigned int RSBufferManager::GetDeferredAlbedoTexture()
  {
    return dynamic_cast<RSDeferredFbo*>(m_fbo_map[FboType::DEFERRED])->GetDeferredAlbedoTexture();
  }

  unsigned int RSBufferManager::GetCustomDepthTexture()
  {
    return dynamic_cast<RSDeferredFbo*>(m_fbo_map[FboType::DEFERRED])->GetCustomDeferredDepthTexture();
  }

	unsigned int RSBufferManager::GetFluidDeferredPositionTexture()
	{
		return dynamic_cast<RSDeferredFbo*>(m_fbo_map[FboType::DEFERRED])->GetFluidDeferredPositionTexture();
	}

	unsigned int RSBufferManager::GetFluidDeferredNormalTexture()
	{
		return dynamic_cast<RSDeferredFbo*>(m_fbo_map[FboType::DEFERRED])->GetFluidDeferredNormalTexture();
	}

	unsigned int RSBufferManager::GetFluidDeferredAlbedoTexture()
	{
    return dynamic_cast<RSDeferredFbo*>(m_fbo_map[FboType::DEFERRED])->GetFluidDeferredAlbedoTexture();
	}

	unsigned int RSBufferManager::GetFluidDeferredDepthTexture()
	{
    return dynamic_cast<RSDeferredFbo*>(m_fbo_map[FboType::DEFERRED])->GetCustomDeferredDepthTexture();
	}

	unsigned int RSBufferManager::GetDepthMap()
	{
		return dynamic_cast<RSDeferredFbo*>(m_fbo_map[FboType::DEFERRED])->GetDepthTexture();
	}

	unsigned int RSBufferManager::GetCustomIndexTexture()
	{
		return dynamic_cast<RSDeferredFbo*>(m_fbo_map[FboType::DEFERRED])->GetCustomMaskTexture();
	}

  unsigned int RSBufferManager::GetShadowMap()
  {
    return dynamic_cast<RSShadowMapFbo*>(m_fbo_map[FboType::SHADOW_MAP])->GetShadowMapTexture();
  }

  unsigned int RSBufferManager::GetPickingTexture()
  {
    return dynamic_cast<RSPickingFbo*>(m_fbo_map[FboType::PICKING])->GetPickingTexture();
  }

  unsigned int RSBufferManager::GetHudTexture()
  {
    return dynamic_cast<RSHudFbo*>(m_fbo_map[FboType::HUD])->GetHudTexture();
  }

	unsigned int RSBufferManager::GetSSAOTexture()
	{
    return dynamic_cast<RSSsaoFbo*>(m_fbo_map[FboType::SSAO])->GetSsaoTexture();
	}

	unsigned int RSBufferManager::GetSSAONoiseTexture()
	{
    return dynamic_cast<RSSsaoFbo*>(m_fbo_map[FboType::SSAO])->GetNoiseTexture();
	}

	unsigned int RSBufferManager::GetSSAOBlurTexture()
	{
    return dynamic_cast<RSSsaoBlurFbo*>(m_fbo_map[FboType::SSAO_BLUR])->GetSsaoBlurTexture();
	}

	unsigned int RSBufferManager::GetSSRTexture()
	{
    return dynamic_cast<RSSsrFbo*>(m_fbo_map[FboType::SSR])->GetSsrTexture();
	}

	unsigned int RSBufferManager::GetSSRBlurTexture()
	{
    return dynamic_cast<RSSsrBlurFbo*>(m_fbo_map[FboType::SSR_BLUR])->GetSsrBlurTexture();
	}

	unsigned int RSBufferManager::GetPostProcessATexture()
	{
    return dynamic_cast<RSPostProcessFbo*>(m_fbo_map[FboType::POST_PROCESS_A])->GetColorTexture();
	}

	unsigned int RSBufferManager::GetPostProcessBTexture()
	{
    return dynamic_cast<RSPostProcessFbo*>(m_fbo_map[FboType::POST_PROCESS_B])->GetColorTexture();
	}

	void RSBufferManager::CreateKernelUBO()
	{
    // UBO layout (std140):
    // float kernel[12] : 48 bytes (9 values + 3 padding for vec4 alignment)
    // float divisor    : 4 bytes
    // float offset     : 4 bytes
    // float padding[2] : 8 bytes
    // Total: 64 bytes

    glGenBuffers(1, &m_kernel_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, m_kernel_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 64, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Bind to binding point 0
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_kernel_ubo);
	}

	void RSBufferManager::UpdateKernelUBO(const float* kernel, const float divisor, const float offset)
	{
    // UBO layout (std140) - 64 bytes total:
    // vec4 kernel_row0 at offset 0   (kernel[0,1,2], padding)
    // vec4 kernel_row1 at offset 16  (kernel[3,4,5], padding)
    // vec4 kernel_row2 at offset 32  (kernel[6,7,8], padding)
    // vec4 params at offset 48       (divisor, offset, padding, padding)

    glBindBuffer(GL_UNIFORM_BUFFER, m_kernel_ubo);

    // Upload kernel rows as vec4
    float row0[4] = { kernel[0], kernel[1], kernel[2], 0.0f };
    float row1[4] = { kernel[3], kernel[4], kernel[5], 0.0f };
    float row2[4] = { kernel[6], kernel[7], kernel[8], 0.0f };
    float params[4] = { divisor, offset, 0.0f, 0.0f };

    glBufferSubData(GL_UNIFORM_BUFFER, 0,  16, row0);
    glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, row1);
    glBufferSubData(GL_UNIFORM_BUFFER, 32, 16, row2);
    glBufferSubData(GL_UNIFORM_BUFFER, 48, 16, params);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void RSBufferManager::ReleaseKernelUBO()
	{
    if (m_kernel_ubo != 0)
    {
      glDeleteBuffers(1, &m_kernel_ubo);
      m_kernel_ubo = 0;
    }
	}

	void RSBufferManager::InitializeConfiguration()
	{
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFAULT_OPAQUE);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "tex", 0);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "shadow_map", 1);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::MATERIAL_PBR);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "tex", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "normal_map", 1);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "shadow_map", 2);

		//DEFERRED_MATERIAL_PBR
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_MATERIAL_PBR);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_MATERIAL_PBR, "diffuse_map", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_MATERIAL_PBR, "normal_map", 1);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_PARTICLE_RENDER);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "tex0", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_PARTICLE_RENDER, "tex1", 1);
				
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::QUAD_DEBUG);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::QUAD_DEBUG, "depth_map", 0);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::QUAD_TEXTURE);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::QUAD_TEXTURE, "quad_texture", 0);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::QUAD_TEXTURE_R32);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::QUAD_TEXTURE_R32, "quad_r32_texture", 0);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::HUD_TEXTURE);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "hud_texture", 0);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::PARTICLE_RENDER);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER, "tex0", 0);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::PARTICLE_RENDER, "tex1", 1);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::POINT_CLIPPER);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::POINT_CLIPPER, "tex0", 0);

    RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::POINT_SINGLE_UINT_VTK);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::POINT_SINGLE_UINT_VTK, "tex0", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::POINT_SINGLE_UINT_VTK, "tex1", 1);

    RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::POINT_SINGLE_FLOAT_VTK);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::POINT_SINGLE_FLOAT_VTK, "tex0", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::POINT_SINGLE_FLOAT_VTK, "tex1", 1);

    RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::POINT_THREE_FLOAT_VTK);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::POINT_THREE_FLOAT_VTK, "tex0", 0);

		//RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_MATERIAL_PBR);
  //  RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_MATERIAL_PBR, "position_texture", 0);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_COMPOSITE);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "position_map", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "normal_map", 1);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "albedo_map", 2);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "fluid_position_map", 3);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "fluid_normal_map", 4);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "fluid_albedo_map", 5);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "custom_depth_map", 6);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "custom_index_map", 7);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "depth_map", 8);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "shadow_map", 9);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "ssao_map", 10);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "ssr_map", 11);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "skybox", 12);


		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_SSAO);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "position_map", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "normal_map", 1);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "fluid_position_map", 2);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "fluid_normal_map", 3);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "custom_mask_map", 4);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "noise_texture", 5);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_SSAO_BLUR);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO_BLUR, "ssao_texture", 0);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "position_map", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "normal_map", 1);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "albedo_map", 2);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "fluid_position_map", 3);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "fluid_normal_map", 4);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "fluid_albedo_map", 5);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "custom_depth_map", 6);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "custom_mask_map", 7);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "depth_map", 8);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION_BLUR);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION_BLUR, "ssr_texture", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION_BLUR, "normal_map", 1); // roughness
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION_BLUR, "fluid_normal_map", 2);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION_BLUR, "custom_mask_map", 3);

		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_POINT_SINGLE_FLOAT_VTK);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_POINT_SINGLE_FLOAT_VTK, "tex0", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_POINT_SINGLE_FLOAT_VTK, "tex1", 1);
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_POINT_SINGLE_UINT_VTK);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_POINT_SINGLE_UINT_VTK, "tex0", 0);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_POINT_SINGLE_UINT_VTK, "tex1", 1);
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::DEFERRED_POINT_THREE_FLOAT_VTK);
    RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::DEFERRED_POINT_THREE_FLOAT_VTK, "tex0", 0);

		// Post-process kernel filter
		RSResourceManager::GetInstance()->GetShaderManager()->Use(RSShaderNames::POST_KERNEL_FILTER);
		RSResourceManager::GetInstance()->GetShaderManager()->SetData(RSShaderNames::POST_KERNEL_FILTER, "input_texture", 0);





		RSResourceManager::GetInstance()->GetShaderManager()->UnbindShader();
	}

	void RSBufferManager::ReleaseAllBuffers()
	{

    if (quad_vao)
    glDeleteVertexArrays(1, &quad_vao);
    if (quad_vbo)
    glDeleteBuffers(1, &quad_vbo);
    if (skybox_vao)
    glDeleteVertexArrays(1, &skybox_vao);
    if (skybox_vbo)
    glDeleteBuffers(1, &skybox_vbo);

	}

	void RSBufferManager::ReleaseAllTextures()
	{

    if (skybox_texture_buffer)
      glDeleteTextures(1, &skybox_texture_buffer);

	}

	unsigned int RSBufferManager::LoadCubemap(const std::vector<std::string>& faces_)
	{
		unsigned int texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

		int width, height, nr_channels;
		for (unsigned int i = 0; i < faces_.size(); i++)
		{
			if (unsigned char* data = stbi_load(faces_[i].c_str(), &width, &height, &nr_channels, 0))
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				stbi_image_free(data);
			}
			else
			{
				RS_WARN("Cubemap texture failed to load at path: %s", faces_[i].c_str());
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 5);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		return texture_id;
	}


	void RSBufferManager::InitializeQuad()
	{
		if (quad_vao == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quad_vao);
			glGenBuffers(1, &quad_vbo);
			glBindVertexArray(quad_vao);
			glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
	}
	void RSBufferManager::DrawQuad() const
	{
		glBindVertexArray(quad_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void RSBufferManager::ResizeBuffers(const int width, const int height)
	{
		buffer_width = width;
		buffer_height = height;

		ResizeFboItems(width, height);
				
	}
}
