#ifndef RS_GEOMETRYDATA_H_
#define RS_GEOMETRYDATA_H_
/******************************************************************************/
/*!
\file   RSGeometryData.h
\author Jinseob Park
\date   2024/08/01

This file contains 3D Geometry Data structure for Rendering System.

*/
/******************************************************************************/

#include "glm/glm.hpp"
//#include "E8DB/E8DBDefine.h"
#include <string>
#include <vector>
//#include <unordered_map>
#include <functional>

/**
 * @brief Vertex Data Structure
 * 
 */
struct RSVertex
{
	RSVertex() : position(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), texture_coordinates(glm::vec2(0.0f)) {}
	RSVertex(const glm::vec3 position_, const glm::vec3 normal_, const glm::vec2 texture_coordinates_) : position(position_), normal(normal_), texture_coordinates(texture_coordinates_) {}

  glm::vec3 position; ///< Position
  glm::vec3 normal; ///< Normal
  glm::vec2 texture_coordinates = glm::vec2(0.0f, 0.0f); ///< Texture Coordinates


  /**
   * @brief Compare operator for RSVertex
   * @param other other RSVertex
   * @return true if the two RSVertex is same, otherwise false
	 */
	bool operator==(const RSVertex& other) const
	{
		return position == other.position && normal == other.normal && texture_coordinates == other.texture_coordinates;
	}
};

// Hash function for RSVertex
struct RSVertexHash
{
	std::size_t operator()(const RSVertex& vertex) const
	{
		const std::size_t h1 = std::hash<float>{}(vertex.position.x);
		const std::size_t h2 = std::hash<float>{}(vertex.position.y);
		const std::size_t h3 = std::hash<float>{}(vertex.position.z);
		const std::size_t h4 = std::hash<float>{}(vertex.normal.x);
		const std::size_t h5 = std::hash<float>{}(vertex.normal.y);
		const std::size_t h6 = std::hash<float>{}(vertex.normal.z);
		const std::size_t h7 = std::hash<float>{}(vertex.texture_coordinates.x);
		const std::size_t h8 = std::hash<float>{}(vertex.texture_coordinates.y);

		return h1 ^ h2 ^ h3 ^ h4 ^ h5 ^ h6 ^ h7 ^ h8;
	}
};

/**
 * @brief Vertex Data Structure for STL
 */
struct RSVertexSTL
{
	RSVertexSTL() : position(glm::vec3(0.0f)), normal(glm::vec3(0.0f)) {}
	RSVertexSTL(const glm::vec3 position_, const glm::vec3 normal_) : position(position_), normal(normal_) {}

  glm::vec3 position; ///< Position
  glm::vec3 normal; ///< Normal
};

/**
 * @brief Triangle Data Structure for STL
 */
struct RSTriangle
{
  glm::vec3 position[3]; ///< Position
  glm::vec3 normal; ///< Normal
};


/**
 * @brief Mesh Data Structure
 */
struct RSMeshData
{
  std::vector<RSVertex> vertices; ///< Vertex Data
  std::vector<unsigned int> indices; ///< Index Data
  glm::vec3 min, max; ///< Min, Max Position
  unsigned int vao = 0, vbo = 0, ebo = 0; ///< VAO, VBO, EBO Buffer ID
  bool has_texcoord = false; ///< Has Texture Coordinates
  std::wstring mesh_data_path; ///< Mesh Data Path

  //eTriangleAr GetTriangleData(); // RSVertexSTL > CE8Triangle Convert. NOT USED
};

/**
 * @brief This triangle mesh is used for calculating physics
 */
struct RSTriangleMesh
{
  std::vector<RSTriangle> triangles; ///< Triangle Data

	void Transform(const glm::mat4& model_matrix)
	{
		for (auto& [position, normal] : triangles)
		{
			position[0] = glm::vec3(model_matrix * glm::vec4(position[0], 1.0f));
			position[1] = glm::vec3(model_matrix * glm::vec4(position[1], 1.0f));
			position[2] = glm::vec3(model_matrix * glm::vec4(position[2], 1.0f));
		}
	}
};

#endif // !RS_GEOMETRYDATA_H_