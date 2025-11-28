/*********************************************************************
 * \file   RSFBOHeader.h
 * \brief  RS engine Frame buffer object headers for rendering system.
 * 
 * \author jinseob Park
 * \date   2025/03/13
 *********************************************************************/
#ifndef RS_FBO_HEADER_H_
#define RS_FBO_HEADER_H_

#include "RSDeferredFbo.h"
#include "RSShadowMapFbo.h"
#include "RSFinalFbo.h"
#include "RSHudFbo.h"
#include "RSPickingFbo.h"
#include "RSSkyboxFbo.h"
#include "RSSsaoFbo.h"
#include "RSSsaoBlurFbo.h"
#include "RSSsrFbo.h"
#include "RSSsrBlurFbo.h"
#include "RSPostProcessFbo.h"

/**
 * @brief Frame buffer object type enum list.
 */
enum class FboType : std::uint16_t
{
  SHADOW_MAP = 0,
  HUD,
  PICKING,
  SKYBOX,
  DEFERRED,
  SSAO,
  SSAO_BLUR,
  SSR,
  SSR_BLUR,
  POST_PROCESS_A,  ///< Ping buffer for kernel post-processing
  POST_PROCESS_B,  ///< Pong buffer for kernel post-processing
  FINAL
};


#endif
