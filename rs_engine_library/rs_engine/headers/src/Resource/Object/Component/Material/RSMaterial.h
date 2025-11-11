/*********************************************************************
 * \file   RSMaterial.h
 * \brief  Material Struct
 * 
 * \author Jinseob Park
 * \date   2025/02/25 
 *********************************************************************/
#ifndef RS_MATERIAL_H_
#define RS_MATERIAL_H_
#include <glm/ext/matrix_transform.hpp>


/**
 * @brief Material namespace
 */
namespace RS_Material
{
  /**
 * @brief material Struct
 * TODO : Consider specular map.
 */
class RSMaterial
{
public:
  RSMaterial() : m_diffuse_map(0), m_normal_map(0), has_normal_map(false),
    m_metallic(0.0f), m_roughness(0.5f), m_ambient(0.3f)
  {
  }

  RSMaterial(const GLuint diffuse_map_) :
    m_diffuse_map(diffuse_map_)
  {
    m_normal_map = 0;
    has_normal_map = false;
    m_metallic = 0.0f;
    m_roughness = 0.5f;
    m_ambient = 0.3f;
  }

  RSMaterial(const GLuint diffuse_map_, const GLuint normal_map_, const float metallic_, const float roughness_, const float ambient) :
    m_diffuse_map(diffuse_map_), m_normal_map(normal_map_), m_metallic(metallic_), m_roughness(roughness_), m_ambient(ambient)
  {
    has_normal_map = (normal_map_ != 0);
  }

  /**
   * @brief Set diffuse map
   * @param diffuse_map_ diffuse map
   */
  void SetDiffuseMap(const GLuint diffuse_map_);

  /**
   * @brief Set normal map
   * @param normal_map_ normal map
   */
  void SetNormalMap(const GLuint normal_map_);

  /**
   * @brief Set material name
   * @param name_ material name
   */
  void SetName(const std::string& name_);

  /**
   * @brief Set metallic value
   * @param metallic_ metallic value
   */
  void SetMetallic(const float metallic_);

  /**
   * @brief Set roughness value
   * @param roughness_ roughness value
   */
  void SetRoughness(const float roughness_);

  /**
   * @brief Set ambient value
   * @param ambient_ ambient value
   */
  void SetAmbient(const float ambient_);

  /**
   * @brief Set tilling for texture mapping
   * @param tiling_ tilling
   */
  void SetTilling(const glm::vec2& tiling_);

  /**
   * @brief Set tilling for texture mapping
   * @param x_ tile x
   * @param y_ tile y
   */
  void SetTilling(const float x_, const float y_);

  /**
   * @brief Set offset for texture mapping
   * @param offset_ offset
   */
  void SetOffset(const glm::vec2& offset_);
  
  /**
   * @brief Set offset for texture mapping
   * @param x_ offset x
   * @param y_ offset y
   */
  void SetOffset(const float x_, const float y_);

  /**
   * @brief Get material name
   * @return material name
   */
  std::string GetName() const { return name; }

  /**
   * @brief Get diffuse map
   * @return diffuse map
   */
  GLuint GetDiffuseMap() const { return m_diffuse_map; }

  /**
   * @brief Get normal map
   * @return normal map
   */
  GLuint GetNormalMap() const { return m_normal_map; }

  /**
   * @brief Get metallic value
   * @return metallic value
   */
  float GetMetallic() const { return m_metallic; }

  /**
   * @brief Get roughness value
   * @return roughness value
   */
  float GetRoughness() const { return m_roughness; }

  /**
   * @brief Get ambient value
   * @return ambient value
   */
  float GetAmbient() const { return m_ambient; }

  /**
   * @brief Get tilling for texture mapping
   * @return tilling
   */
  glm::vec2 GetTiling() const { return m_tiling; }

  /**
   * @brief Get offset for texture mapping
   * @return offset
   */
  glm::vec2 GetOffset() const { return m_offset; }

  /**
   * @brief Get texture transform matrix for texture mapping
   * @return texture transform matrix
   */
  glm::mat4 GetTexTransform() const { return m_tex_transform; }

  /**
   * @brief Update texture transform matrix for texture mapping
   */
  void UpdateTexTransform();
  
  /**
   * @brief Set material
   * @param diffuse_map_ diffuse map 
   * @param normal_map_ normal map
   * @param metallic_ metallic value
   * @param roughness_ roughness value
   */
  void SetMaterial(const GLuint diffuse_map_, const GLuint normal_map_, const float metallic_, const float roughness_, const float ambient);

private:
  glm::mat4 m_tex_transform = glm::mat4(1.0f); ///< Texture transform matrix for texture mapping
  GLuint m_diffuse_map; ///< Diffuse map
  GLuint m_normal_map; ///< Normal map
  //unsigned int specular_map;

  //bool has_specular_map;

  float m_metallic; ///< Metallic value
  float m_roughness; ///< Roughness value
  float m_ambient; ///< Ambient value

  glm::vec2 m_tiling = glm::vec2(1.0f); ///< Tiling for texture mapping
  glm::vec2 m_offset = glm::vec2(0.0f); ///< Offset for texture mapping
  

  std::string name; ///< Material name
  bool has_normal_map; ///< Whether normal map is used
};

}

#endif