#include "pch.h"
#include "RSMaterialManager.h"
#include "RSResourceManager.h"

namespace RS_Material
{
RSMaterialManager::RSMaterialManager() = default;

RSMaterialManager::~RSMaterialManager() = default;

void RSMaterialManager::Initialize()
{
  // Build default materials
  BuildMaterials();
}

void RSMaterialManager::Shutdown()
{
  // Release all materials
  for (auto& [fst, snd] : m_materials)
  {
    delete snd;
  }
  m_materials.clear();
}

RSMaterial* RSMaterialManager::CreateMaterial(const std::string& name,
  const unsigned int diffuse_map_, const unsigned int normal_map_, const float metallic_, const float roughness_, const float ambient_)
{
  // Check if the material already exists, and return it
  if (IsExistMaterial(name))
  {
    RS_WARN("Material already exists : %s", name.c_str());
    return m_materials[name];
  }

  GLuint diffuse_map = diffuse_map_;
  if (diffuse_map == 0)
    diffuse_map = m_default_texture_uint;

  // Create a new material and add it to m_materials
  const auto material = new RSMaterial(diffuse_map, normal_map_, metallic_, roughness_, ambient_);
  material->SetName(name);
  m_materials[name] = material;
  return material;
}

RSMaterial* RSMaterialManager::GetMaterial(const std::string& name)
{
  if (IsExistMaterial(name))
    return m_materials[name];

  return nullptr;
}

void RSMaterialManager::ReleaseMaterial(const std::string& name)
{
  // Deallocate the material from m_materials
  if (IsExistMaterial(name))
  {
    delete m_materials[name];
    m_materials.erase(name);
  }
}

bool RSMaterialManager::IsExistMaterial(const std::string& name) const
{
  // Check if the material exists from m_materials
  return m_materials.contains(name);
}

std::vector<std::string> RSMaterialManager::GetMaterialNames()
{
  // Get all material names from m_materials
  std::vector<std::string> names;
  names.resize(m_materials.size());
  std::ranges::transform(m_materials, names.begin(), [](const auto& pair) { return pair.first; });

  return names;
}

void RSMaterialManager::BuildMaterials()
{
  // Check Timer

  const auto m_tm = RSResourceManager::GetInstance()->GetTextureManager();
  m_default_texture_uint = m_tm->GetDefaultTexture_UINT();
  m_default_normal_map_uint = m_tm->GetDefaultNormalTexture_UINT();


  CreateMaterial("default", m_default_texture_uint, m_default_normal_map_uint);
  CreateMaterial("ticle_face", m_tm->GetTexture_UINT("ticle_face"),
                 m_tm->GetNormalTexture_UINT("ticle_face_normalmap"));

  CreateMaterial("bok", m_tm->GetTexture_UINT("bok"),
                  m_tm->GetNormalTexture_UINT("bok_normalmap"));

  CreateMaterial("bono", m_tm->GetTexture_UINT("bono"),
                  m_tm->GetNormalTexture_UINT("bono_normalmap"));

  CreateMaterial("ddungi", m_tm->GetTexture_UINT("ddungi"),
                  m_tm->GetNormalTexture_UINT("ddungi_normalmap"), 0.5f, 0.5f, 0.5f);

  CreateMaterial("jordy", m_tm->GetTexture_UINT("jordy"),
                  m_tm->GetNormalTexture_UINT("jordy_normalmap"));

  CreateMaterial("neko", m_tm->GetTexture_UINT("neko"),
                  m_tm->GetNormalTexture_UINT("neko_normalmap"));

  CreateMaterial("ticle_center", m_tm->GetTexture_UINT("ticle_center"),
                  m_tm->GetNormalTexture_UINT("ticle_center_normalmap"));

  CreateMaterial("ticle", m_tm->GetTexture_UINT("ticle"),
                  m_tm->GetNormalTexture_UINT("ticle_normalmap"));

  CreateMaterial("light_gold", m_tm->GetTexture_UINT("light_gold"),
                  m_tm->GetNormalTexture_UINT("light_gold_normalmap"), 1.0f);

  CreateMaterial("dirty_tiles", m_tm->GetTexture_UINT("dirty_tiles"),
                  m_tm->GetNormalTexture_UINT("dirty_tiles_normalmap"), 0.4f, 0.5f);

  CreateMaterial("alley-brick-wall", m_tm->GetTexture_UINT("alley-brick-wall"),
                  m_tm->GetNormalTexture_UINT("alley-brick-wall_normalmap"), 0.0f, 0.8f, 0.4f);

  CreateMaterial("cloth-fabric", m_default_texture_uint, m_default_normal_map_uint); ///< Temporary, modify on runtime

}
    
}
