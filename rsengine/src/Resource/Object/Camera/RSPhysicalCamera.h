/******************************************************************************/
/*!
\file   RSPhysicalCamera.h
\author Jinseob Park
\date   2024/11/29

Physical Camera Model for Surround View System.
Contains Intrinsic, Distortion, Extrinsic parameters for real camera calibration.

*/
/******************************************************************************/
#ifndef RS_PHYSICAL_CAMERA_H_
#define RS_PHYSICAL_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <string>

/**
 * @brief Camera namespace
 */
namespace RS_Camera
{
    /**
     * @brief Camera position type for Surround View
     */
    enum class CameraPosition : std::uint8_t
    {
        FRONT = 0,
        REAR,
        LEFT,
        RIGHT,
        COUNT
    };

    /**
     * @brief Distortion model type
     */
    enum class DistortionModel : std::uint8_t
    {
        NONE = 0,
        BROWN_CONRADY,      ///< Standard OpenCV model (k1,k2,p1,p2,k3)
        FISHEYE_KANNALA     ///< Fisheye model (k1,k2,k3,k4)
    };

    /**
     * @brief Physical Camera class for real camera calibration data.
     * 
     * This class stores camera intrinsic parameters (K matrix),
     * distortion coefficients, and extrinsic parameters (R, t).
     * Used for Surround View image processing pipeline.
     */
    class RSPhysicalCamera
    {
    public:
        RSPhysicalCamera();
        explicit RSPhysicalCamera(CameraPosition position);
        ~RSPhysicalCamera() = default;

        // ==================== Initialization ====================
        
        /**
         * @brief Initialize camera with default parameters
         */
        void Initialize();

        /**
         * @brief Load calibration data from file
         * @param filepath Path to calibration YAML/JSON file
         * @return true if successful
         */
        bool LoadCalibration(const std::string& filepath);

        /**
         * @brief Save calibration data to file
         * @param filepath Path to save calibration file
         * @return true if successful
         */
        bool SaveCalibration(const std::string& filepath) const;

        // ==================== Intrinsic Parameters ====================

        /**
         * @brief Set intrinsic matrix (K)
         * @param fx Focal length x
         * @param fy Focal length y
         * @param cx Principal point x
         * @param cy Principal point y
         */
        void SetIntrinsic(float fx, float fy, float cx, float cy);

        /**
         * @brief Set intrinsic matrix directly
         * @param K 3x3 intrinsic matrix (column-major)
         */
        void SetIntrinsicMatrix(const glm::mat3& K);

        /**
         * @brief Get intrinsic matrix (K)
         * @return 3x3 intrinsic matrix
         */
        RS_N_D glm::mat3 GetIntrinsicMatrix() const { return intrinsic_K_; }

        /**
         * @brief Get focal length
         * @return vec2(fx, fy)
         */
        RS_N_D glm::vec2 GetFocalLength() const { return glm::vec2(intrinsic_K_[0][0], intrinsic_K_[1][1]); }

        /**
         * @brief Get principal point
         * @return vec2(cx, cy)
         */
        RS_N_D glm::vec2 GetPrincipalPoint() const { return glm::vec2(intrinsic_K_[2][0], intrinsic_K_[2][1]); }

        // ==================== Distortion Parameters ====================

        /**
         * @brief Set distortion model type
         * @param model Distortion model type
         */
        void SetDistortionModel(DistortionModel model) { distortion_model_ = model; }

        /**
         * @brief Get distortion model type
         * @return Current distortion model
         */
        RS_N_D DistortionModel GetDistortionModel() const { return distortion_model_; }

        /**
         * @brief Set Brown-Conrady distortion coefficients
         * @param k1 Radial distortion k1
         * @param k2 Radial distortion k2
         * @param p1 Tangential distortion p1
         * @param p2 Tangential distortion p2
         * @param k3 Radial distortion k3 (optional)
         */
        void SetDistortionBrownConrady(float k1, float k2, float p1, float p2, float k3 = 0.0f);

        /**
         * @brief Set Fisheye (Kannala-Brandt) distortion coefficients
         * @param k1 Distortion coefficient k1
         * @param k2 Distortion coefficient k2
         * @param k3 Distortion coefficient k3
         * @param k4 Distortion coefficient k4
         */
        void SetDistortionFisheye(float k1, float k2, float k3, float k4);

        /**
         * @brief Get distortion coefficients
         * @return Array of distortion coefficients (up to 8)
         */
        RS_N_D const std::array<float, 8>& GetDistortionCoeffs() const { return distortion_D_; }

        // ==================== Extrinsic Parameters ====================

        /**
         * @brief Set extrinsic parameters (rotation and translation)
         * @param R 3x3 rotation matrix
         * @param t Translation vector
         */
        void SetExtrinsic(const glm::mat3& R, const glm::vec3& t);

        /**
         * @brief Set extrinsic from 4x4 transformation matrix
         * @param T 4x4 transformation matrix [R|t]
         */
        void SetExtrinsicMatrix(const glm::mat4& T);

        /**
         * @brief Get rotation matrix
         * @return 3x3 rotation matrix
         */
        RS_N_D glm::mat3 GetRotationMatrix() const { return extrinsic_R_; }

        /**
         * @brief Get translation vector
         * @return Translation vector
         */
        RS_N_D glm::vec3 GetTranslation() const { return extrinsic_t_; }

        /**
         * @brief Get 4x4 extrinsic transformation matrix
         * @return 4x4 [R|t] matrix
         */
        RS_N_D glm::mat4 GetExtrinsicMatrix() const;

        // ==================== Homography ====================

        /**
         * @brief Set homography matrix for ground-plane projection
         * @param H 3x3 homography matrix
         */
        void SetHomography(const glm::mat3& H) { homography_H_ = H; has_homography_ = true; }

        /**
         * @brief Get homography matrix
         * @return 3x3 homography matrix
         */
        RS_N_D glm::mat3 GetHomography() const { return homography_H_; }

        /**
         * @brief Check if homography is set
         * @return true if homography is available
         */
        RS_N_D bool HasHomography() const { return has_homography_; }

        // ==================== Image Size ====================

        /**
         * @brief Set image resolution
         * @param width Image width in pixels
         * @param height Image height in pixels
         */
        void SetImageSize(int width, int height);

        /**
         * @brief Get image width
         * @return Image width in pixels
         */
        RS_N_D int GetImageWidth() const { return image_width_; }

        /**
         * @brief Get image height
         * @return Image height in pixels
         */
        RS_N_D int GetImageHeight() const { return image_height_; }

        // ==================== Utility ====================

        /**
         * @brief Get camera position type
         * @return Camera position (FRONT, REAR, LEFT, RIGHT)
         */
        RS_N_D CameraPosition GetCameraPosition() const { return camera_position_; }

        /**
         * @brief Set camera position type
         * @param position Camera position
         */
        void SetCameraPosition(CameraPosition position) { camera_position_ = position; }

        /**
         * @brief Get camera name string
         * @return Camera name (e.g., "Front", "Rear")
         */
        RS_N_D std::string GetCameraName() const;

        /**
         * @brief Check if calibration data is valid
         * @return true if camera is calibrated
         */
        RS_N_D bool IsCalibrated() const { return is_calibrated_; }

    private:
        // Camera identification
        CameraPosition camera_position_ = CameraPosition::FRONT;

        // Intrinsic parameters (3x3 matrix K)
        // | fx  0  cx |
        // | 0  fy  cy |
        // | 0   0   1 |
        glm::mat3 intrinsic_K_ = glm::mat3(1.0f);

        // Distortion coefficients (up to 8)
        // Brown-Conrady: [k1, k2, p1, p2, k3, k4, k5, k6]
        // Fisheye: [k1, k2, k3, k4, 0, 0, 0, 0]
        std::array<float, 8> distortion_D_ = {0.0f};
        DistortionModel distortion_model_ = DistortionModel::NONE;

        // Extrinsic parameters
        glm::mat3 extrinsic_R_ = glm::mat3(1.0f);  // Rotation matrix
        glm::vec3 extrinsic_t_ = glm::vec3(0.0f);  // Translation vector

        // Homography for ground-plane projection
        glm::mat3 homography_H_ = glm::mat3(1.0f);
        bool has_homography_ = false;

        // Image size
        int image_width_ = 1280;
        int image_height_ = 720;

        // Calibration status
        bool is_calibrated_ = false;
    };

} // namespace RS_Camera

#endif // !RS_PHYSICAL_CAMERA_H_
