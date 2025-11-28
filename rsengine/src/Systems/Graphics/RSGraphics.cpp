#include "pch.h"
#include "RSGraphics.h"
#include "Resource/Manager/RSResourceManager.h"
#include "Util/RSSimplePainter.h"

namespace RS_Graphics
{
	//using RS_Mesh::RSMesh;
	using namespace RS_Object;
	RSGraphics* mRSGRAPHICS = nullptr;

	RSGraphics::RSGraphics()
	{
		RS_ASSERT(mRSGRAPHICS == nullptr, "Graphics class should be only one");
		mRSGRAPHICS = this;
	}

	RSGraphics::~RSGraphics() = default;


	void RSGraphics::Initialize()
	{
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_POINT_SPRITE);

		// m_rendering_flag includes shadow and deferred rendering and SSAO
    m_rendering_flag = RenderingFlag::SHADOW | RenderingFlag::OPAQUE_DEFERRED | RenderingFlag::SSAO | RenderingFlag::SSR;
	}

	void RSGraphics::PostInitialize()
	{
		p_resource_manager = RSResourceManager::GetInstance();
		RS_ASSERT(p_resource_manager != nullptr, "Resource Manager is not initialized");
	}

	void RSGraphics::Update(float dt)
	{
		RSGraphicsData* graphics_data = p_resource_manager->GetGraphicsData();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		// Clear the color buffer
		glClearColor(graphics_data->clear_color[0], graphics_data->clear_color[1], graphics_data->clear_color[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	  UpdateLight();

    // When rendering flag includes shadow, then draw shadow map. else shadow mapping change to default texture
    if (m_rendering_flag & RenderingFlag::SHADOW)
		  ShadowMapping();
		else
			if (b_rendering_flag_dirty)
        p_resource_manager->GetBufferManager()->ClearShadowMap();

		SetViewport(static_cast<int>(RS_Application::mRSAPPLICATION->GetWidth()),
							 static_cast<int>(RS_Application::mRSAPPLICATION->GetHeight()));

		HUDMapping();

	  HUDDraw();

		glClearColor(graphics_data->clear_color[0], graphics_data->clear_color[1], graphics_data->clear_color[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		Draw();

#if 0 // For DEBUGGING
		DrawPickingHUDDebug();
		HUDDrawDebug();
		DrawShadowMapDebug();
#endif
		b_rendering_flag_dirty = false;
	}

	void RSGraphics::UpdatePost(float )
	{
		glfwSwapBuffers(RS_Application::mRSAPPLICATION->GetWindow());
#ifdef _SPH_ENGINE
		glfwPollEvents();
#endif
	}

	void RSGraphics::Shutdown()
	{

	}

	void RSGraphics::SetRenderingFlag(const RenderingFlag flag_, const bool set_)
	{
    if (set_)
      AddRenderingFlag(flag_);
    else
      RemoveRenderingFlag(flag_);
		b_rendering_flag_dirty = true;
	}

	void RSGraphics::SetRenderingFlagDirty()
	{
		b_rendering_flag_dirty = true;
	}

	void RSGraphics::ClearBuffer()
	{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}


	void RSGraphics::Draw()
	{
		const auto buffer_manager = p_resource_manager->GetBufferManager();
		//StencilBufferObjectDraw();
		Render();
   // // Check Rendering Flag include deferred
   // if (m_rendering_flag & RenderingFlag::OPAQUE_DEFERRED)
   // {
   //   // Deferred Rendering
   //   DeferredRendering();
   // }
   // else
   // {
   //   // Forward Rendering
			//ForwardRendering();
   // }

		// Draw Final Fbo texture
		buffer_manager->DrawFinalFbo();



	}


	void RSGraphics::OpaqueDraw() const
	{
		// TODO : Update shader data when RSGraphicsData is changed only

    // When there is no object, return
		if (p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_OPAQUE).empty() &&
				p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE).empty())
			return;

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// Set Pipeline
		p_resource_manager->GetShaderManager()->Use(RSShaderNames::DEFAULT_OPAQUE);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "view",
                                                    p_resource_manager->GetCamera()->GetView());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "projection",
                                                    p_resource_manager->GetCamera()->GetProj());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "light_space_matrix",
                                                    p_resource_manager->GetGraphicsData()->light_space_matrix);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "view_pos",
                                                    p_resource_manager->GetCamera()->GetPosition());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "light_pos",
                                                    p_resource_manager->GetGraphicsData()->light_pos);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "opacity", 1.0f);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "use_transparent", false);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetShadowMap());
		p_resource_manager->GetObjectManager()->DrawModel(RSRenderLayer::R_SURFACE_OPAQUE, RSShaderNames::DEFAULT_OPAQUE);

    // When there is no wireframe object, return
		if (p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE).empty())
			return;

    // Wireframe Draw
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glDepthFunc(GL_LESS);
		p_resource_manager->GetObjectManager()->DrawModel(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE,
                                                      RSShaderNames::DEFAULT_OPAQUE);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDepthFunc(GL_LEQUAL);
	}

	void RSGraphics::OpaqueDrawPbr() const
	{
		if (p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_OPAQUE).empty() &&
				p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE).empty())
			return;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// Set Pipeline
		p_resource_manager->GetShaderManager()->Use(RSShaderNames::MATERIAL_PBR);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "view",
																										p_resource_manager->GetCamera()->GetView());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "projection",
																										p_resource_manager->GetCamera()->GetProj());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "light_space_matrix",
																										p_resource_manager->GetGraphicsData()->light_space_matrix);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "view_pos",
																										p_resource_manager->GetCamera()->GetPosition());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "light_pos",
																										p_resource_manager->GetGraphicsData()->light_pos);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "opacity", 1.0f);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "use_transparent", false);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetShadowMap());
		p_resource_manager->GetObjectManager()->DrawPbrModel(RSRenderLayer::R_SURFACE_OPAQUE, RSShaderNames::MATERIAL_PBR, true);


		// When there is no wireframe object, return
		if (p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE).empty())
			return;

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glDepthFunc(GL_LESS);
		p_resource_manager->GetObjectManager()->DrawPbrModel(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE,
																											RSShaderNames::MATERIAL_PBR, true);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDepthFunc(GL_LEQUAL);

	}

	void RSGraphics::LineDraw() const
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
		if (!p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_LINE_OPAQUE).empty())
		{
			//glLineWidth((GLfloat)10.0f);
			//GLfloat lineWidthRange[2];
			//glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
			//glEnable(GL_LINE_SMOOTH);
			//GLfloat currLineWidth[1] = {0.2f};
			//glGetFloatv(GL_LINE_WIDTH, currLineWidth);
			//GLfloat lineWidthRange[2] = { 300.0f, 50.0f };
			//glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
			//glLineWidth((GLfloat)30.0f);
			// Set Pipeline
			// glEnable ...
			// gl....
			p_resource_manager->GetShaderManager()->Use(RSShaderNames::DEFAULT_MVPLT);
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "view",
                                                      p_resource_manager->GetCamera()->GetView());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "projection",
                                                      p_resource_manager->GetCamera()->GetProj());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "light_dir",
                                                      p_resource_manager->GetGraphicsData()->light_pos);
			p_resource_manager->GetObjectManager()->DrawModel(RSRenderLayer::R_LINE_OPAQUE, RSShaderNames::DEFAULT_MVPLT);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void RSGraphics::WireFrameDraw() const
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (!p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE).empty())
		{
			//glEnable(GL_LINE_SMOOTH);
			// Set Pipeline
			// glEnable ...
			// gl....
			p_resource_manager->GetShaderManager()->Use(RSShaderNames::DEFAULT_MVPLT);
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "view",
                                                      p_resource_manager->GetCamera()->GetView());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "projection",
                                                      p_resource_manager->GetCamera()->GetProj());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "light_dir",
                                                      p_resource_manager->GetGraphicsData()->light_pos);
			p_resource_manager->GetObjectManager()->DrawWireFrameModel(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE,
                                                                 RSShaderNames::DEFAULT_MVPLT);

		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void RSGraphics::PointDrawDeferred() const
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		if (!p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_POINT).empty())
		{
			// Set Pipeline
			p_resource_manager->GetShaderManager()->Use(RSShaderNames::DEFAULT_MVPLT);
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "view",
                                                      p_resource_manager->GetCamera()->GetView());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "projection",
                                                      p_resource_manager->GetCamera()->GetProj());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "light_dir",
                                                      p_resource_manager->GetGraphicsData()->light_pos);
			p_resource_manager->GetObjectManager()->DrawModel(RSRenderLayer::R_POINT, RSShaderNames::DEFAULT_MVPLT);
		}

		// Particle Rendering
			//p_resource_manager->GetParticleManager()->Draw();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void RSGraphics::PointDrawForward() const
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		if (!p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_POINT).empty())
		{
			// Set Pipeline
			p_resource_manager->GetShaderManager()->Use(RSShaderNames::DEFAULT_MVPLT);
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "view",
																											p_resource_manager->GetCamera()->GetView());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "projection",
																											p_resource_manager->GetCamera()->GetProj());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "light_dir",
																											p_resource_manager->GetGraphicsData()->light_pos);
			p_resource_manager->GetObjectManager()->DrawModel(RSRenderLayer::R_POINT, RSShaderNames::DEFAULT_MVPLT);
		}

		// Particle Rendering
		//p_resource_manager->GetParticleManager()->Draw();

		p_resource_manager->GetParticleManager()->ForwardDraw();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void RSGraphics::DrawPickedObject() const
	{
		if (!p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_HIGHLIGHT).empty())
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			p_resource_manager->GetShaderManager()->Use(RSShaderNames::DEFAULT_MVPLT);
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "view",
                                                      p_resource_manager->GetCamera()->GetView());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "projection",
                                                      p_resource_manager->GetCamera()->GetProj());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVPLT, "light_dir",
                                                      p_resource_manager->GetGraphicsData()->light_pos);
			p_resource_manager->GetObjectManager()->DrawHighlightModel(RSRenderLayer::R_HIGHLIGHT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		}
	}
	
	void RSGraphics::TransparentDraw() const
	{

		if (p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_OPAQUE).size() +
				p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE).size() == 0)
			return;

		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDepthMask(GL_FALSE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_CW);
		//glPolygonMode(GL_FRONT, GL_FILL);

		p_resource_manager->GetShaderManager()->Use(RSShaderNames::DEFAULT_OPAQUE);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "view",
                                                    p_resource_manager->GetCamera()->GetView());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "projection",
                                                    p_resource_manager->GetCamera()->GetProj());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "light_space_matrix",
                                                    p_resource_manager->GetGraphicsData()->light_space_matrix);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "view_pos",
                                                    p_resource_manager->GetCamera()->GetPosition());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "light_pos",
                                                    p_resource_manager->GetGraphicsData()->light_pos);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "opacity", 1.0f);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "use_transparent", true);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetShadowMap());
		p_resource_manager->GetObjectManager()->DrawModel(RSRenderLayer::R_SURFACE_OPAQUE, RSShaderNames::DEFAULT_OPAQUE,
                                                      false);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glDepthFunc(GL_LESS);
		p_resource_manager->GetObjectManager()->DrawModel(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE,
                                                      RSShaderNames::DEFAULT_OPAQUE, false);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDepthFunc(GL_LEQUAL);

		glCullFace(GL_CCW);

		p_resource_manager->GetShaderManager()->Use(RSShaderNames::DEFAULT_OPAQUE);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "view",
                                                    p_resource_manager->GetCamera()->GetView());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "projection",
                                                    p_resource_manager->GetCamera()->GetProj());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "light_space_matrix",
                                                    p_resource_manager->GetGraphicsData()->light_space_matrix);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "view_pos",
                                                    p_resource_manager->GetCamera()->GetPosition());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "light_pos",
                                                    p_resource_manager->GetGraphicsData()->light_pos);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "opacity", 1.0f);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_OPAQUE, "use_transparent", true);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetShadowMap());
		p_resource_manager->GetObjectManager()->DrawModel(RSRenderLayer::R_SURFACE_OPAQUE, RSShaderNames::DEFAULT_OPAQUE,
                                                      false);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glDepthFunc(GL_LESS);
		p_resource_manager->GetObjectManager()->DrawModel(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE,
                                                      RSShaderNames::DEFAULT_OPAQUE, false);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDepthFunc(GL_LEQUAL);
		//glDepthMask(GL_TRUE);

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}
	
	void RSGraphics::TransparentDrawPbr() const
	{
		/*
		 *
		  if (p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_OPAQUE).empty() &&
				  p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE).empty())
			  return;

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		  // Set Pipeline
		  p_resource_manager->GetShaderManager()->Use(RSShaderNames::MATERIAL_PBR);
		  p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "view",
																										  p_resource_manager->GetCamera()->GetView());
		  p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "projection",
																										  p_resource_manager->GetCamera()->GetProj());
		  p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "light_space_matrix",
																										  p_resource_manager->GetGraphicsData()->light_space_matrix);
		  p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "view_pos",
																										  p_resource_manager->GetCamera()->GetPosition());
		  p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "light_pos",
																										  p_resource_manager->GetGraphicsData()->light_pos);
		  p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "opacity", 1.0f);
		  p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "use_transparent", false);

		  glActiveTexture(GL_TEXTURE2);
		  glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetShadowDepthMap());
		  p_resource_manager->GetObjectManager()->DrawPbrModel(RSRenderLayer::R_SURFACE_OPAQUE, RSShaderNames::MATERIAL_PBR);


		  // When there is no wireframe object, return
		  if (p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE).empty())
			  return;

		  glEnable(GL_POLYGON_OFFSET_FILL);
		  glPolygonOffset(1.0, 1.0);
		  glDepthFunc(GL_LESS);
		  p_resource_manager->GetObjectManager()->DrawPbrModel(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE,
																											  RSShaderNames::MATERIAL_PBR);
		  glDisable(GL_POLYGON_OFFSET_FILL);
		  glDepthFunc(GL_LEQUAL);
		 *
		 *
		 */
		if (p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_OPAQUE).empty() &&
				p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE).empty())
			return;

		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDepthMask(GL_FALSE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_CW);
		//glPolygonMode(GL_FRONT, GL_FILL);

		p_resource_manager->GetShaderManager()->Use(RSShaderNames::MATERIAL_PBR);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "view",
																										p_resource_manager->GetCamera()->GetView());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "projection",
																										p_resource_manager->GetCamera()->GetProj());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "light_space_matrix",
																										p_resource_manager->GetGraphicsData()->light_space_matrix);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "view_pos",
																										p_resource_manager->GetCamera()->GetPosition());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "light_pos",
																										p_resource_manager->GetGraphicsData()->light_pos);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "opacity", 1.0f);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "use_transparent", true);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetShadowMap());
		p_resource_manager->GetObjectManager()->DrawPbrModel(RSRenderLayer::R_SURFACE_OPAQUE, RSShaderNames::MATERIAL_PBR,
																											false);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glDepthFunc(GL_LESS);
		p_resource_manager->GetObjectManager()->DrawPbrModel(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE,
																											RSShaderNames::MATERIAL_PBR, false);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDepthFunc(GL_LEQUAL);

		glCullFace(GL_CCW);

		p_resource_manager->GetShaderManager()->Use(RSShaderNames::MATERIAL_PBR);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "view",
																										p_resource_manager->GetCamera()->GetView());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "projection",
																										p_resource_manager->GetCamera()->GetProj());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "light_space_matrix",
																										p_resource_manager->GetGraphicsData()->light_space_matrix);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "view_pos",
																										p_resource_manager->GetCamera()->GetPosition());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "light_pos",
																										p_resource_manager->GetGraphicsData()->light_pos);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "opacity", 1.0f);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::MATERIAL_PBR, "use_transparent", true);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetShadowMap());
		p_resource_manager->GetObjectManager()->DrawPbrModel(RSRenderLayer::R_SURFACE_OPAQUE, RSShaderNames::MATERIAL_PBR,
																											false);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		glDepthFunc(GL_LESS);
		p_resource_manager->GetObjectManager()->DrawPbrModel(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE,
																											RSShaderNames::MATERIAL_PBR, false);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glDepthFunc(GL_LEQUAL);
		//glDepthMask(GL_TRUE);

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}

	void RSGraphics::HUDDraw()
	{
		// Set visible HUD
		SetViewport(static_cast<int>(RS_Application::mRSAPPLICATION->GetWidth()),
							 static_cast<int>(RS_Application::mRSAPPLICATION->GetHeight()));
		//p_resource_manager->GetBufferManager()->BindHudFrameBuffer();
		p_resource_manager->GetBufferManager()->BindFbo(FboType::HUD);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		// Draw HUD
			glActiveTexture(GL_TEXTURE0);
				p_resource_manager->GetShaderManager()->Use(RSShaderNames::HUD_TEXTURE);
				p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_TEXTURE, "projection",
                                                        glm::ortho(0.0f, static_cast<float>(
                                                                     RS_Application::mRSAPPLICATION->GetWidth()), 0.0f,
                                                                   static_cast<float>(RS_Application::mRSAPPLICATION->
                                                                     GetHeight()), -100.0f,
                                                                   100.0f)); // For GIZMO, near & far expand to 100.0f
				p_resource_manager->GetWidgetManager()->VisibleDraw();
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);

		//unbind hud buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);
	}



	void RSGraphics::SkyboxDraw() const
	{
		p_resource_manager->GetShaderManager()->Use(RSShaderNames::SKYBOX);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::SKYBOX, "view",
                                                    glm::mat4(glm::mat3(p_resource_manager->GetCamera()->GetView())));
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::SKYBOX, "projection",
                                                    p_resource_manager->GetCamera()->GetPerspectiveProjection());
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::SKYBOX, "alpha",
																										0.7f);
                                                    //(-p_resource_manager->GetGraphicsData()->skybox_timer_value) *
                                                    //0.02f);

		//glDisable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		// skybox cube
		glBindVertexArray(p_resource_manager->GetBufferManager()->GetSkyboxVAO());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, p_resource_manager->GetBufferManager()->GetSkyboxTextureBuffer());
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LEQUAL); // set depth function back to default
		p_resource_manager->GetShaderManager()->UnbindShader();

	}
	void RSGraphics::ShadowMapping()
	{
		if (p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_OPAQUE).empty())
			return;

		p_resource_manager->GetShaderManager()->Use(RSShaderNames::SHADOW_MAPPING);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::SHADOW_MAPPING, "light_space_matrix",
                                                    p_resource_manager->GetGraphicsData()->light_space_matrix);
		p_resource_manager->GetBufferManager()->BindFbo(FboType::SHADOW_MAP);

		//glViewport(0, 0, static_cast<int>(p_resource_manager->GetBufferManager()->GetShadowWidth()), 
  //             static_cast<int>(p_resource_manager->GetBufferManager()->GetShadowHeight()));
		//glBindFramebuffer(GL_FRAMEBUFFER, p_resource_manager->GetBufferManager()->GetShadowTextureBuffer());
		//	glClear(GL_DEPTH_BUFFER_BIT);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			p_resource_manager->GetObjectManager()->DrawModel(RSRenderLayer::R_SURFACE_OPAQUE, RSShaderNames::SHADOW_MAPPING);

			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			p_resource_manager->GetBufferManager()->UnbindFbo(FboType::SHADOW_MAP);

	}
	void RSGraphics::HUDMapping() const
	{
		// Set picking HUD
		//p_resource_manager->GetBufferManager()->BindPickingFrameBuffer();
		p_resource_manager->GetBufferManager()->BindFbo(FboType::PICKING);

		  p_resource_manager->GetShaderManager()->Use(RSShaderNames::HUD_MAPPING);

    // TODO : Check if this is necessary
		//{
		//  // Prepare 3D Picking HUD
		//  p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_MAPPING, "view",
  //                                                    p_resource_manager->GetCamera()->GetView());
		//  p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_MAPPING, "projection",
  //                                                    p_resource_manager->GetCamera()->GetProj());
		//}


		// Prepare 2D Picking HUD
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_MAPPING, "view", glm::mat4(1.0f));
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::HUD_MAPPING, "projection",
                                                    glm::ortho(0.0f, static_cast<float>(RS_Application::mRSAPPLICATION->
                                                                 GetWidth()), 0.0f,
                                                               static_cast<float>(RS_Application::mRSAPPLICATION->
                                                                 GetHeight()), -100.0f, 100.0f));

		// Draw Picking HUD
		p_resource_manager->GetWidgetManager()->HittableDraw();


		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		p_resource_manager->GetBufferManager()->UnbindFbo(FboType::PICKING);
	}

	void RSGraphics::StencilBufferObjectDraw() const
	{
		if (p_resource_manager->GetGraphicsData()->is_use_stencil_buffer)
		{
			// find "grid" object
			RSObject* p_grid = p_resource_manager->GetObjectManager()->GetObject("StencilGrid");
			if (!p_grid)
				return;
			const auto p_model = dynamic_cast<RSModel*>(p_grid->GetComponent(RSComponentType::CT_MODEL));
			if (p_model == nullptr) return;
			if (p_model->GetMesh() == nullptr) return;
			if (p_model->GetIsVisible() == false) return;


			// Use stencil buffer system
			glEnable(GL_STENCIL_TEST);
			// Clear stencil buffer
			glClearStencil(0);

			// Set stencil buffer
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			
			glClear(GL_STENCIL_BUFFER_BIT);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable color buffer writing
			glDepthMask(GL_FALSE); // Disable depth buffer writing
			//glDepthFunc(GL_GREATER); // Set depth function to less or equal

			// Draw Grid
			p_resource_manager->GetShaderManager()->Use(RSShaderNames::DEFAULT_MVP);
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVP, "view",
                                                      p_resource_manager->GetCamera()->GetView());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVP, "projection",
                                                      p_resource_manager->GetCamera()->GetProj());
			p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFAULT_MVP, "model", p_grid->GetTransformMatrix());

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			p_model->Draw();

			// Set stencil buffer
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Enable color buffer writing
			glDepthMask(GL_TRUE); // Enable depth buffer writing
			//glDepthFunc(GL_LEQUAL); // Set depth function to less or equal

			// Set stencil buffer
			glStencilFunc(GL_EQUAL, 1, 0xFF);
			glStencilMask(0x00); // Disable writing to the stencil buffer
		}
	}

	void RSGraphics::StencilBufferFinish() const
	{
		if (p_resource_manager->GetGraphicsData()->is_use_stencil_buffer)
		{
			glDisable(GL_STENCIL_TEST);
		}
	}

	void RSGraphics::UpdateLight() const
	{
    auto graphics_data = p_resource_manager->GetGraphicsData();

		if (graphics_data->b_light_dirty)
		{
			constexpr float map_size = 30.0f;
			constexpr float shadow_near_plane = 0.01f;
			constexpr float shadow_far_plane = 100.0f;

			p_resource_manager->GetGraphicsData()->light_projection = glm::ortho(-map_size, map_size, -map_size, map_size,
																																					 shadow_near_plane, shadow_far_plane);

			p_resource_manager->GetGraphicsData()->light_view = glm::lookAt(p_resource_manager->GetGraphicsData()->light_pos,
																																			glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			p_resource_manager->GetGraphicsData()->light_space_matrix =
			          p_resource_manager->GetGraphicsData()->light_projection * p_resource_manager->GetGraphicsData()->light_view;

      graphics_data->b_light_dirty = false;
		}

	}

	void RSGraphics::DeferredRendering()
	{
		const auto buffer_manager = p_resource_manager->GetBufferManager();
		// Bind G Buffer
		buffer_manager->BindFbo(FboType::DEFERRED);

		ClearBuffer();

		// Draw Opaque to G Buffer
		DrawOpaqueToGBuffer();

		if (m_rendering_flag & RenderingFlag::FLUID_DEFERRED)
		{
		  // Draw Particle Simulator Point to G Buffer
		  DrawParticleSimulatorPointToGBuffer();
		}


		// Unbind G Buffer
		buffer_manager->UnbindFbo(FboType::DEFERRED);

		// ssao pass
		if (m_rendering_flag & RenderingFlag::SSAO)
		{
			DrawSSAO();
			DrawSSAOBlur();
		}
		else
			if (b_rendering_flag_dirty)
				ClearSSAO();

		// SSR pass
    if (m_rendering_flag & RenderingFlag::SSR)
    {
      DrawSSR();
			DrawSSRBlur();
    }
    else
      if (b_rendering_flag_dirty)
        ClearSSR();

		buffer_manager->BindFbo(FboType::FINAL);

		ClearBuffer();

		CompositeGBuffer();

		LineDraw();

		WireFrameDraw();

		/*if (m_rendering_flag & RenderingFlag::FLUID_DEFERRED)
		  PointDrawDeferred();
    else
      PointDrawForward();*/

    m_rendering_flag& RenderingFlag::FLUID_DEFERRED ?
			PointDrawDeferred() :
			PointDrawForward();

		TransparentDrawPbr();

		DrawPickedObject();

		if (m_rendering_flag & RenderingFlag::SKYBOX)
			SkyboxDraw();

		CompositeHUD();
		buffer_manager->UnbindFbo(FboType::FINAL);
	}

	void RSGraphics::ForwardRendering()
	{
		const auto buffer_manager = p_resource_manager->GetBufferManager();
		buffer_manager->BindFbo(FboType::FINAL);
		ClearBuffer();

		OpaqueDrawPbr();

		LineDraw();

		WireFrameDraw();

		PointDrawForward();

		TransparentDrawPbr();

		DrawPickedObject();

		if (m_rendering_flag & RenderingFlag::SKYBOX)
			SkyboxDraw();

		CompositeHUD();
		buffer_manager->UnbindFbo(FboType::FINAL);
	}

	void RSGraphics::Render()
	{
		// Done : Shadow mapping and HUD mapping

		// Bind G Buffer
		// clear
	  // if(opaque deferred flag) draw opaque object with deferred

    // if(fluid deferred flag) draw fluid object with deferred
    // unbind

    // Composite G Buffer. PBR, lighting

    // if(not opaque deferred flag) draw opaque object with forward
    // if(not fluid deferred flag) draw fluid object with forward

    /* OIT Transparency rendering was not prepared yet. So Draw transparent object with forward now.
    // Bind Transparent Deferred Buffer (with OIT rendering)
    // Draw transparent object with OIT
		// unbind
		*/

    // Post-processing with SSAO, SSR and reflection.
    // Consider to plan DoF, Outline, Bloom and other effects

		// Draw Wireframe / line

		// Draw picked object line

		// Draw skybox


		// Draw HUD


		const auto buffer_manager = p_resource_manager->GetBufferManager();
		// Bind G Buffer
		buffer_manager->BindFbo(FboType::DEFERRED);

		ClearBuffer();

		// Draw Opaque to G Buffer
		if (m_rendering_flag & RenderingFlag::OPAQUE_DEFERRED)
			DrawOpaqueToGBuffer();

		// Draw Particle Simulator Point to G Buffer
		if (m_rendering_flag & RenderingFlag::FLUID_DEFERRED)
			DrawParticleSimulatorPointToGBuffer();

		// Unbind G Buffer
		buffer_manager->UnbindFbo(FboType::DEFERRED);

		// Post-processing
		{
		  // ssao pass
		  if (m_rendering_flag & RenderingFlag::SSAO)
		  {
			  DrawSSAO();
			  DrawSSAOBlur();
		  }
		  else
			  if (b_rendering_flag_dirty)
				  ClearSSAO();

		  // SSR pass
		  if (m_rendering_flag & RenderingFlag::SSR)
		  {
			  DrawSSR();
			  DrawSSRBlur();
		  }
		  else
			  if (b_rendering_flag_dirty)
				  ClearSSR();
		}

		buffer_manager->BindFbo(FboType::FINAL);

		ClearBuffer();

		CompositeGBuffer();

		if (!(m_rendering_flag & RenderingFlag::OPAQUE_DEFERRED))
			OpaqueDrawPbr();


		LineDraw();

		WireFrameDraw();

		m_rendering_flag& RenderingFlag::FLUID_DEFERRED ?
			PointDrawDeferred() :
			PointDrawForward();

		TransparentDrawPbr();

		DrawPickedObject();

		// Unbind FINAL FBO before post-processing (Scene only, Depth preserved)
		buffer_manager->UnbindFbo(FboType::FINAL);

		// Color filter post-processing (applied BEFORE kernel filter)
		// Applies grayscale, channel isolation, sepia, invert effects
		if (m_rendering_flag & RenderingFlag::COLOR_FILTER)
		{
			DrawColorFilter();
		}
		else if (b_rendering_flag_dirty)
		{
			ClearColorFilter();
		}

		// Kernel-based image post-processing (Scene only, excludes Skybox & HUD)
		// Note: Only GL_COLOR_BUFFER_BIT is cleared, Depth Buffer is preserved
		if (m_rendering_flag & RenderingFlag::IMAGE_KERNEL)
		{
			DrawKernelPostProcess();
		}
		else if (b_rendering_flag_dirty)
		{
			ClearKernelPostProcess();
		}

		// Re-bind FINAL FBO to draw Skybox and HUD (after post-processing)
		buffer_manager->BindFbo(FboType::FINAL);

		// Skybox uses GL_LEQUAL depth test, draws only where depth == 1.0
		if (m_rendering_flag & RenderingFlag::SKYBOX)
			SkyboxDraw();

		// HUD is composited with alpha blending (no depth test)
		CompositeHUD();

		buffer_manager->UnbindFbo(FboType::FINAL);

	}

	void RSGraphics::DrawShadowMapDebug() const
	{
		p_resource_manager->GetShaderManager()->Use(RSShaderNames::QUAD_DEBUG);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::QUAD_DEBUG, "near_plane", 0.01f);
		p_resource_manager->GetShaderManager()->SetData(RSShaderNames::QUAD_DEBUG, "far_plane", 100.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetShadowMap());
		p_resource_manager->GetBufferManager() ->DrawQuad(); 
	}

	void RSGraphics::HudDrawDebug() const
	{
		p_resource_manager->GetShaderManager()->Use(RSShaderNames::QUAD_TEXTURE);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetHudTexture());
		p_resource_manager->GetBufferManager()->DrawQuad();
	}

	void RSGraphics::SetViewport(const int width, const int height) const
	{
		glViewport(0, 0, width, height);
	}

	void RSGraphics::DrawPickingHudDebug() const
	{
		p_resource_manager->GetShaderManager()->Use(RSShaderNames::QUAD_TEXTURE_R32);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetPickingTexture());
		p_resource_manager->GetBufferManager()->DrawQuad();
	}

	void RSGraphics::CompositeHUD() const
	{
		p_resource_manager->GetShaderManager()->Use(RSShaderNames::QUAD_TEXTURE);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, p_resource_manager->GetBufferManager()->GetHudTexture());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			p_resource_manager->GetBufferManager()->DrawQuad();
			
		glDisable(GL_BLEND);
	}

	void RSGraphics::DrawOpaqueToGBuffer()
	{
    // Set Pipeline for G Buffer
    p_resource_manager->GetShaderManager()->Use(RSShaderNames::DEFERRED_MATERIAL_PBR);
    p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFERRED_MATERIAL_PBR, "view",
                                                    p_resource_manager->GetCamera()->GetView());
    p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFERRED_MATERIAL_PBR, "projection",
                                                    p_resource_manager->GetCamera()->GetProj());
		// near and far
    p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFERRED_MATERIAL_PBR, "near",
                                                    p_resource_manager->GetCamera()->GetNear());
    p_resource_manager->GetShaderManager()->SetData(RSShaderNames::DEFERRED_MATERIAL_PBR, "far",
                                                    p_resource_manager->GetCamera()->GetFar());


    // Draw Opaque
    if (!p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_OPAQUE).empty())
      p_resource_manager->GetObjectManager()->DrawPbrModel(RSRenderLayer::R_SURFACE_OPAQUE, RSShaderNames::DEFERRED_MATERIAL_PBR, true);

		// Draw Opaque wireframe
		if (!p_resource_manager->GetObjectManager()->GetObjectLayers(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE).empty())
		{

			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			glDepthFunc(GL_LESS);
			p_resource_manager->GetObjectManager()->DrawPbrModel(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE, RSShaderNames::DEFERRED_MATERIAL_PBR, true);

			glDisable(GL_POLYGON_OFFSET_FILL);
			glDepthFunc(GL_LEQUAL);
		}

	}
	void RSGraphics::DrawParticleSimulatorPointToGBuffer()
	{
		// Point Draw
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		// Particle Rendering
		p_resource_manager->GetParticleManager()->DeferredDraw();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		p_resource_manager->GetShaderManager()->UnbindShader();
	}
	void RSGraphics::CompositeGBuffer()
	{
		const auto rm = p_resource_manager;
		const auto bm = rm->GetBufferManager();

		const auto gd = p_resource_manager->GetGraphicsData();
    const auto bg = glm::vec3(gd->clear_color[0], gd->clear_color[1], gd->clear_color[2]);

		rm->GetShaderManager()->Use(RSShaderNames::DEFERRED_COMPOSITE);
		rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "light_space_matrix",
																								rm->GetGraphicsData()->light_space_matrix);
		rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "view_pos",
																										rm->GetCamera()->GetPosition());
		rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "light_pos",
																								rm->GetGraphicsData()->light_pos);
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "background_color",
                                                bg);
		rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "b_skybox_reflection",
																								m_rendering_flag & RenderingFlag::SKYBOX);
		rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "b_ssr_reflection",
																								m_rendering_flag & RenderingFlag::SSR);
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_COMPOSITE, "use_fluid_map",
                                                m_rendering_flag & RenderingFlag::FLUID_DEFERRED);

    const auto deferred_fbo = dynamic_cast<_RS_Internal::RSDeferredFbo*>(bm->GetFboItem(FboType::DEFERRED));
    const auto shadow_map_fbo = dynamic_cast<_RS_Internal::RSShadowMapFbo*>(bm->GetFboItem(FboType::SHADOW_MAP));
    const auto ssbo_blur_fbo = dynamic_cast<_RS_Internal::RSSsaoBlurFbo*>(bm->GetFboItem(FboType::SSAO_BLUR));
    const auto ssr_fbo = dynamic_cast<_RS_Internal::RSSsrFbo*>(bm->GetFboItem(FboType::SSR));
    const auto ssr_blur_fbo = dynamic_cast<_RS_Internal::RSSsrBlurFbo*>(bm->GetFboItem(FboType::SSR_BLUR));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, deferred_fbo->GetDeferredPositionTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, deferred_fbo->GetDeferredNormalTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, deferred_fbo->GetDeferredAlbedoTexture());
    glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, deferred_fbo->GetFluidDeferredPositionTexture());
		glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, deferred_fbo->GetFluidDeferredNormalTexture());
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, deferred_fbo->GetFluidDeferredAlbedoTexture());
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, deferred_fbo->GetCustomDeferredDepthTexture());
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, deferred_fbo->GetCustomMaskTexture());
		glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, deferred_fbo->GetDepthTexture());
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, shadow_map_fbo->GetShadowMapTexture());
		glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, ssbo_blur_fbo->GetSsaoBlurTexture());
		glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, ssr_blur_fbo->GetSsrBlurTexture());
		// Skybox
		if (m_rendering_flag & RenderingFlag::SKYBOX)
		{
      glActiveTexture(GL_TEXTURE12);
      glBindTexture(GL_TEXTURE_CUBE_MAP, bm->GetSkyboxTextureBuffer());
		}

		bm->DrawQuad();

		rm->GetShaderManager()->UnbindShader();

    const int width = static_cast<int>(RS_Application::mRSAPPLICATION->GetWidth());
    const int height = static_cast<int>(RS_Application::mRSAPPLICATION->GetHeight());

		//copy content of geometry's depth buffer to default framebuffer's depth buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, dynamic_cast<_RS_Internal::RSDeferredFbo*>(bm->GetFboItem(FboType::DEFERRED))->GetDeferredFrameBuffer());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dynamic_cast<_RS_Internal::RSFinalFbo*>(bm->GetFboItem(FboType::FINAL))->GetFinalFrameBuffer()); // write to default framebuffer
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, dynamic_cast<_RS_Internal::RSFinalFbo*>(bm->GetFboItem(FboType::FINAL))->GetFinalFrameBuffer());

		
	}

	void RSGraphics::DrawSSAO()
	{
    const auto rm = p_resource_manager;
    const auto bm = rm->GetBufferManager();
    const auto ssao_fbo = dynamic_cast<_RS_Internal::RSSsaoFbo*>(bm->GetFboItem(FboType::SSAO));
    const float screen_width = static_cast<float>(RS_Application::mRSAPPLICATION->GetWidth());
    const float screen_height = static_cast<float>(RS_Application::mRSAPPLICATION->GetHeight());

    // Bind SSAO FBO
    bm->BindFbo(FboType::SSAO);

		glClear(GL_COLOR_BUFFER_BIT);

    rm->GetShaderManager()->Use(RSShaderNames::DEFERRED_SSAO);
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "projection",
                                                rm->GetCamera()->GetProj());
		rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "view",
																								rm->GetCamera()->GetView());
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "noise_texture", ssao_fbo->GetNoiseTexture());
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "screen_size", glm::vec2(screen_width, screen_height));
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "use_fluid_map", m_rendering_flag & RenderingFlag::FLUID_DEFERRED);

		if (ssao_fbo->IsDirty())
		{
		  for (unsigned int i = 0; i < 64; ++i)
		    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "samples[" + std::to_string(i) + "]", ssao_fbo->GetKernel()->at(i));
      rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "bias", ssao_fbo->GetBias());
      rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SSAO, "radius", ssao_fbo->GetRadius());
		  ssao_fbo->SetDirty(false);
		}

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, dynamic_cast<_RS_Internal::RSDeferredFbo*>(bm->GetFboItem(FboType::DEFERRED))->GetDeferredPositionTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, dynamic_cast<_RS_Internal::RSDeferredFbo*>(bm->GetFboItem(FboType::DEFERRED))->GetDeferredNormalTexture());
		if (m_rendering_flag & RenderingFlag::FLUID_DEFERRED)
		{
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, dynamic_cast<_RS_Internal::RSDeferredFbo*>(bm->GetFboItem(FboType::DEFERRED))->GetFluidDeferredPositionTexture());
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D, dynamic_cast<_RS_Internal::RSDeferredFbo*>(bm->GetFboItem(FboType::DEFERRED))->GetFluidDeferredNormalTexture());
      glActiveTexture(GL_TEXTURE4);
      glBindTexture(GL_TEXTURE_2D, dynamic_cast<_RS_Internal::RSDeferredFbo*>(bm->GetFboItem(FboType::DEFERRED))->GetCustomMaskTexture());
		}
		glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, ssao_fbo->GetNoiseTexture());
    bm->DrawQuad();

		// unbind ssao
    bm->UnbindFbo(FboType::SSAO);

	}

	void RSGraphics::DrawSSAOBlur()
	{
    const auto rm = p_resource_manager;
    const auto bm = rm->GetBufferManager();
    const auto ssao_fbo = dynamic_cast<_RS_Internal::RSSsaoFbo*>(bm->GetFboItem(FboType::SSAO));
    // Bind SSAO FBO
    bm->BindFbo(FboType::SSAO_BLUR);
    glClear(GL_COLOR_BUFFER_BIT);
    rm->GetShaderManager()->Use(RSShaderNames::DEFERRED_SSAO_BLUR);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssao_fbo->GetSsaoTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ssao_fbo->GetNoiseTexture());
    bm->DrawQuad();

    // unbind ssao
    bm->UnbindFbo(FboType::SSAO_BLUR);
	}

	void RSGraphics::ClearSSAO()
	{
		const auto buffer_manager = p_resource_manager->GetBufferManager();
		buffer_manager->BindFbo(FboType::SSAO_BLUR);
		// Set background color to white
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		buffer_manager->UnbindFbo(FboType::SSAO_BLUR);
	}

	void RSGraphics::DrawSSR()
	{
    const auto rm = p_resource_manager;
    const auto bm = rm->GetBufferManager();
    const auto gm = rm->GetGraphicsData();
    const auto ssr_fbo = dynamic_cast<_RS_Internal::RSSsrFbo*>(bm->GetFboItem(FboType::SSR));
		const auto deferred_map = dynamic_cast<_RS_Internal::RSDeferredFbo*>(bm->GetFboItem(FboType::DEFERRED));

		// Bind SSR FBO
    bm->BindFbo(FboType::SSR);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
    rm->GetShaderManager()->Use(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION);
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "proj",
                                                rm->GetCamera()->GetProj());
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "view",
                                                rm->GetCamera()->GetView());
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "cam_pos",
                                                rm->GetCamera()->GetPosition());
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "near",
                                                rm->GetCamera()->GetNear());
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "far",
                                                rm->GetCamera()->GetFar());
		rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "g_max_thickness",
																						      gm->g_max_thickness);
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "g_depthbias",
                                                 gm->g_depthbias);
		//screen_size
		rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "screen_size",
                                                glm::vec2(static_cast<float>(RS_Application::mRSAPPLICATION->GetWidth()),
                                                  static_cast<float>(RS_Application::mRSAPPLICATION->GetHeight())));
		// b use fluid map
    rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION, "use_fluid_map",
                                                m_rendering_flag & RenderingFlag::FLUID_DEFERRED);

		// textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, deferred_map->GetDeferredPositionTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, deferred_map->GetDeferredNormalTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, deferred_map->GetDeferredAlbedoTexture());
    glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, deferred_map->GetFluidDeferredPositionTexture());
		glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, deferred_map->GetFluidDeferredNormalTexture());
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, deferred_map->GetFluidDeferredAlbedoTexture());
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, deferred_map->GetCustomDeferredDepthTexture());
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, deferred_map->GetCustomMaskTexture());
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, deferred_map->GetDepthTexture());

    bm->DrawQuad();

		// unbind
    bm->UnbindFbo(FboType::SSR);
	}

	void RSGraphics::DrawSSRBlur()
	{
    const auto rm = p_resource_manager;
    const auto bm = rm->GetBufferManager();
    const auto g_blur_coefficient = rm->GetGraphicsData()->g_blur_coefficient;
    const auto ssr_fbo = dynamic_cast<_RS_Internal::RSSsrFbo*>(bm->GetFboItem(FboType::SSR));
    const auto deferred_map = dynamic_cast<_RS_Internal::RSDeferredFbo*>(bm->GetFboItem(FboType::DEFERRED));
    // Bind SSR FBO
    bm->BindFbo(FboType::SSR_BLUR);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    rm->GetShaderManager()->Use(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION_BLUR);

		rm->GetShaderManager()->SetData(RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION_BLUR, "blur_coefficient", g_blur_coefficient);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssr_fbo->GetSsrTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, deferred_map->GetDeferredNormalTexture());
		glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, deferred_map->GetFluidDeferredNormalTexture());
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, deferred_map->GetCustomMaskTexture());

    bm->DrawQuad();
    // unbind
    bm->UnbindFbo(FboType::SSR_BLUR);
	}

	void RSGraphics::ClearSSR()
	{
    const auto buffer_manager = p_resource_manager->GetBufferManager();
    buffer_manager->BindFbo(FboType::SSR);
    // Set background color to white
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    buffer_manager->UnbindFbo(FboType::SSR);
	}

	void RSGraphics::DrawKernelPostProcess()
	{
    const auto rm = p_resource_manager;
    const auto bm = rm->GetBufferManager();

    // Update UBO if kernel data is dirty
    if (m_kernel_data.is_dirty)
    {
      bm->UpdateKernelUBO(m_kernel_data.kernel, m_kernel_data.divisor, m_kernel_data.offset);
      m_kernel_data.is_dirty = false;
    }

    // Get FBOs
    const auto final_fbo = dynamic_cast<_RS_Internal::RSFinalFbo*>(bm->GetFboItem(FboType::FINAL));
    const auto post_a_fbo = dynamic_cast<_RS_Internal::RSPostProcessFbo*>(bm->GetFboItem(FboType::POST_PROCESS_A));
    const auto post_b_fbo = dynamic_cast<_RS_Internal::RSPostProcessFbo*>(bm->GetFboItem(FboType::POST_PROCESS_B));

    // Clamp pass count to 1-4
    const int pass_count = glm::clamp(m_kernel_data.pass_count, 1, 4);

    rm->GetShaderManager()->Use(RSShaderNames::POST_KERNEL_FILTER);

    // Disable depth test for full-screen quad rendering
    glDisable(GL_DEPTH_TEST);

    // Ping-pong rendering pattern:
    // Pass 0: FINAL -> POST_A
    // Pass 1: POST_A -> POST_B
    // Pass 2: POST_B -> POST_A
    // Pass 3: POST_A -> POST_B
    // Final copy: last output -> FINAL

    unsigned int input_texture = final_fbo->GetFinalTexture();
    FboType last_output_fbo = FboType::POST_PROCESS_A;

    for (int pass = 0; pass < pass_count; ++pass)
    {
      // Determine output FBO (alternate between A and B)
      FboType output_fbo_type = (pass % 2 == 0) ? FboType::POST_PROCESS_A : FboType::POST_PROCESS_B;
      last_output_fbo = output_fbo_type;

      bm->BindFbo(output_fbo_type);
      glClear(GL_COLOR_BUFFER_BIT);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, input_texture);

      bm->DrawQuad();

      bm->UnbindFbo(output_fbo_type);

      // Set input for next pass
      input_texture = (pass % 2 == 0) ? post_a_fbo->GetColorTexture() : post_b_fbo->GetColorTexture();
    }

    // Final copy: POST_PROCESS -> FINAL (using simple texture copy shader)
    rm->GetShaderManager()->Use(RSShaderNames::QUAD_TEXTURE);
    bm->BindFbo(FboType::FINAL);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    unsigned int final_input = (last_output_fbo == FboType::POST_PROCESS_A) 
                               ? post_a_fbo->GetColorTexture() 
                               : post_b_fbo->GetColorTexture();
    glBindTexture(GL_TEXTURE_2D, final_input);

    bm->DrawQuad();

    bm->UnbindFbo(FboType::FINAL);

    // Re-enable depth test
    glEnable(GL_DEPTH_TEST);

    rm->GetShaderManager()->UnbindShader();
	}

	void RSGraphics::ClearKernelPostProcess()
	{
    const auto buffer_manager = p_resource_manager->GetBufferManager();

    // Clear POST_PROCESS_A
    buffer_manager->BindFbo(FboType::POST_PROCESS_A);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    buffer_manager->UnbindFbo(FboType::POST_PROCESS_A);

    // Clear POST_PROCESS_B
    buffer_manager->BindFbo(FboType::POST_PROCESS_B);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    buffer_manager->UnbindFbo(FboType::POST_PROCESS_B);
	}

	void RSGraphics::DrawColorFilter()
	{
    const auto rm = p_resource_manager;
    const auto bm = rm->GetBufferManager();

    // Update UBO if color filter data is dirty
    if (m_color_filter_data.is_dirty)
    {
      bm->UpdateColorFilterUBO(
        static_cast<int>(m_color_filter_data.mode),
        m_color_filter_data.weights,
        m_color_filter_data.intensity
      );
      m_color_filter_data.is_dirty = false;
    }

    // Skip if mode is NONE
    if (m_color_filter_data.mode == ColorFilterMode::NONE)
      return;

    // Get FBOs
    const auto final_fbo = dynamic_cast<_RS_Internal::RSFinalFbo*>(bm->GetFboItem(FboType::FINAL));
    const auto post_a_fbo = dynamic_cast<_RS_Internal::RSPostProcessFbo*>(bm->GetFboItem(FboType::POST_PROCESS_A));

    rm->GetShaderManager()->Use(RSShaderNames::POST_COLOR_FILTER);

    // Disable depth test for full-screen quad rendering
    glDisable(GL_DEPTH_TEST);

    // Single pass: FINAL -> POST_A -> FINAL
    // Pass 1: Apply color filter to POST_A
    bm->BindFbo(FboType::POST_PROCESS_A);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, final_fbo->GetFinalTexture());

    bm->DrawQuad();

    bm->UnbindFbo(FboType::POST_PROCESS_A);

    // Pass 2: Copy back to FINAL
    rm->GetShaderManager()->Use(RSShaderNames::QUAD_TEXTURE);
    bm->BindFbo(FboType::FINAL);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, post_a_fbo->GetColorTexture());

    bm->DrawQuad();

    bm->UnbindFbo(FboType::FINAL);

    // Re-enable depth test
    glEnable(GL_DEPTH_TEST);

    rm->GetShaderManager()->UnbindShader();
	}

	void RSGraphics::ClearColorFilter()
	{
    // Color filter uses same FBOs as kernel filter, no separate clearing needed
    // The UBO state is managed by is_dirty flag
	}

} // namespace rs
