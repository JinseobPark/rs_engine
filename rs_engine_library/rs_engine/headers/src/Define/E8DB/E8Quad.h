#pragma once
#include "pch.h"
#include <glm/glm.hpp>

namespace RS_Particle
{

//?????
class CE8Quad
{

public:
	/**
	@brief 생성자
	*/
	CE8Quad()
	{
		Initialize();
	}

	/**
	@brief 소멸자
	*/
	~CE8Quad() {};

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
		return glm::length(glm::cross(vtx1 - vtx2, vtx3 - vtx2));
	}

	/**
	@brief 사각형의 최대 값을 가져온다.
	@return glm::fvec3	삼각형의 최대 값
	*/
	glm::fvec3 GetMax()
	{
		glm::fvec3 triMax(-FLT_MAX);
		triMax = glm::max(triMax, vtx1);
		triMax = glm::max(triMax, vtx2);
		triMax = glm::max(triMax, vtx3);
		triMax = glm::max(triMax, vtx4);
		return triMax;
	}

	/**
	@brief 사각형의 최소 값을 가져온다.
	@return glm::fvec3	사각형의 최소 값
	*/
	glm::fvec3 GetMin()
	{
		glm::fvec3 triMin(FLT_MAX);
		triMin = glm::min(triMin, vtx1);
		triMin = glm::min(triMin, vtx2);
		triMin = glm::min(triMin, vtx3);
		triMin = glm::min(triMin, vtx4);
		return triMin;
	}

public:
	glm::fvec3 vtx1;	  ///<	@brief	사각형 첫번째 점
	glm::fvec3 vtx2;	  ///<	@brief	사각형 두번째 점
	glm::fvec3 vtx3;	  ///<	@brief	사각형 세번째 점
	glm::fvec3 vtx4;	  ///<	@brief	사각형 네번째 점
	glm::fvec3 normal;	  ///<	@brief	사각형 법선 벡터
};


class CE8QuadD
{

public:
	/**
	@brief 생성자
	*/
	CE8QuadD()
	{
		Initialize();
	}

	/**
	@brief 소멸자
	*/
	~CE8QuadD() {};

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
		return glm::length(glm::cross(vtx1 - vtx2, vtx3 - vtx2));
	}

	/**
	@brief 사각형의 최대 값을 가져온다.
	@return glm::fvec3	삼각형의 최대 값
	*/
	glm::dvec3 GetMax()
	{
		glm::dvec3 triMax(-FLT_MAX);
		triMax = glm::max(triMax, vtx1);
		triMax = glm::max(triMax, vtx2);
		triMax = glm::max(triMax, vtx3);
		triMax = glm::max(triMax, vtx4);
		return triMax;
	}

	/**
	@brief 사각형의 최소 값을 가져온다.
	@return glm::fvec3	사각형의 최소 값
	*/
	glm::dvec3 GetMin()
	{
		glm::dvec3 triMin(FLT_MAX);
		triMin = glm::min(triMin, vtx1);
		triMin = glm::min(triMin, vtx2);
		triMin = glm::min(triMin, vtx3);
		triMin = glm::min(triMin, vtx4);
		return triMin;
	}

public:
	glm::dvec3 vtx1;	  ///<	@brief	사각형 첫번째 점
	glm::dvec3 vtx2;	  ///<	@brief	사각형 두번째 점
	glm::dvec3 vtx3;	  ///<	@brief	사각형 세번째 점
	glm::dvec3 vtx4;	  ///<	@brief	사각형 네번째 점
	glm::dvec3 normal;	  ///<	@brief	사각형 법선 벡터
};

}
