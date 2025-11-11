/*********************************************************************
 * \file   RSGLSL_CS.h
 * \brief  This file contains all of glsl shader code.
 * The shader codes are compute shader.
 *
 *
 * \author Jinseob Park
 * \date   25/11/2024
 *********************************************************************/
#ifndef RS_GLSL_CS_H_
#define RS_GLSL_CS_H_

extern const char* sph_triple_buffering_cs;

extern const char* sph_grid_init_cs;

extern const char* sph_grid_batch_cs;

extern const char* sph_grid_density_cs;

extern const char* sph_grid_acc_cs;

extern const char* sph_grid_main_cs;

extern const char* sph_grid_post_cs;


extern const char* sph_cmu_density_cs;

extern const char* sph_cmu_force_cs;

extern const char* sph_cmu_post_cs;



extern const char* sph_dynamic_bt_state_cs;

extern const char* sph_dynamic_bt_creator_cs;

extern const char* sph_dynamic_bt_density_cs;

extern const char* sph_dynamic_bt_force_cs;

extern const char* sph_dynamic_bt_post_cs;




#endif
