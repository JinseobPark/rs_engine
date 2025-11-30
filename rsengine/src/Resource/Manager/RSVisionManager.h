/******************************************************************************/
/*!
\file   RSVisionManager.h
\author Jinseob Park
\date   2024/11/29

Vision Manager for managing vision-related resources.
Provides access to image processing and surround view functionality.

*/
/******************************************************************************/
#ifndef RS_VISION_MANAGER_H_
#define RS_VISION_MANAGER_H_

#include "../Vision/RSSurroundView.h"
#include "../Vision/RSImageProcessor.h"
#include <memory>
#include <string>

/**
 * @brief Vision namespace
 */
namespace RS_Vision
{
    /**
     * @brief Vision Manager class.
     * 
     * Manages vision-related resources including:
     * - Surround View System
     * - Image processing utilities
     * - Camera calibration data
     */
    class RSVisionManager
    {
    public:
        /**
         * @brief Get singleton instance
         * @return Pointer to vision manager instance
         */
        static RSVisionManager* GetInstance();

        /**
         * @brief Release singleton instance
         */
        static void Release();

        /**
         * @brief Initialize vision manager
         */
        void Initialize();

        /**
         * @brief Shutdown and release resources
         */
        void Shutdown();

        // ==================== Surround View ====================

        /**
         * @brief Get surround view system
         * @return Pointer to surround view system
         */
        RSSurroundView* GetSurroundView() { return surround_view_.get(); }

        /**
         * @brief Get surround view system (const)
         */
        const RSSurroundView* GetSurroundView() const { return surround_view_.get(); }

        /**
         * @brief Initialize surround view with configuration
         * @param config Surround view configuration
         */
        void InitializeSurroundView(const SurroundViewConfig& config = SurroundViewConfig());

        /**
         * @brief Quick setup surround view demo
         * 
         * Loads sample images and sets up default homographies for testing.
         * 
         * @param imageFolder Folder containing camera images
         * @return true if successful
         */
        bool SetupSurroundViewDemo(const std::string& imageFolder);

        // ==================== Image Processing ====================

        /**
         * @brief Load and process single image
         * @param filepath Image file path
         * @param output Output image data
         * @return true if successful
         */
        bool LoadImage(const std::string& filepath, RSImageData& output);

        /**
         * @brief Create OpenGL texture from image file
         * @param filepath Image file path
         * @return OpenGL texture ID (0 if failed)
         */
        unsigned int LoadImageAsTexture(const std::string& filepath);

        /**
         * @brief Process image with undistortion
         * @param input Input image
         * @param output Output undistorted image
         * @param camera Physical camera with calibration
         * @return true if successful
         */
        bool UndistortImage(const RSImageData& input, RSImageData& output,
                            const RS_Camera::RSPhysicalCamera& camera);

        /**
         * @brief Apply homography warp to image
         * @param input Input image
         * @param output Output warped image
         * @param H Homography matrix
         * @param width Output width
         * @param height Output height
         * @return true if successful
         */
        bool WarpImage(const RSImageData& input, RSImageData& output,
                       const glm::mat3& H, int width, int height);

        // ==================== Calibration ====================

        /**
         * @brief Load calibration from folder
         * @param calibFolder Folder containing calibration files
         * @return true if all calibrations loaded
         */
        bool LoadCalibrations(const std::string& calibFolder);

        // Prevent copying
        RSVisionManager(const RSVisionManager&) = delete;
        RSVisionManager& operator=(const RSVisionManager&) = delete;

    private:
        RSVisionManager();
        ~RSVisionManager();

        static RSVisionManager* m_instance;

        std::unique_ptr<RSSurroundView> surround_view_;
        bool is_initialized_ = false;
    };

} // namespace RS_Vision

#endif // !RS_VISION_MANAGER_H_
