/******************************************************************************/
/*!
\file   RSImageProcessor.h
\author Jinseob Park
\date   2024/11/29

Image processing utilities using OpenCV.
Provides undistortion, homography warping, and OpenGL texture conversion.

*/
/******************************************************************************/
#ifndef RS_IMAGE_PROCESSOR_H_
#define RS_IMAGE_PROCESSOR_H_

#include <string>
#include <vector>
#include <glm/glm.hpp>

// Forward declarations for OpenCV types
namespace cv { class Mat; }

namespace RS_Camera { class RSPhysicalCamera; }

/**
 * @brief Vision namespace for image processing
 */
namespace RS_Vision
{
    /**
     * @brief Image data container (OpenCV-independent interface)
     */
    struct RSImageData
    {
        std::vector<unsigned char> data;  ///< Raw pixel data
        int width = 0;                     ///< Image width
        int height = 0;                    ///< Image height
        int channels = 3;                  ///< Number of channels (1=Gray, 3=BGR, 4=BGRA)

        /**
         * @brief Check if image data is valid
         */
        bool IsValid() const { return !data.empty() && width > 0 && height > 0; }

        /**
         * @brief Get total data size in bytes
         */
        size_t GetDataSize() const { return static_cast<size_t>(width * height * channels); }
    };

    /**
     * @brief Image processor class for vision operations.
     * 
     * Provides OpenCV-based image processing functions:
     * - Image loading/saving
     * - Undistortion (Brown-Conrady, Fisheye)
     * - Homography warping
     * - OpenGL texture conversion
     */
    class RSImageProcessor
    {
    public:
        RSImageProcessor() = default;
        ~RSImageProcessor() = default;

        // ==================== Image I/O ====================

        /**
         * @brief Load image from file
         * @param filepath Path to image file
         * @param output Output image data
         * @return true if successful
         */
        static bool LoadImage(const std::string& filepath, RSImageData& output);

        /**
         * @brief Save image to file
         * @param filepath Path to save image
         * @param image Image data to save
         * @return true if successful
         */
        static bool SaveImage(const std::string& filepath, const RSImageData& image);

        // ==================== Undistortion ====================

        /**
         * @brief Undistort image using camera calibration
         * @param input Input distorted image
         * @param output Output undistorted image
         * @param camera Camera with calibration data
         * @return true if successful
         */
        static bool Undistort(const RSImageData& input, RSImageData& output,
                              const RS_Camera::RSPhysicalCamera& camera);

        /**
         * @brief Undistort fisheye image
         * @param input Input fisheye image
         * @param output Output undistorted image
         * @param camera Camera with fisheye calibration
         * @return true if successful
         */
        static bool UndistortFisheye(const RSImageData& input, RSImageData& output,
                                     const RS_Camera::RSPhysicalCamera& camera);

        // ==================== Homography ====================

        /**
         * @brief Apply homography transformation (perspective warp)
         * @param input Input image
         * @param output Output warped image
         * @param H 3x3 homography matrix
         * @param outputWidth Output image width
         * @param outputHeight Output image height
         * @return true if successful
         */
        static bool WarpPerspective(const RSImageData& input, RSImageData& output,
                                    const glm::mat3& H, int outputWidth, int outputHeight);

        /**
         * @brief Compute homography from 4 point correspondences
         * @param srcPoints Source points (4 points)
         * @param dstPoints Destination points (4 points)
         * @param H Output 3x3 homography matrix
         * @return true if successful
         */
        static bool ComputeHomography(const std::vector<glm::vec2>& srcPoints,
                                      const std::vector<glm::vec2>& dstPoints,
                                      glm::mat3& H);

        // ==================== Image Processing ====================

        /**
         * @brief Convert image to grayscale
         * @param input Input color image
         * @param output Output grayscale image
         * @return true if successful
         */
        static bool ConvertToGray(const RSImageData& input, RSImageData& output);

        /**
         * @brief Resize image
         * @param input Input image
         * @param output Output resized image
         * @param newWidth New width
         * @param newHeight New height
         * @return true if successful
         */
        static bool Resize(const RSImageData& input, RSImageData& output,
                           int newWidth, int newHeight);

        /**
         * @brief Apply Gaussian blur
         * @param input Input image
         * @param output Output blurred image
         * @param kernelSize Kernel size (odd number)
         * @return true if successful
         */
        static bool GaussianBlur(const RSImageData& input, RSImageData& output, int kernelSize = 5);

        // ==================== OpenGL Integration ====================

        /**
         * @brief Create OpenGL texture from image data
         * @param image Image data
         * @param flipVertical Flip image vertically for OpenGL
         * @return OpenGL texture ID (0 if failed)
         */
        static unsigned int CreateGLTexture(const RSImageData& image, bool flipVertical = true);

        /**
         * @brief Update existing OpenGL texture with new image data
         * @param textureId Existing texture ID
         * @param image New image data
         * @param flipVertical Flip image vertically
         * @return true if successful
         */
        static bool UpdateGLTexture(unsigned int textureId, const RSImageData& image, bool flipVertical = true);

        /**
         * @brief Convert BGR to RGB (OpenCV uses BGR, OpenGL uses RGB)
         * @param image Image data to convert (in-place)
         */
        static void ConvertBGRtoRGB(RSImageData& image);

        // ==================== Blending ====================

        /**
         * @brief Alpha blend two images
         * @param img1 First image
         * @param img2 Second image
         * @param output Output blended image
         * @param alpha Blend factor (0.0 = img1, 1.0 = img2)
         * @return true if successful
         */
        static bool AlphaBlend(const RSImageData& img1, const RSImageData& img2,
                               RSImageData& output, float alpha = 0.5f);

        /**
         * @brief Feather blend at boundary region
         * @param img1 First image
         * @param img2 Second image
         * @param output Output blended image
         * @param blendWidth Width of blend region in pixels
         * @return true if successful
         */
        static bool FeatherBlend(const RSImageData& img1, const RSImageData& img2,
                                 RSImageData& output, int blendWidth = 50);

    private:
        // Internal helper functions
        static void FlipVertical(RSImageData& image);
    };

} // namespace RS_Vision

#endif // !RS_IMAGE_PROCESSOR_H_
