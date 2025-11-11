/******************************************************************************/
/*!
\file   RSSimplePainter.h
\author Jinseob Park
\date   2024/08/01

This file contains simple painter for drawing the object.

*/
/******************************************************************************/
#ifndef RSSIMPLEPAINTER_H
#define RSSIMPLEPAINTER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief RS_Util namespace
 */
namespace RS_Util
{
	/**
	 * @brief SimplePainter class for drawing immediate mode.
	 * This class is not efficient for drawing the object.
	 */
	class SimplePainter
	{
	public:
		/**
		 * @brief Draw simple quad 2D
		 */
		static void DrawSimpleQuad();

		/**
		 * @brief Draw simple quad line 2D
		 */
		static void DrawSimpleQuadLine();

		/**
		 * @brief Draw simple cube 3D
		 */
		static void DrawSimpleCube();

		/**
		 * @brief Draw simple cube line 3D
		 */
		static void DrawSimpleCubeLine();


		/**
		 * @brief Draw Line given two vertices and color
		 * @param vertex0 first vertex
		 * @param vertex1 second vertex
		 * @param color_ color of the line
		 */
		static void DrawLine(glm::vec3 vertex0, glm::vec3 vertex1, glm::vec3 color_ = glm::vec3(1, 1, 1));

		/**
		 * @brief Draw Lines given vertices and color
		 * @param polygon_ vertices of the polygon
		 * @param color_ color of the line
		 */
		static void DrawLines(const std::vector<glm::vec3>& polygon_, glm::vec3 color_ = glm::vec3(1, 1, 1));

		/**
		 * @brief Draw Quad given 4 vertices and color
		 * @param vertex0 first vertex
		 * @param vertex1 second vertex
		 * @param vertex2 third vertex
		 * @param vertex3 fourth vertex
		 * @param color color of the quad
		 */
		static void DrawQuad(glm::vec3 vertex0, glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3, glm::vec3 color = glm::vec3(1, 1, 1));

		/**
		 * @brief Draw Cube given 8 vertices and color
		 * @param vertex0 first vertex
		 * @param vertex1 second vertex
		 * @param vertex2 third vertex
		 * @param vertex3 fourth vertex
		 * @param vertex4 fifth vertex
		 * @param vertex5 sixth vertex
		 * @param vertex6 seventh vertex
		 * @param vertex7 eighth vertex
		 * @param color color of the cube
		 */
		static void DrawCube(glm::vec3 vertex0, glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3, glm::vec3 vertex4, glm::vec3 vertex5, glm::vec3 vertex6, glm::vec3 vertex7, glm::vec3 color = glm::vec3(1, 1, 1));

		/**
		 * @brief Draw Cube Line given 8 vertices and color
		 * @param vertex0 first vertex
		 * @param vertex1 second vertex
		 * @param vertex2 third vertex
		 * @param vertex3 fourth vertex
		 * @param vertex4 fifth vertex
		 * @param vertex5 sixth vertex
		 * @param vertex6 seventh vertex
		 * @param vertex7 eighth vertex
		 * @param color	  color of the cube
		 */
		static void DrawCubeLine(glm::vec3 vertex0, glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3, glm::vec3 vertex4, glm::vec3 vertex5, glm::vec3 vertex6, glm::vec3 vertex7, glm::vec3 color = glm::vec3(1, 1, 1));

		/**
		 * @brief Draw Cube Line given min and max vertices and color
		 * @param min_ min vertex
		 * @param max_ max vertex
		 * @param color color of the cube
		 */
		static void DrawCubeLine(glm::vec3 min_, glm::vec3 max_, glm::vec3 color = glm::vec3(1, 1, 1));

		/**
		 * @brief Draw Intersect Polygon with Cube.
		 * Point and Normal are for the plane.
		 * Vertices are for the cube.
		 * @param point_ Plane point
		 * @param normal_ Plane normal
		 * @param vertex0 first vertex
		 * @param vertex1 second vertex
		 * @param vertex2 third vertex
		 * @param vertex3 fourth vertex
		 * @param vertex4 fifth vertex
		 * @param vertex5 sixth vertex
		 * @param vertex6 seventh vertex
		 * @param vertex7 eighth vertex
		 */
		static void DrawPolygonIntersectCube(glm::vec3 point_, glm::vec3 normal_, glm::vec3 vertex0, glm::vec3 vertex1, glm::vec3 vertex2, glm::vec3 vertex3, glm::vec3 vertex4, glm::vec3 vertex5, glm::vec3 vertex6, glm::vec3 vertex7);

		/**
		 * @brief Draw Polygon given vertices and color
		 * @param polygon_ vertices of the polygon
		 * @param color_ color of the polygon
		 */
		static void DrawPolygon(const std::vector<glm::vec3>& polygon_, glm::vec3 color_ = glm::vec3(1, 1, 1));

		/**
		 * @brief Draw Polygon given vertices and color
		 * @param points vertices of the polygon
		 * @param size size of the point 
		 */
		static void DrawPoints(const std::vector<glm::vec3>* points, float size = 5.0f);

		/**
		 * @brief Draw Triangles given vertices
		 * @param points vertices of the triangles
		 */
		static void DrawTriangles(const std::vector<glm::vec3>* points);

		/**
		 * @brief Draw Cubes given vertices and size
		 * @param points vertices of the cubes
		 * @param size size of the cube
		 */
		static void DrawCubes(const std::vector<glm::vec3>* points, float size = 0.5f);

		/**
		 * @brief Draw Cube with temporary buffer object
		 */
		static void RenderCube();

		/**
		 * @brief Draw Quad with temporary buffer object
		 */
		static void RenderQuad();

		/**
		 * @brief Draw Sphere with temporary buffer object
		 */
		static void RenderSphere();

		/**
		 * @brief Draw 2D box with vertices
		 * @param vertices_ vertices of the box
		 */
		static void Draw2DBox(float vertices_[]);

	};
}

#endif // !RSSIMPLEPAINTER_H