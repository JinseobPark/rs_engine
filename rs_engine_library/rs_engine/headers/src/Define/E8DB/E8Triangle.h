#pragma once
#include "pch.h"
#include <glm/glm.hpp>

#ifdef max
#undef max
#undef min
#endif

namespace RS_Particle
{
/**
@class	CE8Triangle		E8Triangle.h	"include/E8Triangle.h"
@version	1.0
@author	송상민/수석연구원
@brief	삼각형 데이터
@details	삼각형을 정의하고 삼각형를 활용할 수 있는 함수들이 정의되어 있음.
*/
class CE8Triangle
{
public:
	/**
	@brief 생성자
	*/
	CE8Triangle()
	{
		Initialize();
	}

	/**
	@brief 소멸자
	*/
	~CE8Triangle() {};

	/**
	@brief 초기화
	*/
	void Initialize()
	{
		vtx1 = vtx2 = vtx3 = normal = glm::vec3();
	}

	/**
	@brief 법선 벡터 계산
	@return glm::fvec3& 법선 벡터 결과
	*/
	glm::fvec3& Normal()
	{
		normal = glm::normalize(glm::cross(vtx1 - vtx2, vtx3 - vtx2));
		return normal;
	}

	/**
	@brief 면적 계산
	@return float 면적
	*/
	float GetArea()
	{
		return glm::length(glm::cross(vtx1 - vtx2, vtx3 - vtx2)) / 2.0f;
	}

	/**
	@brief 삼각형의 최대 값을 가져온다.
	@return glm::fvec3	삼각형의 최대 값
	*/
	glm::fvec3 GetMax()
	{
		glm::fvec3 triMax(-FLT_MAX);
		triMax = glm::max(triMax, vtx1);
		triMax = glm::max(triMax, vtx2);
		triMax = glm::max(triMax, vtx3);
		return triMax;
	}

	/**
	@brief 삼각형의 최소 값을 가져온다.
	@return glm::fvec3	삼각형의 최소 값
	*/
	glm::fvec3 GetMin()
	{
		glm::fvec3 triMin(FLT_MAX);
		triMin = glm::min(triMin, vtx1);
		triMin = glm::min(triMin, vtx2);
		triMin = glm::min(triMin, vtx3);
		return triMin;
	}

	/**
	@brief 입력된 위치가 삼각형 내부에 있는지 검사한다.
	@param[in] triangle		삼각망 표면
	@param[in] pos			위치
	@param[in] normalDist	수선 거리
	@return bool	위치 (true : 내부, false : 외부)
	*/
	static bool IsInsideTriangle(const CE8Triangle& triangle, const glm::fvec3& pos, float& normalDist)
	{
		normalDist = glm::dot(triangle.normal, pos - triangle.vtx1);  // 수선 거리
		glm::fvec3 posNormal = pos - triangle.normal * normalDist;

		if (glm::dot(glm::cross(triangle.vtx1 - posNormal, triangle.vtx2 - posNormal), triangle.normal) < -0.00001f ||
			glm::dot(glm::cross(triangle.vtx2 - posNormal, triangle.vtx3 - posNormal), triangle.normal) < -0.00001f ||
			glm::dot(glm::cross(triangle.vtx3 - posNormal, triangle.vtx1 - posNormal), triangle.normal) < -0.00001f)
			return false;
		return true;
	}

public:
	glm::fvec3 vtx1;	  ///<	@brief	삼각형 첫번째 점
	glm::fvec3 vtx2;	  ///<	@brief	삼각형 두번째 점
	glm::fvec3 vtx3;	  ///<	@brief	삼각형 세번째 점
	glm::fvec3 normal;	  ///<	@brief	삼각형 법선 벡터
};


class CE8TriangleD
{
public:
	/**
	@brief 생성자
	*/
	CE8TriangleD()
	{
		Initialize();
	}

	/**
	@brief 소멸자
	*/
	~CE8TriangleD() {};

	/**
	@brief 초기화
	*/
	void Initialize()
	{
		vtx1 = vtx2 = vtx3 = normal = glm::dvec3();
	}

	/**
	@brief 법선 벡터 계산
	@return glm::fvec3& 법선 벡터 결과
	*/
	glm::dvec3& Normal()
	{
		normal = glm::normalize(glm::cross(vtx1 - vtx2, vtx3 - vtx2));
		return normal;
	}

	/**
	@brief 면적 계산
	@return float 면적
	*/
	double GetArea()
	{
		return glm::length(glm::cross(vtx1 - vtx2, vtx3 - vtx2)) / 2.0;
	}

	/**
	@brief 삼각형의 최대 값을 가져온다.
	@return glm::fvec3	삼각형의 최대 값
	*/
	glm::dvec3 GetMax()
	{
		glm::dvec3 triMax(-FLT_MAX);
		triMax = glm::max(triMax, vtx1);
		triMax = glm::max(triMax, vtx2);
		triMax = glm::max(triMax, vtx3);
		return triMax;
	}

	/**
	@brief 삼각형의 최소 값을 가져온다.
	@return glm::fvec3	삼각형의 최소 값
	*/
	glm::dvec3 GetMin()
	{
		glm::dvec3 triMin(FLT_MAX);
		triMin = glm::min(triMin, vtx1);
		triMin = glm::min(triMin, vtx2);
		triMin = glm::min(triMin, vtx3);
		return triMin;
	}

	/**
	@brief 입력된 위치가 삼각형 내부에 있는지 검사한다.
	@param[in] triangle		삼각망 표면
	@param[in] pos			위치
	@param[in] normalDist	수선 거리
	@return bool	위치 (true : 내부, false : 외부)
	*/
	static bool IsInsideTriangle(const CE8TriangleD& triangle, const glm::dvec3& pos, double& normalDist)
	{
		normalDist = glm::dot(triangle.normal, pos - triangle.vtx1);  // 수선 거리
		glm::dvec3 posNormal = pos - triangle.normal * normalDist;

		if (glm::dot(glm::cross(triangle.vtx1 - posNormal, triangle.vtx2 - posNormal), triangle.normal) < -0.00001f ||
			glm::dot(glm::cross(triangle.vtx2 - posNormal, triangle.vtx3 - posNormal), triangle.normal) < -0.00001f ||
			glm::dot(glm::cross(triangle.vtx3 - posNormal, triangle.vtx1 - posNormal), triangle.normal) < -0.00001f)
			return false;
		return true;
	}

public:
	glm::dvec3 vtx1;	  ///<	@brief	삼각형 첫번째 점
	glm::dvec3 vtx2;	  ///<	@brief	삼각형 두번째 점
	glm::dvec3 vtx3;	  ///<	@brief	삼각형 세번째 점
	glm::dvec3 normal;	  ///<	@brief	삼각형 법선 벡터
};

}
