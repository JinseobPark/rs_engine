/******************************************************************************/
/*!
\file   RSSurroundView.h
\author Jinseob Park
\date   2024/11/29

Surround View System for 360-degree bird's eye view generation.
Combines 4 camera images (Front, Rear, Left, Right) into a single top-down view.

*/
/******************************************************************************/
#ifndef RS_SURROUND_VIEW_H_
#define RS_SURROUND_VIEW_H_

#include "RSImageProcessor.h"
#include "Resource/Object/Camera/RSPhysicalCamera.h"
#include <array>
#include <memory>
#include <string>

namespace RS_Vision
{
    /**
     * @brief Surround View configuration parameters
     */
    struct SurroundViewConfig
    {
        int outputWidth = 800;          ///< Output image width
        int outputHeight = 800;         ///< Output image height
        float groundPlaneSize = 10.0f;  ///< Ground plane size in meters
        int blendWidth = 50;            ///< Blend region width in pixels
        bool enableBlending = true;     ///< Enable seamless blending
        bool showCarModel = true;       ///< Show car model overlay
        float carModelScale = 1.0f;     ///< Car model scale
    };

    /**
     * @brief Surround View System class.
     * 
     * Generates a 360-degree bird's eye view by combining 4 fisheye camera images.
     * 
     * Pipeline:
     * 1. Load 4 camera images (Front, Rear, Left, Right)
     * 2. Undistort each image using camera calibration
     * 3. Apply homography to project to ground plane
     * 4. Blend overlapping regions
     * 5. Generate final top-down view texture
     */
    class RSSurroundView
    {
    public:
        RSSurroundView();
        ~RSSurroundView();

        /**
         * @brief Initialize surround view system
         * @param config Configuration parameters
         */
        void Initialize(const SurroundViewConfig& config = SurroundViewConfig());

        /**
         * @brief Shutdown and release resources
         */
        void Shutdown();

        // ==================== Camera Management ====================

        /**
         * @brief Get camera by position
         * @param position Camera position (FRONT, REAR, LEFT, RIGHT)
         * @return Pointer to camera (nullptr if invalid)
         */
        RS_Camera::RSPhysicalCamera* GetCamera(RS_Camera::CameraPosition position);

        /**
         * @brief Get camera by position (const version)
         */
        const RS_Camera::RSPhysicalCamera* GetCamera(RS_Camera::CameraPosition position) const;

        /**
         * @brief Load calibration for all cameras from folder
         * @param calibFolder Folder containing calibration files
         * @return true if all calibrations loaded successfully
         */
        bool LoadCalibrations(const std::string& calibFolder);

        /**
         * @brief Load calibration for specific camera
         * @param position Camera position
         * @param filepath Calibration file path
         * @return true if successful
         */
        bool LoadCameraCalibration(RS_Camera::CameraPosition position, const std::string& filepath);

        // ==================== Image Processing ====================

        /**
         * @brief Set input image for camera
         * @param position Camera position
         * @param image Input image data
         */
        void SetCameraImage(RS_Camera::CameraPosition position, const RSImageData& image);

        /**
         * @brief Load camera image from file
         * @param position Camera position
         * @param filepath Image file path
         * @return true if successful
         */
        bool LoadCameraImage(RS_Camera::CameraPosition position, const std::string& filepath);

        /**
         * @brief Load all camera images from folder
         * @param imageFolder Folder containing camera images
         * @param extension Image file extension (e.g., ".jpg", ".png")
         * @return Number of images loaded
         */
        int LoadAllCameraImages(const std::string& imageFolder, const std::string& extension = ".jpg");

        /**
         * @brief Process single camera image (undistort + homography)
         * @param position Camera position
         * @return true if successful
         */
        bool ProcessCameraImage(RS_Camera::CameraPosition position);

        /**
         * @brief Process all camera images
         * @return true if all processed successfully
         */
        bool ProcessAllCameraImages();

        /**
         * @brief Generate surround view by combining all processed images
         * @return true if successful
         */
        bool GenerateSurroundView();

        // ==================== Output ====================

        /**
         * @brief Get surround view result image
         * @return Surround view image data
         */
        const RSImageData& GetSurroundViewImage() const { return surround_view_image_; }

        /**
         * @brief Get processed image for specific camera
         * @param position Camera position
         * @return Processed image data
         */
        const RSImageData& GetProcessedImage(RS_Camera::CameraPosition position) const;

        /**
         * @brief Create OpenGL texture from surround view
         * @return OpenGL texture ID (0 if failed)
         */
        unsigned int CreateSurroundViewTexture();

        /**
         * @brief Update existing surround view texture
         * @param textureId Existing texture ID
         * @return true if successful
         */
        bool UpdateSurroundViewTexture(unsigned int textureId);

        /**
         * @brief Get surround view texture ID
         * @return Current texture ID (0 if not created)
         */
        unsigned int GetSurroundViewTextureId() const { return surround_view_texture_id_; }

        /**
         * @brief Save surround view image to file
         * @param filepath Output file path
         * @return true if successful
         */
        bool SaveSurroundView(const std::string& filepath) const;

        // ==================== Configuration ====================

        /**
         * @brief Get current configuration
         * @return Configuration parameters
         */
        const SurroundViewConfig& GetConfig() const { return config_; }

        /**
         * @brief Set configuration
         * @param config New configuration
         */
        void SetConfig(const SurroundViewConfig& config) { config_ = config; }

        /**
         * @brief Set output resolution
         * @param width Output width
         * @param height Output height
         */
        void SetOutputSize(int width, int height);

        /**
         * @brief Set blend width for seamless blending
         * @param width Blend width in pixels
         */
        void SetBlendWidth(int width) { config_.blendWidth = width; }

        // ==================== Homography Setup ====================

        /**
         * @brief Set homography points for camera
         * 
         * Define 4 source points in image and corresponding destination points
         * in the ground plane for perspective transformation.
         * 
         * @param position Camera position
         * @param srcPoints 4 source points in image coordinates
         * @param dstPoints 4 destination points in ground plane coordinates
         * @return true if homography computed successfully
         */
        bool SetHomographyPoints(RS_Camera::CameraPosition position,
                                 const std::vector<glm::vec2>& srcPoints,
                                 const std::vector<glm::vec2>& dstPoints);

        /**
         * @brief Set homography matrix directly
         * @param position Camera position
         * @param H 3x3 homography matrix
         */
        void SetHomography(RS_Camera::CameraPosition position, const glm::mat3& H);

        /**
         * @brief Setup default homography for typical car camera positions
         * 
         * Assumes cameras mounted at typical positions (front bumper, rear, side mirrors)
         */
        void SetupDefaultHomographies();

    private:
        // Combine processed images into surround view
        void CombineImages();

        // Blend overlapping regions between adjacent cameras
        void BlendRegions();

        // Get camera index from position
        static int GetCameraIndex(RS_Camera::CameraPosition position);

        // Configuration
        SurroundViewConfig config_;

        // Cameras (Front, Rear, Left, Right)
        std::array<std::unique_ptr<RS_Camera::RSPhysicalCamera>, 4> cameras_;

        // Input images
        std::array<RSImageData, 4> input_images_;

        // Processed images (undistorted + warped)
        std::array<RSImageData, 4> processed_images_;

        // Final surround view image
        RSImageData surround_view_image_;

        // OpenGL texture ID
        unsigned int surround_view_texture_id_ = 0;

        // Processing state
        bool is_initialized_ = false;
        std::array<bool, 4> images_processed_ = {false, false, false, false};
    };

} // namespace RS_Vision

#endif // !RS_SURROUND_VIEW_H_
