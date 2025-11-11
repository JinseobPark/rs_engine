/******************************************************************************/
/*!
\file   RSMesh.h
\author Jinseob Park
\date   2024/08/01

This file contains mesh class for model object component.
   Which includes mesh data, texture id, bounding box, and collision check.

*/
/******************************************************************************/
#ifndef RS_MESH_H_
#define RS_MESH_H_

#include "glm/glm.hpp"
#include "RSBoundingBox.h"
#include "../Material/RSMaterial.h"
#include <vector>

/**
 * @brief Mesh namespace
 */
namespace RS_Mesh
{
	/**
	 * @brief Mesh class for model object component
	 */
	class RSMesh
	{
	public:
		RSMesh();
		~RSMesh();

    // Deep copy constructor
    RSMesh(const RSMesh& other);

		/**
		 * @brief Initialize Mesh
		 */
		void Initialize();

		/**
		 * @brief Set data to mesh, Set bounding box min and max
		 * @param data mesh data
		 */
		void SetMeshData(RSMeshData* data);

		/**
		 * @brief Get mesh data
		 * @return mesh data pointer
		 */
		RS_N_D RSMeshData* GetMeshData() const;

		/**
		 * @brief Get Texture id
		 * @return texture id (uint)
		 */
		RS_N_D unsigned int GetTextureID() const;

		/**
		 * @brief Set Texture id
		 * @param id texture id (uint)
		 */
		void SetTextureID(unsigned int id);

		/**
		 * @brief Get Is Texture
		 * @return is_texture (bool)
		 */
		bool GetIsTexture() const; 

		/**
		 * @brief Set Is Texture. When the mesh has texture, then set true.
		 * @param is is_texture (bool)
		 */
		void SetIsTexture(bool is);

		/**
		 * @brief Set Bounding Box by bounding box pointer
		 * @param box bounding box pointer
		 */
		void SetBoundingBox(const RSBoundingBox& box); 

		/**
		 * @brief Set Bounding Box by min and max
		 * @param min min point of bounding box
		 * @param max max point of bounding box
		 */
		void SetBoundingBox(const glm::vec3& min, const glm::vec3& max);

		/**
		 * @brief Get Bounding Box
		 * @return bounding box pointer
		 */
		RSBoundingBox* GetBoundingBox();

		/**
		 * @brief Get Bounding Box triangles (12 triangles)
		 * @return bounding box triangles vector of bounding triangles
		 */
		RS_N_D const std::vector<RSBoundingTriangle>& GetTriangles() const;

		/**
		 * @brief Get distance to collision with Ray Cast
		 * @param ray_origin ray origin
		 * @param ray_direction ray direction
		 * @param hit_position hit position from ray
		 * 
		 * @return distance of collision
		 */
		float GetRayDistance(const glm::vec3& ray_origin, const glm::vec3& ray_direction, glm::vec3& hit_position) const;

		/** **************************************************************** */
		/* Materials */
    /** **************************************************************** */

		/**
     * @brief Get material
     * @return material pointer
		 */
		RS_Material::RSMaterial* GetMaterial() const;

    /**
     * @brief Set material
     * @param material material pointer
     */
		void SetMaterial(RS_Material::RSMaterial* material);

    /**
     * @brief Set material by name
     * @param name material name
     */
    void SetMaterial(const std::string& name);

    /**
     * @brief Check if the material exists
     * @return exist or not
     */
		bool IsMaterialExist() const;


	private:
    RSMeshData* m_mesh_data = nullptr; ///< Mesh data
    RSBoundingBox m_bounding_box; ///< Bounding box

    RS_Material::RSMaterial* m_material = nullptr; ///< Material
		unsigned int texture_id = 0; // TODO : change to material
    bool is_texture = false; ///< Is texture
	};
}

#endif // !RS_MESH_H_