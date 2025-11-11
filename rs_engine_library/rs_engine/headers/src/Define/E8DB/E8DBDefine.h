#pragma once

#include "E8Triangle.h"
#include "E8Quad.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace RS_Particle
{
#define PI 3.14159265358979323846f			///<Value of cte PI. 
#define PI_DOUBLE 3.14159265358979323846	///<Value of cte PI. 
#define E8_EPSILON	0.0000001f

	typedef std::vector<glm::ivec2> eiVec2Ar;
	typedef std::vector<glm::ivec3> eiVec3Ar;
	typedef std::vector<glm::ivec4> eiVec4Ar;
	typedef std::vector<glm::fvec2> efVec2Ar;
	typedef std::vector<glm::fvec3> efVec3Ar;
	typedef std::vector<glm::fvec4> efVec4Ar;
	typedef std::vector<glm::dvec2> edVec2Ar;
	typedef std::vector<glm::dvec3> edVec3Ar;
	typedef std::vector<glm::dvec4> edVec4Ar;
	typedef std::vector<glm::fquat> efQuatAr;
	typedef std::vector<glm::dquat> edQuatAr;
	typedef std::vector<glm::fmat2> efMat2Ar;
	typedef std::vector<glm::fmat3> efMat3Ar;
	typedef std::vector<glm::fmat4> efMat4Ar;
	typedef std::vector<glm::dmat2> edMat2Ar;
	typedef std::vector<glm::dmat3> edMat3Ar;
	typedef std::vector<glm::dmat4> edMat4Ar;
	typedef std::vector<int> eIntAr;
	typedef std::vector<unsigned int> eUIntAr;
	typedef std::vector<float> eFloatAr;
	typedef std::vector<double> eDoubleAr;
	typedef std::vector<std::string> eStringAr;

	typedef std::vector<CE8Triangle>	eTriangleAr;
	typedef std::vector<CE8TriangleD>	eTriangleDAr;
	typedef std::vector<CE8Quad>		eQuadAr;
	typedef std::vector<CE8QuadD>		eQuadDAr;
	///////////////////////////////////////////////////////////

	typedef int DB_TYPE;
}
