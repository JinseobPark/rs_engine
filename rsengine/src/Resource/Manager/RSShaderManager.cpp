#include "pch.h"
#include "RSShaderManager.h"
#include "../Pipeline/RSMergedShaders.h"

namespace RS_Shader
{
	RSShaderManager::~RSShaderManager() = default;

	void RSShaderManager::Initialize()
	{


		/****************************** Default RSShader  ******************************/
		const auto default_shader = new RSShader(default_vs, default_fs, false);
		shaders_map_[RSShaderNames::DEFAULT] = default_shader;
		const auto default_mvp_shader = new RSShader(DefaultMVP_vs, DefaultMVP_fs, false);
		shaders_map_[RSShaderNames::DEFAULT_MVP] = default_mvp_shader;
		const auto default_mvp_light_texture_shader = new RSShader(DefaultMVPLT_vs, DefaultMVPLT_fs,false);
		shaders_map_[RSShaderNames::DEFAULT_MVPLT] = default_mvp_light_texture_shader;
    const auto material_pbr_shader = new RSShader(MaterialPbr_vs, MaterialPbr_fs, false); //MATERIAL_PBR
    shaders_map_[RSShaderNames::MATERIAL_PBR] = material_pbr_shader;
		//const auto default_point_instancing = new RSShader(DefaultParticle_vs, DefaultParticle_fs,false);
		//shaders_map_[RSShaderNames::DEFAULT_PARTICLE] = default_point_instancing;
		const auto hud_text = new RSShader(HudText_vs, HudText_fs,false);
		shaders_map_[RSShaderNames::HUD_TEXT] = hud_text;
		const auto hud_texture = new RSShader(HudTexture_vs, HudTexture_fs,false);
		shaders_map_[RSShaderNames::HUD_TEXTURE] = hud_texture;
		const auto skybox_shader = new RSShader(Skybox_vs, Skybox_fs,false);
		shaders_map_[RSShaderNames::SKYBOX] = skybox_shader;
		const auto shadow_mapping = new RSShader(ShadowMapping_vs, ShadowMapping_fs,false);
		shaders_map_[RSShaderNames::SHADOW_MAPPING] = shadow_mapping;
		const auto quad_Debug = new RSShader(Quad_Debug_vs, Quad_Debug_fs,false);
		shaders_map_[RSShaderNames::QUAD_DEBUG] = quad_Debug;
		const auto quad_texture = new RSShader(QuadTexture_vs, QuadTexture_fs,false);
		shaders_map_[RSShaderNames::QUAD_TEXTURE] = quad_texture;
		const auto default_opaque = new RSShader(DefaultMVPLT_Opaque_vs, DefaultMVPLT_Opaque_fs,false);
		shaders_map_[RSShaderNames::DEFAULT_OPAQUE] = default_opaque;
		const auto hud_mapping = new RSShader(HUDPicking_vs, HUDPicking_fs,false);
		shaders_map_[RSShaderNames::HUD_MAPPING] = hud_mapping;
		const auto quad_texture_r32 = new RSShader(QuadTextureR32_vs, QuadTextureR32_fs,false);
		shaders_map_[RSShaderNames::QUAD_TEXTURE_R32] = quad_texture_r32;
		const auto particle_render = new RSShader(ParticleDefaultShader_vs, ParticleDefaultShader_fs,false);
		shaders_map_[RSShaderNames::PARTICLE_RENDER] = particle_render;
		const auto solid_color = new RSShader(SolidColor_vs, SolidColor_fs,false);
		shaders_map_[RSShaderNames::SOLID_COLOR] = solid_color;
		const auto point_clipper = new RSShader(PointCloudShader_vs, PointCloudShader_fs,false);
		shaders_map_[RSShaderNames::POINT_CLIPPER] = point_clipper;

		//const auto point_single_float_vtk = new RSShader("resources\\GLSL\\PointSingleFloatVtk.vert", "resources\\GLSL\\PointSingleFloatVtk.frag", true);
		const auto point_single_float_vtk = new RSShader(PointSingleFloatVtk_vs, PointSingleFloatVtk_fs, false);
    shaders_map_[RSShaderNames::POINT_SINGLE_FLOAT_VTK] = point_single_float_vtk;
    //const auto point_multi_float_vtk = new RSShader("resources\\GLSL\\PointMultiFloatVtk.vert", "resources\\GLSL\\PointMultiFloatVtk.frag", true);
    const auto point_multi_float_vtk = new RSShader(PointMultiFloatVtk_vs, PointMultiFloatVtk_fs, false);
    shaders_map_[RSShaderNames::POINT_THREE_FLOAT_VTK] = point_multi_float_vtk;
    //const auto point_single_uint_vtk = new RSShader("resources\\GLSL\\PointSingleUintVtk.vert", "resources\\GLSL\\PointSingleUintVtk.frag", true);
    const auto point_single_uint_vtk = new RSShader(PointSingleUintVtk_vs, PointSingleUintVtk_fs, false);
    shaders_map_[RSShaderNames::POINT_SINGLE_UINT_VTK] = point_single_uint_vtk;

		//DEFERRED_MATERIAL_PBR
		//const auto deferred_material_pbr = new RSShader("resources\\GLSL\\DMaterialPbr.vert", "resources\\GLSL\\DMaterialPbr.frag", true);
		const auto deferred_material_pbr = new RSShader(DMaterialPbr_vs, DMaterialPbr_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_MATERIAL_PBR] = deferred_material_pbr;
    //const auto deferred_particle_render = new RSShader("resources\\GLSL\\DParticleRender.vert", "resources\\GLSL\\DParticleRender.frag", true);
    const auto deferred_particle_render = new RSShader(DParticleRender_vs, DParticleRender_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_PARTICLE_RENDER] = deferred_particle_render;

    //const auto deferred_point_single_float_vtk = new RSShader("resources\\GLSL\\DPointSingleFloatVtk.vert", "resources\\GLSL\\DPointSingleFloatVtk.frag", true);
    const auto deferred_point_single_float_vtk = new RSShader(DPointSingleFloatVtk_vs, DPointSingleFloatVtk_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_POINT_SINGLE_FLOAT_VTK] = deferred_point_single_float_vtk;
    //const auto deferred_point_multi_float_vtk = new RSShader("resources\\GLSL\\DPointMultiFloatVtk.vert", "resources\\GLSL\\DPointMultiFloatVtk.frag", true);
    const auto deferred_point_multi_float_vtk = new RSShader(DPointMultiFloatVtk_vs, DPointMultiFloatVtk_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_POINT_THREE_FLOAT_VTK] = deferred_point_multi_float_vtk;
    //const auto deferred_point_single_uint_vtk = new RSShader("resources\\GLSL\\DPointSingleUintVtk.vert", "resources\\GLSL\\DPointSingleUintVtk.frag", true);
    const auto deferred_point_single_uint_vtk = new RSShader(DPointSingleUintVtk_vs, DPointSingleUintVtk_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_POINT_SINGLE_UINT_VTK] = deferred_point_single_uint_vtk;
    //const auto deferred_point_clipper = new RSShader("resources\\GLSL\\DPointClipper.vert", "resources\\GLSL\\DPointClipper.frag", true);
    const auto deferred_point_clipper = new RSShader(DPointClipper_vs, DPointClipper_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_POINT_CLIPPER] = deferred_point_clipper;



		/***** POST PROCESSING *****/

    //const auto deferred_composite = new RSShader("resources\\GLSL\\DeferredQuad.vert", "resources\\GLSL\\DeferredComposite.frag", true);
    const auto deferred_composite = new RSShader(DeferredQuad_vs, DeferredComposite_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_COMPOSITE] = deferred_composite;
    //const auto deferred_ssao = new RSShader("resources\\GLSL\\DeferredQuad.vert", "resources\\GLSL\\DeferredSSAO.frag", true);
    const auto deferred_ssao = new RSShader(DeferredQuad_vs, DeferredSSAO_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_SSAO] = deferred_ssao;
    //const auto deferred_ssao_blur = new RSShader("resources\\GLSL\\DeferredQuad.vert", "resources\\GLSL\\DeferredSSAOBlur.frag", true);
    const auto deferred_ssao_blur = new RSShader(DeferredQuad_vs, DeferredSSAOBlur_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_SSAO_BLUR] = deferred_ssao_blur;
    //const auto deferred_ssr = new RSShader("resources\\GLSL\\DeferredQuad.vert", "resources\\GLSL\\DeferredScreenSpaceReflection.frag", true);
    const auto deferred_ssr = new RSShader(DeferredQuad_vs, DeferredScreenSpaceReflection_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION] = deferred_ssr;
    //const auto deferred_ssr_blur = new RSShader("resources\\GLSL\\DeferredQuad.vert", "resources\\GLSL\\DeferredScreenSpaceReflectionBlur.frag", true);
    const auto deferred_ssr_blur = new RSShader(DeferredQuad_vs, DeferredScreenSpaceReflectionBlur_fs, false);
    shaders_map_[RSShaderNames::DEFERRED_SCREEN_SPACE_REFLECTION_BLUR] = deferred_ssr_blur;



		// Compute Shaders
		//const auto gravity_compute_shader = new ComputeShader("resources\\GLSL\\ComputeParticles.comp");
		//comp_shaders_map_[RSComputeShaderNames::GRAVITY] = gravity_compute_shader;`
		//const auto sph_test_compute_shader = new ComputeShader("resources\\GLSL\\ComputeParticlesSPH.comp");
		//comp_shaders_map_[RSComputeShaderNames::SPH_TEST] = sph_test_compute_shader;
		//const auto sph_pressure_compute_shader = new ComputeShader("resources\\GLSL\\ComputeSPH_pressure.comp");
		//comp_shaders_map_[RSComputeShaderNames::SPH_PRESSURE] = sph_pressure_compute_shader;

		const auto sph_triple_buffering_compute_shader = new ComputeShader(ComputeParticleTriBuffer_cs, false);
		comp_shaders_map_[RSComputeShaderNames::SPH_TRIPLE_BUFFERING] = sph_triple_buffering_compute_shader;

		//const auto sph_grid_init_compute_shader = new ComputeShader(ComputeGridInit_cs, false);
		const auto sph_grid_init_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridInit.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_INIT] = sph_grid_init_compute_shader;
		//const auto sph_grid_batch_compute_shader = new ComputeShader(ComputeGridBatch_cs, false);
		const auto sph_grid_batch_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridBatch.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_BATCH] = sph_grid_batch_compute_shader;
		//const auto sph_grid_main_compute_shader = new ComputeShader(ComputeGridSPH_cs, false);
		const auto sph_grid_main_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridSPH.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_MAIN] = sph_grid_main_compute_shader;
		//const auto sph_grid_post_compute_shader = new ComputeShader(ComputeGridPost_cs, false);
		const auto sph_grid_post_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridPost.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_POST] = sph_grid_post_compute_shader;
		//const auto sph_grid_density_compute_shader = new ComputeShader(ComputeGridDensity_cs, false);
		const auto sph_grid_density_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridDensity.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_DENSITY] = sph_grid_density_compute_shader;
		//const auto sph_grid_acc_compute_shader = new ComputeShader(ComputeGridAcc_cs, false);
		const auto sph_grid_acc_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridAcc.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_ACC] = sph_grid_acc_compute_shader;

		const auto sph_cmu_density_compute_shader = new ComputeShader(ComputeCMUDensity_cs, false);
		comp_shaders_map_[RSComputeShaderNames::SPH_CMU_DENSITY] = sph_cmu_density_compute_shader;
		const auto sph_cmu_force_compute_shader = new ComputeShader(ComputeCMUForce_cs, false);
		comp_shaders_map_[RSComputeShaderNames::SPH_CMU_FORCE] = sph_cmu_force_compute_shader;
		const auto sph_cmu_post_compute_shader = new ComputeShader(ComputeCMUPost_cs, false);
		comp_shaders_map_[RSComputeShaderNames::SPH_CMU_POST] = sph_cmu_post_compute_shader;

    const auto sph_dynamic_bt_state_compute_shader = new ComputeShader(DynamicInOutFlowBatch_cs, false);
    comp_shaders_map_[RSComputeShaderNames::SPH_DYNAMIC_BT_STATE] = sph_dynamic_bt_state_compute_shader;
    const auto sph_dynamic_bt_creator_compute_shader = new ComputeShader(DynamicInOutFlowCreator_cs, false);
    comp_shaders_map_[RSComputeShaderNames::SPH_DYNAMIC_BT_CREATOR] = sph_dynamic_bt_creator_compute_shader;
    const auto sph_dynamic_bt_density_compute_shader = new ComputeShader(DynamicComputeDensity_cs, false);
    comp_shaders_map_[RSComputeShaderNames::SPH_DYNAMIC_BT_DENSITY] = sph_dynamic_bt_density_compute_shader;
    const auto sph_dynamic_bt_force_compute_shader = new ComputeShader(DynamicComputeForce_cs, false);
    comp_shaders_map_[RSComputeShaderNames::SPH_DYNAMIC_BT_FORCE] = sph_dynamic_bt_force_compute_shader;
    const auto sph_dynamic_bt_post_compute_shader = new ComputeShader(DynamicComputePost_cs, false);
    comp_shaders_map_[RSComputeShaderNames::SPH_DYNAMIC_BT_POST] = sph_dynamic_bt_post_compute_shader;

#if 0
		/****************************** Default RSShader  ******************************/
		const auto default_shader = new RSShader("resources\\GLSL\\default.vert", "resources\\GLSL\\default.frag");
		shaders_map_[RSShaderNames::DEFAULT] = default_shader;
		const auto default_mvp_shader = new RSShader("resources\\GLSL\\DefaultMVP.vert", "resources\\GLSL\\DefaultMVP.frag");
		shaders_map_[RSShaderNames::DEFAULT_MVP] = default_mvp_shader;
		const auto default_mvp_light_texture_shader = new RSShader("resources\\GLSL\\DefaultMVPLT.vert", "resources\\GLSL\\DefaultMVPLT.frag");
		shaders_map_[RSShaderNames::DEFAULT_MVPLT] = default_mvp_light_texture_shader;
		//const auto default_point_instancing = new RSShader("resources\\GLSL\\DefaultParticle.vert", "resources\\GLSL\\DefaultParticle.frag");
		//shaders_map_[RSShaderNames::DEFAULT_PARTICLE] = default_point_instancing;
		const auto hud_text = new RSShader("resources\\GLSL\\HUDText.vert", "resources\\GLSL\\HUDText.frag");
		shaders_map_[RSShaderNames::HUD_TEXT] = hud_text;
		const auto hud_texture = new RSShader("resources\\GLSL\\HUDTexture.vert", "resources\\GLSL\\HUDTexture.frag");
		shaders_map_[RSShaderNames::HUD_TEXTURE] = hud_texture;
		const auto skybox_shader = new RSShader("resources\\GLSL\\Skybox.vert", "resources\\GLSL\\Skybox.frag");
		shaders_map_[RSShaderNames::SKYBOX] = skybox_shader;
		const auto shadow_mapping = new RSShader("resources\\GLSL\\ShadowMapping.vert", "resources\\GLSL\\ShadowMapping.frag");
		shaders_map_[RSShaderNames::SHADOW_MAPPING] = shadow_mapping;
		const auto quad_Debug = new RSShader("resources\\GLSL\\Quad_Debug.vert", "resources\\GLSL\\Quad_Debug.frag");
		shaders_map_[RSShaderNames::QUAD_DEBUG] = quad_Debug;
		const auto quad_texture = new RSShader("resources\\GLSL\\QuadTexture.vert", "resources\\GLSL\\QuadTexture.frag");
		shaders_map_[RSShaderNames::QUAD_TEXTURE] = quad_texture;
		const auto default_opaque = new RSShader("resources\\GLSL\\DefaultMVPLT_Opaque.vert", "resources\\GLSL\\DefaultMVPLT_Opaque.frag");
		shaders_map_[RSShaderNames::DEFAULT_OPAQUE] = default_opaque;
		const auto hud_mapping = new RSShader("resources\\GLSL\\HUDPicking.vert", "resources\\GLSL\\HUDPicking.frag");
		shaders_map_[RSShaderNames::HUD_MAPPING] = hud_mapping;
		const auto quad_texture_r32 = new RSShader("resources\\GLSL\\QuadTextureR32.vert", "resources\\GLSL\\QuadTextureR32.frag");
		shaders_map_[RSShaderNames::QUAD_TEXTURE_R32] = quad_texture_r32;
		const auto particle_render = new RSShader("resources\\GLSL\\ParticleDefaultShader.vert", "resources\\GLSL\\ParticleDefaultShader.frag");
		shaders_map_[RSShaderNames::PARTICLE_RENDER] = particle_render;
		const auto solid_color = new RSShader("resources\\GLSL\\SolidColor.vert", "resources\\GLSL\\SolidColor.frag");
		shaders_map_[RSShaderNames::SOLID_COLOR] = solid_color;
		const auto point_clipper = new RSShader("resources\\GLSL\\PointCloudShader.vert", "resources\\GLSL\\PointCloudShader.frag");
		shaders_map_[RSShaderNames::POINT_CLIPPER] = point_clipper;


		// Compute Shaders
		//const auto gravity_compute_shader = new ComputeShader("resources\\GLSL\\ComputeParticles.comp");
		//comp_shaders_map_[RSComputeShaderNames::GRAVITY] = gravity_compute_shader;
		//const auto sph_test_compute_shader = new ComputeShader("resources\\GLSL\\ComputeParticlesSPH.comp");
		//comp_shaders_map_[RSComputeShaderNames::SPH_TEST] = sph_test_compute_shader;
		//const auto sph_pressure_compute_shader = new ComputeShader("resources\\GLSL\\ComputeSPH_pressure.comp");
		//comp_shaders_map_[RSComputeShaderNames::SPH_PRESSURE] = sph_pressure_compute_shader;

		const auto sph_triple_buffering_compute_shader = new ComputeShader("resources\\GLSL\\ComputeParticleTriBuffer.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_TRIPLE_BUFFERING] = sph_triple_buffering_compute_shader;
		const auto sph_grid_init_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridInit.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_INIT] = sph_grid_init_compute_shader;
		const auto sph_grid_batch_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridBatch.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_BATCH] = sph_grid_batch_compute_shader;
		const auto sph_grid_main_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridSPH.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_MAIN] = sph_grid_main_compute_shader;
		const auto sph_grid_post_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridPost.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_POST] = sph_grid_post_compute_shader;
		const auto sph_grid_density_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridDensity.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_DENSITY] = sph_grid_density_compute_shader;
		const auto sph_grid_acc_compute_shader = new ComputeShader("resources\\GLSL\\ComputeGridAcc.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_GRID_ACC] = sph_grid_acc_compute_shader;

		const auto sph_cmu_density_compute_shader = new ComputeShader("resources\\GLSL\\ComputeCMUDensity.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_CMU_DENSITY] = sph_cmu_density_compute_shader;
		const auto sph_cmu_force_compute_shader = new ComputeShader("resources\\GLSL\\ComputeCMUForce.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_CMU_FORCE] = sph_cmu_force_compute_shader;
		const auto sph_cmu_post_compute_shader = new ComputeShader("resources\\GLSL\\ComputeCMUPost.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_CMU_POST] = sph_cmu_post_compute_shader;

		const auto sph_dynamic_bt_state_compute_shader = new ComputeShader("resources\\GLSL\\DynamicInOutFlowBatch.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_DYNAMIC_BT_STATE] = sph_dynamic_bt_state_compute_shader;
		const auto sph_dynamic_bt_creator_compute_shader = new ComputeShader("resources\\GLSL\\DynamicInOutFlowCreator.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_DYNAMIC_BT_CREATOR] = sph_dynamic_bt_creator_compute_shader;
		const auto sph_dynamic_bt_density_compute_shader = new ComputeShader("resources\\GLSL\\DynamicComputeDensity.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_DYNAMIC_BT_DENSITY] = sph_dynamic_bt_density_compute_shader;
		const auto sph_dynamic_bt_force_compute_shader = new ComputeShader("resources\\GLSL\\DynamicComputeForce.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_DYNAMIC_BT_FORCE] = sph_dynamic_bt_force_compute_shader;
		const auto sph_dynamic_bt_post_compute_shader = new ComputeShader("resources\\GLSL\\DynamicComputePost.comp");
		comp_shaders_map_[RSComputeShaderNames::SPH_DYNAMIC_BT_POST] = sph_dynamic_bt_post_compute_shader;
#endif

	}

	void RSShaderManager::Shutdown()
	{
		for (const auto& item : shaders_map_)
			delete item.second;

		for (const auto& item : comp_shaders_map_)
			delete item.second;

		shaders_map_.clear();
		comp_shaders_map_.clear();
	}

	void RSShaderManager::Compile() const
	{
		for (auto& item : shaders_map_)
			item.second->CompileShader();

		for (auto& item : comp_shaders_map_)
			item.second->CompileShader();
	}

	void RSShaderManager::Use(const RSShaderNames name_)
	{
		shaders_map_[name_]->Use();
	}

	void RSShaderManager::Use(const RSComputeShaderNames name_)
	{
		comp_shaders_map_[name_]->Use();
	}

	void RSShaderManager::UnbindShader()
	{
    glUseProgram(0);
	}

	void RSShaderManager::SetData(const RSShaderNames shader_name_, const std::string& value_name, const glm::mat4& data)
	{
		shaders_map_[shader_name_]->SetMat4(value_name, data);
	}

	void RSShaderManager::SetData(const RSShaderNames shader_name_, const std::string& value_name, const glm::vec3 data)
	{
		shaders_map_[shader_name_]->SetVec3(value_name, data);
	}

	void RSShaderManager::SetData(const RSShaderNames shader_name_, const std::string& value_name, const glm::ivec3 data)
	{
		shaders_map_[shader_name_]->SetIVec3(value_name, data);
	}

	void RSShaderManager::SetData(const RSShaderNames shader_name_, const std::string& value_name, const glm::vec4 data)
	{
		shaders_map_[shader_name_]->SetVec4(value_name, data);
	}

	void RSShaderManager::SetData(const RSShaderNames shader_name_, const std::string& value_name, const glm::ivec4 data)
	{
		shaders_map_[shader_name_]->SetIVec4(value_name, data);
	}

	void RSShaderManager::SetData(RSShaderNames shader_name_, const std::string& value_name, glm::vec2 data)
	{
    shaders_map_[shader_name_]->SetVec2(value_name, data);
	}

	void RSShaderManager::SetData(const RSShaderNames shader_name_, const std::string& value_name, const glm::float32 data)
	{
		shaders_map_[shader_name_]->SetFl32(value_name, data);
	}

	void RSShaderManager::SetData(const RSShaderNames shader_name_, const std::string& value_name, const int data)
	{
		shaders_map_[shader_name_]->SetInt(value_name, data);
	}

	void RSShaderManager::SetData(const RSShaderNames shader_name_, const std::string& value_name, const unsigned int data)
	{
		shaders_map_[shader_name_]->SetUInt(value_name, data);
	}

	void RSShaderManager::SetData(const RSShaderNames shader_name_, const std::string& value_name, const bool data)
	{
		shaders_map_[shader_name_]->SetBool(value_name, data);
	}

	void RSShaderManager::SetTexture(const RSShaderNames shader_name_, const std::string& value_name,
                                   const unsigned int  data, const unsigned int         index)
	{
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, data);

		shaders_map_[shader_name_]->SetUInt(value_name, index);
	}

	void RSShaderManager::SetData(const RSComputeShaderNames shader_name_, const std::string& value_name,
                                const glm::mat4&           data)
	{
		comp_shaders_map_[shader_name_]->SetMat4(value_name, data);
	}

	void RSShaderManager::SetData(const RSComputeShaderNames shader_name_, const std::string& value_name,
                                const glm::vec3            data)
	{
		comp_shaders_map_[shader_name_]->SetVec3(value_name, data);
	}

	void RSShaderManager::SetData(const RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name,
                                const glm::ivec3                            data)
	{
		comp_shaders_map_[shader_name_]->SetIVec3(value_name, data);
	}

	void RSShaderManager::SetData(const RSComputeShaderNames shader_name_, const std::string& value_name,
                                const glm::vec4            data)
	{
		comp_shaders_map_[shader_name_]->SetVec4(value_name, data);
	}

	void RSShaderManager::SetData(const RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name,
                                const glm::ivec4                            data)
	{
		comp_shaders_map_[shader_name_]->SetIVec4(value_name, data);
	}

	void RSShaderManager::SetData(const RSComputeShaderNames shader_name_, const std::string& value_name,
                                const glm::float32         data)
	{
		comp_shaders_map_[shader_name_]->SetFl32(value_name, data);
	}

	void RSShaderManager::SetData(const RSComputeShaderNames shader_name_, const std::string& value_name, const int data)
	{
		comp_shaders_map_[shader_name_]->SetInt(value_name, data);
	}

	void RSShaderManager::SetData(const RSComputeShaderNames shader_name_, const std::string& value_name,
                                const unsigned int         data)
	{
		comp_shaders_map_[shader_name_]->SetUInt(value_name, data);
	}

	void RSShaderManager::SetData(const RSComputeShaderNames shader_name_, const std::string& value_name, const bool data)
	{
		comp_shaders_map_[shader_name_]->SetBool(value_name, data);
	}

}
