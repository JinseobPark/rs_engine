/******************************************************************************/
/*!
\file   CameraV2.h
\author Jinseob Park
\date   2024/08/01

This file contains Camera Version 2 base on Camera Base class.
It will be included in Camera class.
This camera looks like the first person view camera.

*/
/******************************************************************************/
#ifndef RS_CAMERA_V2_H_
#define RS_CAMERA_V2_H_

#include "RSCameraBase.h"

/**
 * @brief Camera namespace
 */
namespace RS_Camera
{
	constexpr float YAW = -90.0f;
	constexpr float PITCH = 0.0f;
	constexpr float SPEED = 0.02f;
	constexpr float SENSITIVITY = 0.04f;
	constexpr float ZOOM = 45.0f;

    /**
	 * @brief Camera Movement Enum
     */
    enum class CameraMovement : std::uint8_t
    {
        FORWARD = 0,
        BACKWARD,
        LEFT,
        RIGHT
    };

    /**
	 * @brief Camera version 2. This camera looks like the first person view camera.
	 * TODO : Consider that this camera merge with RSCamera (was CameraV1)
     */
    class CameraV2 final : public RSCameraBase {
    public:
	    explicit CameraV2(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
            : RSCameraBase(pos), front_(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed_(SPEED), mouse_sensitivity_(SENSITIVITY), zoom_(ZOOM)
        {
            world_up_ = up;
            yaw_ = yaw;
            pitch_ = pitch;
            UpdateCameraVectors();
        }

        CameraV2(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw, float pitch)
            : RSCameraBase(glm::vec3(pos_x, pos_y, pos_z)), front_(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed_(SPEED), mouse_sensitivity_(SENSITIVITY), zoom_(ZOOM)
        {
            world_up_ = glm::vec3(up_x, up_y, up_z);
            yaw_ = yaw;
            pitch_ = pitch;
            UpdateCameraVectors();
        }

        /**
         * @brief Get Move value binary
		 * @return unsigned char move_
         */
        RS_N_D unsigned char GetMove() const { return move_; }

        /**
		 * @brief Initialize Camera
         */
        void Initialize() override;

        /**
		 * @brief Key Press Event from Input event
         * @param direction direction value 
         * @see CameraMovement
         */
        void PressKeyboard(CameraMovement direction);

        /**
         * @brief Key Release Event from Input event
         * @param direction direction value
		 * @see CameraMovement
         */
        void ReleaseKeyboard(CameraMovement direction);

        /**
		 * @brief Update Camera Move
		 * @param is_slow is slow mode
		 * Note : slow multiply value is 0.2 
         */
        void UpdateMove(bool is_slow = false);

        /**
		 * @brief Process Mouse Movement
         * @param x_offset x offset from screen space
		 * @param y_offset y offset from screen space
         */
        void ProcessMouseMovement(float x_offset, float y_offset);

        /**
         * @brief Zoom Camera
         * @param y_offset zoom offset value.
         * note : zoom value is 1 ~ 90
         */
        void Zoom(float y_offset);

        /**
         * @brief Update Camera vectors
		 * This include front, right, up vector, movement speed, view matrix
         */
        void UpdateCameraVectors();

        /**
		 * @brief Update Perspective Matrix
         */
        void UpdatePerspectiveMat();

        /**
		 * @brief Update Aspect Ratio
		 * @param width_ width value
		 * @param height_ height value
         */
        void UpdateAspect(int width_, int height_);

    private:
        glm::vec3 front_;
        glm::vec3 up_;
        glm::vec3 right_;
        glm::vec3 world_up_;

        float yaw_;
        float pitch_;

        float movement_speed_;
        float mouse_sensitivity_;
        float zoom_;
        float aspect_ = 1.333f;
        unsigned char move_ = 0x0000;

    };

}

#endif // !RS_CAMERA_V2_H_