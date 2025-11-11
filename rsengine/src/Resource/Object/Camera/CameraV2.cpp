#include "pch.h"
#include "CameraV2.h"

namespace RS_Camera
{
    void CameraV2::Initialize()
    {
        SetPosition(glm::vec3(0, 0, 1.0f));
        yaw_ = YAW;
        pitch_ = PITCH;
        zoom_ = ZOOM;
        UpdateCameraVectors();
    }

    void CameraV2::PressKeyboard(const CameraMovement direction)
    {
        if (direction == CameraMovement::FORWARD)
            move_ |= 0x01;
        if (direction == CameraMovement::BACKWARD)
            move_ |= 0x02;
        if (direction == CameraMovement::LEFT)
            move_ |= 0x04;
        if (direction == CameraMovement::RIGHT)
            move_ |= 0x08;
    }

    void CameraV2::ReleaseKeyboard(const CameraMovement direction)
    {
        if (direction == CameraMovement::FORWARD)
            move_ &= ~0x01;
        if (direction == CameraMovement::BACKWARD)
            move_ &= ~0x02;
        if (direction == CameraMovement::LEFT)
            move_ &= ~0x04;
        if (direction == CameraMovement::RIGHT)
            move_ &= ~0x08;
    }

    void CameraV2::UpdateMove(const bool is_slow)
    {
        float mul_speed = 1.0f;
        if (is_slow) mul_speed = 0.2f;
        if (move_ & 0x01)
            position_ += front_ * movement_speed_ * mul_speed;
        if (move_ & 0x02)
            position_ -= front_ * movement_speed_ * mul_speed;
        if (move_ & 0x04)
            position_ -= right_ * movement_speed_ * mul_speed;
        if (move_ & 0x08)
            position_ += right_ * movement_speed_ * mul_speed;
        UpdateCameraVectors();
    }

    void CameraV2::UpdateAspect(const int width_, const int height_)
    {
        if (width_ != 0 && height_ != 0)
        {
            aspect_ = static_cast<float>(width_) / static_cast<float>(height_);
            UpdatePerspectiveMat();
        }
    }

    void CameraV2::ProcessMouseMovement(float x_offset, float y_offset)
    {
        x_offset *= mouse_sensitivity_;
        y_offset *= mouse_sensitivity_;

        yaw_ += x_offset;
        pitch_ += y_offset;

        if (pitch_ > 89.0f)
            pitch_ = 89.0f;
        if (pitch_ < -89.0f)
            pitch_ = -89.0f;

        // Update
        UpdateCameraVectors();
    }

    void CameraV2::Zoom(const float y_offset)
    {
        zoom_ -= static_cast<float>(y_offset) * 0.0078125f;
        if (zoom_ < 1.0f)
            zoom_ = 1.0f;
        if (zoom_ > 90.0f)
            zoom_ = 90.0f;
    }

    void CameraV2::UpdateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front);
        right_ = glm::normalize(glm::cross(front_, world_up_));
        up_ = glm::normalize(glm::cross(right_, front_));

        movement_speed_ = glm::max(glm::length(position_) * 0.0125f, 0.001f);
        view_matrix_ = glm::lookAt(position_, position_ + front_, up_);
    }

    void CameraV2::UpdatePerspectiveMat()
    {
        proj_matrix_ = glm::perspective(glm::radians(zoom_), aspect_, 0.01f, 1000.0f);
    }

}