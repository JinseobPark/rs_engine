/******************************************************************************/
/*!
\file   ClothSimulatorData.h
\author Jinseob Park
\date   2025/12/01

This file is the game data of Cloth Simulator level.
*/
/******************************************************************************/

#ifndef CLOTH_SIMULATOR_DATA_H_
#define CLOTH_SIMULATOR_DATA_H_

namespace SJG
{
    class ClothSimulatorData
    {
    public:
        ClothSimulatorData();
        ~ClothSimulatorData();

        void Initialize();
        void Shutdown();

        void Update(float dt);

    private:
		RS_Object::RSObject* m_sphere_object;
		RS_Object::RSObject* m_plane_object;
    };
} // namespace SJG

#endif // CLOTH_SIMULATOR_DATA_H_