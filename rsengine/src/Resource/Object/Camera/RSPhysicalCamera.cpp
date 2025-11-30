/******************************************************************************/
/*!
\file   RSPhysicalCamera.cpp
\author Jinseob Park
\date   2024/11/29

Physical Camera Model implementation for Surround View System.

*/
/******************************************************************************/
#include "pch.h"
#include "RSPhysicalCamera.h"
#include <fstream>
#include <sstream>

namespace RS_Camera
{
    RSPhysicalCamera::RSPhysicalCamera()
        : camera_position_(CameraPosition::FRONT)
    {
        Initialize();
    }

    RSPhysicalCamera::RSPhysicalCamera(CameraPosition position)
        : camera_position_(position)
    {
        Initialize();
    }

    void RSPhysicalCamera::Initialize()
    {
        // Default intrinsic matrix (assume 1280x720 image)
        // fx = fy = 500, cx = 640, cy = 360
        intrinsic_K_ = glm::mat3(1.0f);
        intrinsic_K_[0][0] = 500.0f;  // fx
        intrinsic_K_[1][1] = 500.0f;  // fy
        intrinsic_K_[2][0] = 640.0f;  // cx
        intrinsic_K_[2][1] = 360.0f;  // cy

        // Clear distortion
        distortion_D_.fill(0.0f);
        distortion_model_ = DistortionModel::NONE;

        // Identity extrinsic
        extrinsic_R_ = glm::mat3(1.0f);
        extrinsic_t_ = glm::vec3(0.0f);

        // Identity homography
        homography_H_ = glm::mat3(1.0f);
        has_homography_ = false;

        // Default image size
        image_width_ = 1280;
        image_height_ = 720;

        is_calibrated_ = false;
    }

    bool RSPhysicalCamera::LoadCalibration(const std::string& filepath)
    {
        // TODO: Implement OpenCV FileStorage loading
        // This will be implemented when OpenCV integration is complete
        // 
        // cv::FileStorage fs(filepath, cv::FileStorage::READ);
        // if (!fs.isOpened()) return false;
        // fs["K"] >> K_mat;
        // fs["D"] >> D_mat;
        // ...
        
        RS_MSG("RSPhysicalCamera::LoadCalibration - Not yet implemented: %s", filepath.c_str());
        return false;
    }

    bool RSPhysicalCamera::SaveCalibration(const std::string& filepath) const
    {
        // TODO: Implement OpenCV FileStorage saving
        RS_MSG("RSPhysicalCamera::SaveCalibration - Not yet implemented: %s", filepath.c_str());
        return false;
    }

    void RSPhysicalCamera::SetIntrinsic(float fx, float fy, float cx, float cy)
    {
        intrinsic_K_ = glm::mat3(1.0f);
        intrinsic_K_[0][0] = fx;
        intrinsic_K_[1][1] = fy;
        intrinsic_K_[2][0] = cx;
        intrinsic_K_[2][1] = cy;
    }

    void RSPhysicalCamera::SetIntrinsicMatrix(const glm::mat3& K)
    {
        intrinsic_K_ = K;
    }

    void RSPhysicalCamera::SetDistortionBrownConrady(float k1, float k2, float p1, float p2, float k3)
    {
        distortion_model_ = DistortionModel::BROWN_CONRADY;
        distortion_D_[0] = k1;
        distortion_D_[1] = k2;
        distortion_D_[2] = p1;
        distortion_D_[3] = p2;
        distortion_D_[4] = k3;
        distortion_D_[5] = 0.0f;
        distortion_D_[6] = 0.0f;
        distortion_D_[7] = 0.0f;
    }

    void RSPhysicalCamera::SetDistortionFisheye(float k1, float k2, float k3, float k4)
    {
        distortion_model_ = DistortionModel::FISHEYE_KANNALA;
        distortion_D_[0] = k1;
        distortion_D_[1] = k2;
        distortion_D_[2] = k3;
        distortion_D_[3] = k4;
        distortion_D_[4] = 0.0f;
        distortion_D_[5] = 0.0f;
        distortion_D_[6] = 0.0f;
        distortion_D_[7] = 0.0f;
    }

    void RSPhysicalCamera::SetExtrinsic(const glm::mat3& R, const glm::vec3& t)
    {
        extrinsic_R_ = R;
        extrinsic_t_ = t;
    }

    void RSPhysicalCamera::SetExtrinsicMatrix(const glm::mat4& T)
    {
        // Extract rotation from 4x4 matrix
        extrinsic_R_[0] = glm::vec3(T[0]);
        extrinsic_R_[1] = glm::vec3(T[1]);
        extrinsic_R_[2] = glm::vec3(T[2]);

        // Extract translation
        extrinsic_t_ = glm::vec3(T[3]);
    }

    glm::mat4 RSPhysicalCamera::GetExtrinsicMatrix() const
    {
        glm::mat4 T(1.0f);
        T[0] = glm::vec4(extrinsic_R_[0], 0.0f);
        T[1] = glm::vec4(extrinsic_R_[1], 0.0f);
        T[2] = glm::vec4(extrinsic_R_[2], 0.0f);
        T[3] = glm::vec4(extrinsic_t_, 1.0f);
        return T;
    }

    void RSPhysicalCamera::SetImageSize(int width, int height)
    {
        image_width_ = width;
        image_height_ = height;

        // Update principal point to image center if not calibrated
        if (!is_calibrated_)
        {
            intrinsic_K_[2][0] = static_cast<float>(width) / 2.0f;
            intrinsic_K_[2][1] = static_cast<float>(height) / 2.0f;
        }
    }

    std::string RSPhysicalCamera::GetCameraName() const
    {
        switch (camera_position_)
        {
        case CameraPosition::FRONT: return "Front";
        case CameraPosition::REAR:  return "Rear";
        case CameraPosition::LEFT:  return "Left";
        case CameraPosition::RIGHT: return "Right";
        default: return "Unknown";
        }
    }

} // namespace RS_Camera
