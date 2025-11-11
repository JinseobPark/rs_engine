#include "pch.h"
#include "RSCamera.h"
#include "Resource/Manager/RSResourceManager.h"

namespace RS_Camera
{
    void RSCamera::Reset()
    {
        SetPosition(glm::vec3(0.0f, 25.0f, 50.0f));
        SetFocalPoint(glm::vec3(0, 0, 0));
		SetViewUp(glm::vec3(0, 1, 0));
		SetRotateAxis(glm::vec3(0, 0, 0));

#ifdef _SPH_ENGINE
        mouse_rotation_sensitivity_ = 100.0f;
        mouse_move_sensitivity_ = 1.0f;
#else
        mouse_rotation_sensitivity_ = 15.0f;
        mouse_move_sensitivity_ = 0.03f;
#endif
		UpdateCameraVectors();
        target_size_data_[0] = target_size_data_[2] = target_size_data_[4] = FLT_MAX;
        target_size_data_[1] = target_size_data_[3] = target_size_data_[5] = -FLT_MAX;
        center_of_target_ = glm::vec3(0, 0, 0);
        focal_length_ = glm::length(focal_point_ - position_);

    }
    void RSCamera::Initialize()
    {
        SetPosition(glm::vec3(0.0f, 25.0f, 50.0f));
        SetFocalPoint(glm::vec3(0, 0, 0));
        SetViewUp(glm::vec3(0, 1, 0));
        SetRotateAxis(glm::vec3(0, 0, 0));
        UpdateCameraVectors();

        target_size_data_[0] = target_size_data_[2] = target_size_data_[4] = FLT_MAX;
        target_size_data_[1] = target_size_data_[3] = target_size_data_[5] = -FLT_MAX;
        center_of_target_ = glm::vec3(0, 0, 0);
        focal_length_ = glm::length(focal_point_ - position_);
        //focal_length_ = 1.0f;
        //SetCameraByObj(ViewPointType::ISO);
    }

    void RSCamera::ClearTargetData()
    {
        focal_length_ = 0.1f;
        target_size_data_[0] = target_size_data_[2] = target_size_data_[4] = FLT_MAX;
        target_size_data_[1] = target_size_data_[3] = target_size_data_[5] = -FLT_MAX;
        center_of_target_ = glm::vec3(0, 0, 0);
    }

    void RSCamera::AddTargetData(const float* minmax_data)
    {
        for (unsigned int i = 0; i < TARGET_SIZE_COUNT; i++)
            target_size_data_[i] = (i % 2) ? glm::max(target_size_data_[i], minmax_data[i]) : glm::min(target_size_data_[i], minmax_data[i]);
    }

    void RSCamera::DoneAddTargetData()
    {
	    const std::array<float, 6> target_size_data = RSResourceManager::GetInstance()->GetObjectManager()->
        GetTargetSizeData();
	    const glm::vec3 length = {
        target_size_data[1] - target_size_data[0], target_size_data[3] - target_size_data[2],
        target_size_data[5] - target_size_data[4]
      };
        focal_length_ = glm::max(glm::length(length) * 2.0f, 0.1f);
        target_size_ = glm::max(glm::length(length), 0.1f);
        UpdateCameraVectors();
    }


    void RSCamera::ResetTargetFocal()
    {
	    const std::array<float, 6> target_size_data = RSResourceManager::GetInstance()->GetObjectManager()->
        GetTargetSizeData();
        center_of_target_ = glm::vec3(target_size_data[1] + target_size_data[0],
                                      target_size_data[3] + target_size_data[2],
                                      target_size_data[5] + target_size_data[4]) / 2.0f;
        SetFocalPoint(center_of_target_);
        rotate_axis_ = focal_point_;
        UpdateCameraVectors();
    }

    void RSCamera::SetCameraViewPoint(const ViewPointType angle)
    {
	    const std::array<float, 6> target_size_data = RSResourceManager::GetInstance()->GetObjectManager()->
        GetTargetSizeData();
        const glm::vec3 length_ = {
          target_size_data[1] - target_size_data[0], target_size_data[3] - target_size_data[2],
          target_size_data[5] - target_size_data[4]
        };
	    auto direction      = glm::vec3(0, 0, 1);
        focal_length_     = glm::max(glm::length(length_) * 2.0f, 0.1f);
        target_size_      = glm::max(glm::length(length_), 0.1f);
        center_of_target_ = glm::vec3(target_size_data[1] + target_size_data[0],
                                      target_size_data[3] + target_size_data[2],
                                      target_size_data[5] + target_size_data[4]) / 2.0f;
        SetFocalPoint(center_of_target_);
        rotate_axis_ = focal_point_;
        switch (angle)
        {
        case ViewPointType::BACK:
            direction = glm::vec3(0, 0, -1);
            SetViewUp(glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case ViewPointType::FRONT:
            direction = glm::vec3(0, 0, 1);
            SetViewUp(glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case ViewPointType::RIGHT:
            direction = glm::vec3(1, 0, 0);
            SetViewUp(glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case ViewPointType::LEFT:
            direction = glm::vec3(-1, 0, 0);
            SetViewUp(glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case ViewPointType::TOP:
            direction = glm::vec3(0, 1, 0);
            SetViewUp(glm::vec3(0.0f, 0.0f, -1.0f));
            break;
        case ViewPointType::BOTTOM:
            direction = glm::vec3(0, -1, 0);
            SetViewUp(glm::vec3(0.0f, 0.0f, 1.0f));
            break;
        case ViewPointType::ISO:
            direction = glm::normalize(glm::vec3(-1, 1, -1));
            SetViewUp(glm::normalize(glm::vec3(1, 1, 2)));
            break;
        }
        position_ = focal_point_ + direction * focal_length_;
        UpdateCameraVectors();
    }

    void RSCamera::Rotate(const float x_offset, const float y_offset)
    {
	    const glm::vec3 direction = glm::normalize(focal_point_ - position_);
	    const glm::vec3 right = glm::normalize(glm::cross(direction, view_up_));

	    const glm::quat rotation_around_up = glm::angleAxis(glm::radians(x_offset * mouse_rotation_sensitivity_),
                                                          view_up_);
	    const glm::quat rotation_around_right = glm::angleAxis(glm::radians(y_offset * mouse_rotation_sensitivity_),
                                                             right);

	    const glm::quat combined_rotation = rotation_around_up * rotation_around_right; // note the order

        //glm::vec3 offsetFocWithCenter = rotate_axis_ - focal_point_;

	    const glm::vec3 relative_pos = position_ - rotate_axis_;
        position_ = rotate_axis_ + combined_rotation * relative_pos;

	    const glm::vec3 relative_focus = focal_point_ - rotate_axis_;
        focal_point_ = rotate_axis_ + combined_rotation * relative_focus;

        UpdateCameraVectors();
    }

    void RSCamera::Move(const float x_offset, const float y_offset)
    {
	    const float xoffset = x_offset * focal_length_;
	    const float yoffset = y_offset * focal_length_;
        const glm::vec3 direction = glm::normalize(focal_point_ - position_);
        const glm::vec3 right = glm::normalize(glm::cross(direction, view_up_));

        const glm::vec3 right_movement = xoffset * mouse_move_sensitivity_ * right;
        const glm::vec3 up_movement = yoffset * mouse_move_sensitivity_ * view_up_;

        position_ += right_movement - up_movement;
        focal_point_ += right_movement - up_movement;

        UpdateCameraVectors();
    }

    void RSCamera::UpdateAspect(const int width_, const int height_)
    {
        if (width_ != 0 && height_ != 0)
        {
            const float temp_aspect = aspect_;
            const float temp_fov = fov_y_;
            aspect_ = static_cast<float>(width_) / static_cast<float>(height_);
            fov_y_ = 2 * atan(tan(temp_fov / 2) * (temp_aspect / aspect_));
            UpdatePerspectiveMat();
        }
    }

    glm::mat4 RSCamera::CalculateView() const
    {
        float revision = 0;
        glm::vec3 const front(glm::normalize(focal_point_ - position_));
        glm::vec3 const right(glm::normalize(glm::cross(front, view_up_)));
        glm::vec3 const up(glm::cross(right, front));

        if (GetCameraIsOrthographic()) revision = target_size_;
        glm::mat4 result(1.0f);
        result[0][0] = right.x;
        result[1][0] = right.y;
        result[2][0] = right.z;
        result[0][1] = up.x;
        result[1][1] = up.y;
        result[2][1] = up.z;
        result[0][2] = -front.x;
        result[1][2] = -front.y;
        result[2][2] = -front.z;
        result[3][0] = -glm::dot(right, position_);
        result[3][1] = -glm::dot(up, position_);
        result[3][2] = glm::dot(front, position_) - revision;
        //Result[3][2] = glm::dot(front, position_) /*- revision*/; // Not yet apply for Orthogonal camera mode
        return result;
    }

    glm::mat4 RSCamera::CalculateProjection() const
    {
        float const tan_half_fov_y = tan(fov_y_ / static_cast<float>(2));

        glm::mat4 result(0.0f);
        result[0][0] = static_cast<float>(1) / (aspect_ * tan_half_fov_y);
        result[1][1] = static_cast<float>(1) / (tan_half_fov_y);
        result[2][2] = -(far_z_ + near_z_) / (far_z_ - near_z_);
        result[2][3] = -static_cast<float>(1);
        result[3][2] = -(static_cast<float>(2) * far_z_ * near_z_) / (far_z_ - near_z_);
        return result;
    }

    glm::mat4 RSCamera::CalculateOrthogonal() const
    {
	    const float distance = glm::length(focal_point_ - position_);
	    const float scale_factor = glm::tan(fov_y_ * 0.5f) * distance;
        glm::mat4 result(1.0f);
        result[0][0] = static_cast<float>(2) / (2.0f * aspect_ * scale_factor);
        result[1][1] = static_cast<float>(2) / (2.0f * scale_factor);
        result[2][2] = -static_cast<float>(2) / (far_z_ - near_z_);
        result[3][0] = 0.0f; //-(right + left) / (right - left);
        result[3][1] = 0.0f; //-(top + bottom) / (top - bottom);
        result[3][2] = -(far_z_ + near_z_) / (far_z_ - near_z_);

        return glm::ortho(-aspect_ * scale_factor, aspect_ * scale_factor, -scale_factor, scale_factor, near_z_,
                          far_z_);
        return result;
        return glm::mat4();
    }

    void RSCamera::Zoom(const int y_offset)
    {
        // y offset = += 120
        const float sign = static_cast<float>(y_offset) * 0.25f;
        const glm::vec3 direction = glm::normalize(focal_point_ - position_);
        const float distance = glm::length(focal_point_ - position_);

        focal_length_ = distance - sign * distance;
        focal_length_ = glm::max(0.001f, focal_length_);

        position_ = focal_point_ - focal_length_ * direction;
        UpdateCameraVectors();
    }

    glm::mat4 RSCamera::GetInverseProjection() const
    {
        glm::mat4 result = proj_matrix_;

        result = glm::inverse(result);

        return result;
    }

    glm::mat4 RSCamera::GetViewRot() const
    {
        glm::mat4 result = view_matrix_;
        result[3][0] = result[3][1] = result[3][2] = 0;
        return result;
    }

    void RSCamera::UpdateCameraVectors()
    {
	    const glm::vec3 direction = glm::normalize(focal_point_ - position_);
        view_up_ = glm::normalize(glm::cross(glm::cross(direction, view_up_), direction));
        view_matrix_ = CalculateView();
        UpdatePerspectiveMat();

        //std::cout << "near : " << near_z_ << " far : " << far_z_ << "\n";
        //std::cout << "focal_point_ : " << focal_point_.x << " " << focal_point_.y << " " << focal_point_.z << "\n";
        //std::cout << "focal_length_ : " << focal_length_ << "\n";
        //std::cout << "target_size_data_ : " << target_size_data_[0] << " " << target_size_data_[1] << " " << target_size_data_[2] << " " << target_size_data_[3] << " " << target_size_data_[4] << " " << target_size_data_[5] << "\n";
    }

    void RSCamera::UpdatePerspectiveMat()
    {
        const float length_axis_to_target = glm::length(rotate_axis_ - center_of_target_);
        const float position_lgn = glm::length(position_ - rotate_axis_);
      if (IS_FIXED_CAMERA_NEAR_FAR)
      {
        // FIXED TEST
        near_z_ = 0.1f;
        far_z_ = 1000.0f;
      }
      else
      {
        //near_z_ = glm::clamp(positionLgn - length_axis_to_target - target_size_ * 2.0f, CAMERA_MIN_NEAR, CAMERA_MAX_FAR);
        //far_z_ = glm::clamp(positionLgn + length_axis_to_target + target_size_ * 2.0f, near_z_, CAMERA_MAX_FAR);
        near_z_ = glm::clamp((position_lgn - length_axis_to_target) / 100.0f, CAMERA_MIN_NEAR, CAMERA_MAX_FAR);
        far_z_ = glm::clamp((position_lgn + length_axis_to_target) * 10.0f, near_z_, CAMERA_MAX_FAR);
        //far_z_ = glm::clamp((position_lgn + length_axis_to_target) * 100.0f, near_z_, CAMERA_MAX_FAR);
      }

        if (GetCameraIsPerspective())
            proj_matrix_ = CalculateProjection();
        else
            proj_matrix_ = CalculateOrthogonal();
    }

    void RSCamera::SwitchCameraMode()
    {
        view_type_ == RSCameraType::PERSPECTIVE
          ? view_type_ = RSCameraType::ORTHOGRAPHIC
          : view_type_ = RSCameraType::PERSPECTIVE;
        proj_matrix_   = view_type_ == RSCameraType::PERSPECTIVE ? CalculateProjection() : CalculateOrthogonal();
    }

    void RSCamera::SetCameraMode(const RSCameraType mode_)
    {
        view_type_ = mode_;
        proj_matrix_ = view_type_ == RSCameraType::PERSPECTIVE ? CalculateProjection() : CalculateOrthogonal();
    }
    glm::mat4 RSCamera::GetPerspectiveProjection() const
    {
        return CalculateProjection();
    }
}