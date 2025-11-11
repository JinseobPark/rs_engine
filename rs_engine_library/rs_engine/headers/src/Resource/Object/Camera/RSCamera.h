/******************************************************************************/
/*!
\file   RSCamera.h
\author Jinseob Park
\date   2024/08/01

This file contains Camera class base on Camera Base class.
It has more detailed functions for Camera.

*/
/******************************************************************************/
#ifndef RS_CAMERA_H_
#define RS_CAMERA_H_

#include "RSCameraBase.h"
#include <vector>

#define IS_FIXED_CAMERA_NEAR_FAR true

/**
 * @brief GUI Handler namespace
 */
namespace RS_Handler
{
    class RSImguiHandler;
}

/**
 * @brief Camera namespace
 */
namespace RS_Camera
{
    /**
	 * @brief Main Camera class. It derives from RSCameraBase.
     */
    class RSCamera final : public RSCameraBase {
    public:
#ifdef _SPH_ENGINE
        RSCamera() : RSCameraBase(glm::vec3(0.0f, 0.0f, 3.0f)),
                     rotate_axis_(0.0f, 0.0f, 0.0f),
                     focal_point_(0.0f, 0.0f, -1.0f),
                     view_up_(0.0f, 1.0f, 0.0f),
                     mouse_rotation_sensitivity_(100.0f),
                     mouse_move_sensitivity_(1.0f),
                     zoom_sensitivity_(1.0f), target_size_data_(),
                     focal_length_(glm::length(focal_point_ - position_))
        {
        }

        RSCamera(glm::vec3 pos, glm::vec3 center, glm::vec3 focal, glm::vec3 up)
	        : RSCameraBase(pos), rotate_axis_(center), focal_point_(focal), view_up_(up),
	          mouse_rotation_sensitivity_(100.0f), mouse_move_sensitivity_(1.0f), zoom_sensitivity_(1.0f),
	          target_size_data_(), focal_length_(glm::length(focal_point_ - position_))
        {
        }

        ~RSCamera() override {}
#else
        RSCamera() : RSCameraBase(glm::vec3(0.0f, 0.0f, 3.0f)),
            rotate_axis_(0.0f, 0.0f, 0.0f),
            focal_point_(0.0f, 0.0f, -1.0f),
            view_up_(0.0f, 1.0f, 0.0f),
            mouse_rotation_sensitivity_(15.0f),
            mouse_move_sensitivity_(0.03f),
            focal_length_(glm::length(focal_point_ - position_)),
            zoom_sensitivity_(1.0f) {
        }

        RSCamera(glm::vec3 pos, glm::vec3 center, glm::vec3 focal, glm::vec3 up)
            : RSCameraBase(pos), rotate_axis_(center), focal_point_(focal), view_up_(up), mouse_rotation_sensitivity_(15.0f), mouse_move_sensitivity_(0.03f), focal_length_(glm::length(focal_point_ - position_)), zoom_sensitivity_(1.0f) {
        }
#endif
        /**
		 * @brief Reset Camera to default value
         */
        void Reset();
        /**
         * @brief Initialize Camera
         */
        void Initialize() override;
        /**
         * @brief Set Focal Point
         * @param[in] focal focus point (vec3)
         */
        void SetFocalPoint(const glm::vec3 focal) { focal_point_ = focal; }
        /**
		 * @brief Set View Up
		 * @param[in] up view up point (vec3)
		 */
        void SetViewUp(const glm::vec3 up) { view_up_ = up; }
        /**
		 * @brief Set Rotate Axis
		 * @param rotate_axis axis for rotation (vec3)
         */
        void SetRotateAxis(const glm::vec3 rotate_axis) { rotate_axis_ = rotate_axis; }
        /**
		 * @brief Set Focus point (same as SetFocalPoint)
		 * @param point focus point (vec3)
         */
        void LookAtPoint(const glm::vec3 point) { SetFocalPoint(point); }

        /**
         * @brief Get forward direction
         * 
         * @return forward direction (vec3)
         */
        glm::vec3 GetForwardDir() const { return glm::normalize(focal_point_ - position_);}


        /**
         * @brief Get Near
		 * @return near z value
         */
        RS_N_D float GetNear() const { return near_z_; }
        /**
         * @brief Get Far
         * @return far z value
         */
        RS_N_D float GetFar() const { return far_z_; }

        /**
		 * @brief Clear Target Data. It is used for target data calculation
		 * Center of target set to zero point.
		 * focal length set to default value (0.1f).
		 * target size set to maximum.
		 * 
		 * @return void
         */
        void ClearTargetData();

        /**
		 * @brief Add data to calculate target data
		 * @param minmax_data minmax data (6 float )for target calculation
         */
        void AddTargetData(const float* minmax_data);

        /**
		 * @brief When all target data is added, this function should be called.
         * @see UpdateCameraVectors
         */
        void DoneAddTargetData();

        /**
         * @brief Reset Target Focal point
         */
        void ResetTargetFocal();

        /**
         * @brief Set camera view point by viewpoint type
         * @param angle ViewPointType 
         */
        void SetCameraViewPoint(const ViewPointType angle);

        /**
		 * @brief Get Focal Point
		 * @return focal point (vec3)
         */
        RS_N_D glm::vec3 GetFocal() const { return focal_point_; }

        /**
		 * @brief Get View Up
		 * @return view up (vec3)
         */
        RS_N_D glm::vec3 GetViewUp() const { return view_up_; }

        /**
		 * @brief Get Rotate Axis
		 * @return rotate axis (vec3)
         */
        RS_N_D glm::vec3 GetRotateAxis() const { return rotate_axis_; }

        /**
		 * @brief Get Focal Length
		 * @return focal length
         */
        RS_N_D float GetFocalLength() const { return focal_length_; }


        /**
		 * @brief Get Fov
		 * @return fov value
		 */
		float GetFovY() const { return fov_y_; }

        /**
		 * @brief Rotate Camera by mouse right click and drag (default)
		 * @param x_offset mouse x offset
		 * @param y_offset mouse y offset
         */
        void Rotate(float x_offset, float y_offset);
        
        /**
         * @brief Move camera by mouse middle click and drag default)
		 * @param x_offset mouse x offset
		 * @param y_offset mouse y offset
         */
        void Move(float x_offset, float y_offset);

        /**
		 * @brief Zoom in and out by mouse wheel
		 * @param y_offset mouse wheel offset
         */
        void Zoom(int y_offset);

        /**
         * @brief Get Camera projection inverse matrix
		 * @return inverse projection matrix
         */
        RS_N_D glm::mat4 GetInverseProjection() const;

        /**
		 * @brief update camera for window resize event. It would be changed aspect and fov
         * @param width_ window width size
		 * @param height_  window height size
         */
        void UpdateAspect(int width_, int height_);

        /**
         * @brief Calculate View matrix
         * @return view matrix (mat4)
         */
        RS_N_D glm::mat4 CalculateView() const;

        /**
		 * @brief Calculate Projection matrix
		 * @return projection matrix (mat4)
		 */
        RS_N_D glm::mat4 CalculateProjection() const;

        /**
		 * @brief Calculate Orthogonal matrix
		 * @return orthogonal matrix (mat4)
         */
        RS_N_D glm::mat4 CalculateOrthogonal() const;

        /**
		 * @brief Get View Rotation matrix (not include camera position)
		 * @return view rotation matrix (mat4)
         */
        RS_N_D glm::mat4 GetViewRot() const;

        /**
		 * @brief Update camera vectors(view up, view, projection matrix) by camera position and focal point
		 * @see CalculateView
		 * @see UpdatePerMat
         */
        void UpdateCameraVectors();

        /**
		 * @brief Update camera projection matrix include near and far z value
		 * @see CalculateProjection
		 * @see CalculateOrthogonal
         */
        void UpdatePerspectiveMat();

        /**
		 * @brief switch camera mode (orthogonal, perspective)
         */
        void SwitchCameraMode();

        /**
		 * @brief Set Camera Mode (orthogonal, perspective)
         * @param mode_ 
         */
        void SetCameraMode(RSCameraType mode_);


        /**
         * @brief Set Rotation Sensitivity
         * @param sensitivity rotation sensitivity
         */
        void SetRotationSensitivity(const float sensitivity) { mouse_rotation_sensitivity_ = sensitivity; }


        /**
         * @brief Set Move Sensitivity
         * @param sensitivity move sensitivity
         */
        void SetMoveSensitivity(const float sensitivity) { mouse_move_sensitivity_ = sensitivity; }


        /**
         * @brief Get perspective projection matrix
         * @return perspective projection matrix (mat4)
         */
        RS_N_D glm::mat4 GetPerspectiveProjection() const;


    private:
      glm::vec3 rotate_axis_; ///< rotate axis
      glm::vec3 focal_point_; ///< focal point
      glm::vec3 view_up_; ///< view up

      float near_z_ = CAMERA_MIN_NEAR; ///< near z value
      float far_z_ = CAMERA_MAX_FAR; ///< far z value
      float aspect_ = 1.333f; ///< aspect ratio
      float fov_y_ = glm::radians(30.0f); ///< fov value

      float mouse_rotation_sensitivity_; ///< mouse rotation sensitivity
      float mouse_move_sensitivity_; ///< mouse move sensitivity
      float zoom_sensitivity_; ///< zoom sensitivity

      bool view_dirty_ = true; ///< view dirty flag
      bool changed_z_ = false; ///< changed z flag
      std::array<float, 6> target_size_data_; ///< target size data
      glm::vec3 center_of_target_ = glm::vec3(0.0f); ///< center of target
      float target_size_ = 1.0f; ///< target size 
      float focal_length_;  ///< focal length
      friend class RS_Handler::RSImguiHandler; ///< friend class
    };

}

#endif // !RS_CAMERA_H_