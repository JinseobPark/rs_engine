/*********************************************************************
 * \file   RSMaterialManager.h
 * \author Jinseob Park
 * \date   2025/02/25
 * 
 * \brief  Material Manager for the RS Engine
 *********************************************************************/
#ifndef RS_MATERIAL_MANAGER_H_
#define RS_MATERIAL_MANAGER_H_
#include "../Object/Component/Material/RSMaterial.h"


namespace RS_Material
{
  class RSMaterialManager
  {
  public:
    RSMaterialManager();
    RSMaterialManager(const RSMaterialManager& other) = delete;
    RSMaterialManager& operator=(const RSMaterialManager& other) = delete;
    ~RSMaterialManager();


    /**
     * @brief Initialization
     */
    void Initialize();


    /**
     * @brief Deallocate all materials
     */
    void Shutdown();


    /**
     * @brief Create a new material
     * If the material already exists, return the existing material.
     * @param name name of material
     * @param diffuse_map_ diffuse map texture id. If 0, use default texture. Default is 0.
     * @param normal_map_ [optional] normal map texture id. Default is 0.
     * @param metallic_ [optional] metallic value. Default is 0.0f
     * @param roughness_ [optional] roughness value. Default is 0.5f
     * @return The created material pointer or existing material pointer
     */
    RSMaterial* CreateMaterial(const std::string& name, const unsigned int diffuse_map_ = 0, const unsigned int normal_map_ = 0,
                               const float metallic_ = 0.0f, const float roughness_ = 0.5f, const float ambient_ = 0.3f);

    /**
     * @brief Get material by name
     * @param name name of material
     * @return material pointer. If not exist, return nullptr.
     */
    RSMaterial* GetMaterial(const std::string& name);

    /**
     * @brief Release material from memory
     * @param name name of material
     */
    void ReleaseMaterial(const std::string& name);

    /**
     * @brief Check if the material exists
     * @param name name of material
     * @return exist or not
     */
    bool IsExistMaterial(const std::string& name) const;

    /**
     * @brief Get all materials
     * @return [unordered map] material map
     */
    std::unordered_map<std::string, RSMaterial*>* GetMaterials() { return &m_materials; }

    /**
     * @brief Get material names
     * @return [vector] material names
     */
    std::vector<std::string> GetMaterialNames();

    /**
     * @brief Build materials on initialization
     * Set default texture id from texture manager.
     */
    void BuildMaterials();

  private:
    std::unordered_map<std::string, RSMaterial*> m_materials; /**< Material map */
    GLuint m_default_texture_uint = 0; /**< Default texture id */
    GLuint m_default_normal_map_uint = 0; /**< Default normal map id */
  };

}

#endif
