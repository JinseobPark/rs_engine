/******************************************************************************/
/*!
\file   RSGeometryGenerator.h
\author Jinseob Park
\date   2024/08/01

This file contains the declaration of RSGeometryGenerator class

*/
/******************************************************************************/
#ifndef RS_GEOMETRY_GENERATOR_H_
#define RS_GEOMETRY_GENERATOR_H_

/**
 * @brief Mesh namespace
 */
namespace RS_Mesh
{
	/**
	 * @brief Geometry generator class
	 */
	class RSGeometryGenerator
	{
	public:

		RSGeometryGenerator() = default;
		~RSGeometryGenerator() = default;

		/**
		 * @brief Create Grid mesh
		 * @param rows_ 'rows_' in the grid.
		 * @param cols_ 'cols_' in the grid.
		 * @return Mesh data of grid.
		 */
		RSMeshData* CreateGrid(unsigned int rows_ = 10, unsigned int cols_ = 10);

		/**
		 * @brief Create Plane mesh
		 * @return Mesh data of plane.
		 */
		RSMeshData* CreatePlaneMesh();

		/**
		 * @brief Create Cube Mesh
		 * @param num_sub_divisions_ Number of subdivisions .
		 * @return Mesh data of cube.
		 */
		RSMeshData* CreateCubeMesh(UINT32 num_sub_divisions_ = 0);

		/**
		 * @brief Create Sphere Mesh
		 * @param slice_count_ phi slice count.
		 * @param stack_count_ theta stack count.
		 * @return 
		 */
		RSMeshData* CreateSphereMesh(UINT32 slice_count_ = 20, UINT32 stack_count_ = 20);
		
		/**
		 * @brief Create Sphere tessellated mesh
		 * @param num_sub_divisions_ Number of subdivisions.
		 * @return Sphere tessellated mesh.
		 */
		RSMeshData* CreateSphereTeMesh(UINT32 num_sub_divisions_ = 3);

	private:

    /**
     * @brief Subdivide the mesh
     * @param mesh_ Mesh data to subdivide.
		 */
		void Subdivide (RSMeshData* mesh_);

    /**
     * @brief Midpoint of two vertices
     * @param v0 first vertex
     * @param v1 second vertex
     * @return Midpoint of two vertices.
		 */
		RSVertex MidPoint(const RSVertex& v0, const RSVertex& v1);


	};
}

#endif // !RS_GEOMETRY_GENERATOR_H_