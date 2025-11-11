/******************************************************************************/
/*!
\file   RSBoundingBox.h
\author Jinseob Park
\date   2024/08/01

This file contains Bounding Box class for Mesh
*/
/******************************************************************************/
#ifndef RS_BOUNDING_BOX_H_
#define RS_BOUNDING_BOX_H_

#include "glm/glm.hpp"

/**
 * @brief Mesh namespace
 */
namespace RS_Mesh
{
	/**
	 * @brief Bounding Triangle struct
	 * Commonly, 12 triangles are used for bounding box like a cube.
	 */
	struct RSBoundingTriangle
	{
		glm::vec3 v0 = glm::vec3(0.0f);
		glm::vec3 v1 = glm::vec3(0.0f);
		glm::vec3 v2 = glm::vec3(0.0f);
	};

	/**
	 * @brief Bounding Box class for Model object component
	 */
	class RSBoundingBox
	{
	public:
		RSBoundingBox();
		~RSBoundingBox() = default;

    // Deep copy constructor
    RSBoundingBox(const RSBoundingBox& other) = default;

		/**
		 * @brief Set Bounding Box size min and max
		 * @param min min point of bounding box (local)
		 * @param max max point of bounding box (local)
		 */
		void SetMinMax(const glm::vec3& min, const glm::vec3& max);

		/**
		 * @brief Get Min point of bounding box
		 * @return min point of bounding box
		 */
		RS_N_D glm::vec3 GetMin() const { return min; }

		/**
		 * @brief Get Max point of bounding box
		 * @return max point of bounding box
		 */
		RS_N_D glm::vec3 GetMax() const { return max; }

		/**
		 * @brief Return Bounding Box triangles
		 * @return Bounding Box triangles vector of Bounding triangles
		 */
		RS_N_D const std::vector<RSBoundingTriangle>& GetTriangles() const { return m_triangles; }

		/**
		 * @brief Check Collision with Ray Cast
		 * @param ray_origin ray origin
		 * @param ray_direction ray direction
		 * @return true if collision, false if not
		 */
		bool IsCheckCollisionRayCast(const glm::vec3& ray_origin, const glm::vec3& ray_direction) const;

		/**
		 * @brief Draw Bounding Box
		 * It is draw lines immediately.
		 */
		void DrawBoundingBox() const;

	private:
		/**
		 * @brief If min is bigger than max, swap min and max
		 * @param a a
		 * @param b b
		 * @see SetMinMax
		 */
		void SwapData(float *a, float *b);


	private:
    std::vector<RSBoundingTriangle> m_triangles; ///< Bounding Box triangles

    glm::vec3 min = glm::vec3(0.0f); ///< Min point of bounding box
    glm::vec3 max = glm::vec3(0.0f); ///< Max point of bounding box
	};
}

#endif // !RS_BOUNDING_BOX_H_