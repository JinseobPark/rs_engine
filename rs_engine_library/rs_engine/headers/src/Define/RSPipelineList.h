/******************************************************************************/
/*!
\file   RSPipelineList.h
\author Jinseob Park
\date   2024/08/01

This file contains pipeline, shader names, rendering layer system for Rendering System.

*/
/******************************************************************************/
#ifndef RS_PIPELINE_LIST_H_
#define RS_PIPELINE_LIST_H_

namespace RS_PipelineList
{


/**
* @brief Shader Names enum list. 
* Users maybe not use all of them. It controlled by Engine.
* 
*/
enum class RSShaderNames : std::uint8_t
{
	DEFAULT = 0,
	DEFAULT_MVP,
	//DEFAULT_MVPL,
	DEFAULT_OPAQUE,
	DEFAULT_MVPLT,
	MATERIAL_PBR,
	//DEFAULT_PARTICLE,
	HUD_TEXT,
	HUD_TEXTURE,
	SKYBOX,
	SHADOW_MAPPING,
	HUD_MAPPING,
	QUAD_DEBUG,
	QUAD_TEXTURE_R32,
	QUAD_TEXTURE,
	SOLID_COLOR,

	POINT_CLIPPER,
	DEFERRED_POINT_CLIPPER,

	PARTICLE_RENDER,
  //DEFERRED_PARTICLE_RENDER,

	POINT_SINGLE_FLOAT_VTK,
	POINT_THREE_FLOAT_VTK,
	POINT_SINGLE_UINT_VTK,

  DEFERRED_POINT_SINGLE_FLOAT_VTK,
  DEFERRED_POINT_THREE_FLOAT_VTK,
  DEFERRED_POINT_SINGLE_UINT_VTK,

	DEFERRED_MATERIAL_PBR,
	DEFERRED_PARTICLE_RENDER,
	DEFERRED_COMPOSITE,
  DEFERRED_SSAO,
  DEFERRED_SSAO_BLUR,
  DEFERRED_SCREEN_SPACE_REFLECTION,
  DEFERRED_SCREEN_SPACE_REFLECTION_BLUR,
	SHADER_COUNT
};

/**
* @brief Compute Shader Names enum list.
* 
*/
enum class RSComputeShaderNames : int
{
	DEFAULT = 0,
	//GRAVITY,
	//SPH_TEST,
	//SPH_PRESSURE,
	SPH_TRIPLE_BUFFERING,
	//SAMPLE,
	//FIND_SURFACE,
	SPH_GRID_INIT,
	SPH_GRID_BATCH,
	SPH_GRID_DENSITY,
	SPH_GRID_ACC,
	SPH_GRID_MAIN,
	SPH_GRID_POST,

	SPH_CMU_DENSITY,
	SPH_CMU_FORCE,
	SPH_CMU_POST,

	SPH_DYNAMIC_BT_STATE,
	SPH_DYNAMIC_BT_CREATOR,
  SPH_DYNAMIC_BT_DENSITY,
  SPH_DYNAMIC_BT_FORCE,
  SPH_DYNAMIC_BT_POST,

};

/**
* @brief Pipeline Type enum list. This is not used in the project yet.
* 
*/ 
enum class RSPipelineType : std::uint8_t
{
	DEFAULT = 0,
	DEFAULT_MVP,
	DEFAULT_PARTICLE,

};

/**
* @brief Render Layer enum list.
* Users can use this enum list for rendering layer system.
*
*
*/
enum class RSRenderLayer : std::uint8_t
{
	R_DEFAULT = 0,
	R_SURFACE_OPAQUE,
	R_LINE_OPAQUE,
	R_POINT,
	R_SURFACE_WIREFRAME_OPAQUE,
	//R_SURFACE_TRANSPARENT,
	//R_LINE_TRANSPARENT,
	//R_SURFACE_WIRE_TRANSPARENT,
	R_LIGHT,
	R_PARTICLE,
	R_SKYBOX,
	R_HIGHLIGHT,
	R_SSAO,
	R_SHADOW,
	R_HUD_TEXT,
	R_HUD_TEXTURE,
	R_COUNT
};


///**
//* @brief Render Type enum list.
//*  Not used in the project yet.
//*/
//enum class RSDataRenderType : std::uint8_t
//{
//	T_NONE = 0x00000000,
//	T_DEFAULT = 0x00000001,
//	T_TEXTURE = 0x00000002,
//	T_BOUNDARY = 0x00000004,
//	T_SELECTED = 0x00000010,
//	T_PARTICLE = 0x00000020,
//	T_TRACE = 0x00000040,
//	T_TRANSPARENT = 0x00000080,
//
//	//T_COUNT = 0xffffffff
//
//
//};

} // namespace RS_PipelineList



#endif // !RS_PIPELINE_LIST_H_