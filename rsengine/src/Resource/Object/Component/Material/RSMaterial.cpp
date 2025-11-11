#include "pch.h"
#include "RSMaterial.h"

namespace RS_Material
{
  void RSMaterial::SetDiffuseMap(const GLuint diffuse_map_)
  {
    m_diffuse_map = diffuse_map_;
  }
  void RSMaterial::SetNormalMap(const GLuint normal_map_)
  {
    has_normal_map = (normal_map_ != 0);
    m_normal_map = normal_map_;
  }
  void RSMaterial::SetName(const std::string& name_)
  {
    name = name_;
  }
  void RSMaterial::SetMetallic(const float metallic_)
  {
    m_metallic = metallic_;
  }
  void RSMaterial::SetRoughness(const float roughness_)
  {
    m_roughness = roughness_;
  }
  void RSMaterial::SetAmbient(const float ambient_)
  {
    m_ambient = ambient_;
  }
  void RSMaterial::SetTilling(const glm::vec2& tiling_)
  {
    m_tiling = tiling_;
    UpdateTexTransform();
  }
  void RSMaterial::SetTilling(const float x_, const float y_)
  {
    m_tiling = glm::vec2(x_, y_);
    UpdateTexTransform();
  }
  void RSMaterial::SetOffset(const glm::vec2& offset_)
  {
    m_offset = offset_;
    UpdateTexTransform();
  }
  void RSMaterial::SetOffset(const float x_, const float y_)
  {
    m_offset = glm::vec2(x_, y_);
    UpdateTexTransform();
  }
  void RSMaterial::UpdateTexTransform()
  {
    m_tex_transform = glm::mat4(1.0f);
    m_tex_transform = glm::translate(m_tex_transform, glm::vec3(m_offset, 0.0f));
    m_tex_transform = glm::scale(m_tex_transform, glm::vec3(m_tiling, 1.0f));
  }
  void RSMaterial::SetMaterial(const GLuint diffuse_map_, const GLuint normal_map_, const float metallic_,
                               const float  roughness_, const float ambient)
  {
    m_diffuse_map = diffuse_map_;
    m_normal_map = normal_map_;
    m_metallic = metallic_;
    m_roughness = roughness_;
    m_ambient = ambient;
    has_normal_map = (normal_map_ != 0);
  }



}
