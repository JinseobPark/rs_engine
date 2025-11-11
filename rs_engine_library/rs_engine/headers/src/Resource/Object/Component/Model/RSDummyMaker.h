/******************************************************************************/
/*!
\file   RSDummyMaker.h
\author Jinseob Park
\date   2024/08/01

This file contains the dummy maker for SPH particles.
Not used in the current version.
*/
/******************************************************************************/
#ifndef RS_DUMMY_MAKER_H_
#define RS_DUMMY_MAKER_H_

#include "Define/E8DB/E8DBDefine.h"

namespace RS_Particle
{
	class RSDummyMaker
	{
	public:
		RSDummyMaker() {}
		~RSDummyMaker() {}

	public:
		static void MakeParticlesByRect(const glm::mat4& transform_, float width_, float height_, float spacing_,
                                    efVec3Ar&        positions_);

		static void MakeParticlesByCircle(const glm::mat4& transform_, float radius_, float spacing_, efVec3Ar& positions_);

		static void MakeParticlesBySurface(const eTriangleAr& surfaces_, float spacing_, efVec3Ar& positions_);

	private:
		static bool isInsideSurface(const eTriangleAr& surfaces_, const glm::vec3& position_);

		static float GetDistanceFromSurface(const eTriangleAr& surfaces_, const glm::vec3& position_);
	};

}


#endif // !RS_DUMMY_MAKER_H_