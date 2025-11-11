#pragma once
#include "pch.h"
#include "glm/glm.hpp"
#include "Systems/Application/RSApplication.h"


/**
 * @brief Util namespace
 */
namespace RS_Util
{
	/**
	 * @brief Get distance between ray and triangle
	 * @param ray_origin ray origin point
	 * @param ray_direction ray direction vector
	 * @param vert0 triangle vertex 0
	 * @param vert1 triangle vertex 1
	 * @param vert2 triangle vertex 2
	 * @return distance between ray and triangle. If there is no intersection, return -1
	 */
	inline float GetDistanceRayTriangle(const glm::vec3& ray_origin, const glm::vec3& ray_direction, const glm::vec3& vert0, const glm::vec3& vert1, const glm::vec3& vert2)
	{
        glm::vec3 const edge1 = vert1 - vert0;
        glm::vec3 const edge2 = vert2 - vert0;

        glm::vec3 const p = glm::cross(ray_direction, edge2);

        float const det = glm::dot(edge1, p);

        glm::vec3 perpendicular(0);

        if (det > FLT_EPSILON)
        {
            glm::vec3 const dist = ray_origin - vert0;

            const float bary_x = glm::dot(dist, p);
            if (bary_x < 0.0f || bary_x > det)
                return -1;

            perpendicular = glm::cross(dist, edge1);

            const float bary_y = glm::dot(ray_direction, perpendicular);
            if ((bary_y < 0.0f) || ((bary_x + bary_y) > det))
                return -1;
        }
        else if (det < -FLT_EPSILON)
        {
            glm::vec3 const dist = ray_origin - vert0;

            const float bary_x = glm::dot(dist, p);
            if ((bary_x > 0.0f) || (bary_x < det))
                return -1;

            perpendicular = glm::cross(dist, edge1);

            const float bary_y = glm::dot(ray_direction, perpendicular);
            if ((bary_y > 0.0f) || ((bary_x + bary_y) < det))
                return -1;
        }
        else
            return -1;

        const float inv_det = static_cast<float>(1) / det;

        return glm::dot(edge2, perpendicular) * inv_det;
	}

	/**
	 * @brief Convert -1 ~ 1 to screen coordinate (0 ~ width, 0 ~ height)
	 * @param vec vector to convert
	 * @return converted vector
     */
    inline glm::vec3 ConvertToScreen(const glm::vec3& vec) {
        glm::vec3 result = (vec + glm::vec3(1.0f, 1.0f, 1.0f)) * 0.5f * glm::vec3(RS_Application::mRSAPPLICATION->GetWidth(), RS_Application::mRSAPPLICATION->GetHeight(), 1.0f);
        result.z = 0;
        return result;
    }

    /**
     * @brief Get Random number exclude 0
	 * @return random number (-50 ~ 50) exclude 0
     */
    inline float GetRandomNumberExcludeZero() {
	    if (const int random_number = rand() % 100; random_number < 50)
			return static_cast<float>(random_number);
		else
			return static_cast<float>(random_number) - 101.0f;
    }

    /**
	 * @brief Get Random number between 0 ~ 1
	 * @return random number (0 ~ 1)
	 */
	inline float GetRandomNumberZeroToOne() {
		const int random_number = rand() % 100 + 1;
			return static_cast<float>(random_number) / 100.0f;
	}

	/**
	 * @brief Convert RSMeshData to RSTriangleMesh
	 * @param mesh_data mesh data to convert
	 * @return converted triangle mesh
     */
    inline RSTriangleMesh ConvertToTriangleMesh(const RSMeshData& mesh_data) {
		RSTriangleMesh triangle_mesh;
        triangle_mesh.triangles.resize(mesh_data.indices.size() / 3);

		for (int i = 0; i < mesh_data.indices.size(); i += 3) {
			RSTriangle triangle;
			triangle.position[0] = mesh_data.vertices[mesh_data.indices[i]].position;
			triangle.position[1] = mesh_data.vertices[mesh_data.indices[i + 1]].position;
			triangle.position[2] = mesh_data.vertices[mesh_data.indices[i + 2]].position;
			triangle.normal = glm::normalize(glm::cross(mesh_data.vertices[mesh_data.indices[i + 1]].position - mesh_data.vertices[mesh_data.indices[i]].position, mesh_data.vertices[mesh_data.indices[i + 2]].position - mesh_data.vertices[mesh_data.indices[i]].position));
           
			triangle_mesh.triangles[i / 3] = triangle;
		}
        _ASSERTE(triangle_mesh.triangles.capacity() == triangle_mesh.triangles.size() && " Error : ConvertToTriangleMesh");

		return triangle_mesh;
	}


    /**
	 * @brief Calculate the intersection point between ray and plane
	 * @param ray_origin_ ray origin point
	 * @param ray_direction_ ray direction vector
	 * @param plane_ plane equation. (x, y, z, w) = (normal.x, normal.y, normal.z, d)
	 * @return 
	 */
	inline float IntersectRayPlane(const glm::vec3& ray_origin_, const glm::vec3& ray_direction_, const glm::vec4& plane_) {

		const float numer = glm::dot(glm::vec3(plane_), ray_origin_) + plane_.w;
		const float denominate = glm::dot(glm::vec3(plane_), ray_direction_);

		if (fabs(denominate) < FLT_EPSILON)
			return -1;

		return -(numer / denominate);

	}


	/**
	 * @brief Build a plane from a point and a normal
	 * @param p_point the point on the plane
	 * @param p_normal the normal of the plane
	 * @return plane equation (x, y, z, w) = (normal.x, normal.y, normal.z, d)
	 */
	inline glm::vec4 BuildPlane(const glm::vec3& p_point, const glm::vec3& p_normal)
	{
		const glm::vec3 normal = glm::normalize(p_normal);
		glm::vec4 plane;
		plane.x = normal.x;
		plane.y = normal.y;
		plane.z = normal.z;
		plane.w = glm::dot(normal, p_point);

		return plane;
	}

	
  /**
   * Malloc dynamic buffer class
   * This class writhed by cppreference.com
	 * https://en.cppreference.com/w/cpp/memory/c/realloc
	 */
	class MallocDynamicBuffer
	{
		char* p;
	public:
		explicit MallocDynamicBuffer(const std::size_t initial = 0) : p(nullptr)
		{
			Resize(initial);
		}

		~MallocDynamicBuffer() { std::free(p); }

		void Resize(const std::size_t new_size)
		{
			if (new_size == 0) // this check is not strictly needed,
			{
				std::free(p); // but zero-size realloc is deprecated in C
				p = nullptr;
			}
			else
			{
				if (void* mem = std::realloc(p, new_size))
					p = static_cast<char*>(mem);
				else
					throw std::bad_alloc();
			}
		}

		char& operator[](const size_t n) { return p[n]; }
		char operator[](const size_t n) const { return p[n]; }
	};



}
