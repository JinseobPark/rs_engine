/******************************************************************************/
/*!
\file   RSImageProcessor.cpp
\author Jinseob Park
\date   2024/11/29

Image processing utilities implementation using OpenCV.

*/
/******************************************************************************/
#include "pch.h"
#include "RSImageProcessor.h"
#include "Resource/Object/Camera/RSPhysicalCamera.h"

// OpenCV headers
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/calib3d.hpp>

// OpenGL
#include <GL/glew.h>

namespace RS_Vision
{
    // ==================== Helper Functions ====================

    namespace
    {
        // Convert RSImageData to cv::Mat
        cv::Mat ToMat(const RSImageData& image)
        {
            if (!image.IsValid()) return cv::Mat();

            int type = CV_8UC1;
            if (image.channels == 3) type = CV_8UC3;
            else if (image.channels == 4) type = CV_8UC4;

            cv::Mat mat(image.height, image.width, type);
            std::memcpy(mat.data, image.data.data(), image.GetDataSize());
            return mat;
        }

        // Convert cv::Mat to RSImageData
        void FromMat(const cv::Mat& mat, RSImageData& image)
        {
            if (mat.empty()) return;

            image.width = mat.cols;
            image.height = mat.rows;
            image.channels = mat.channels();
            image.data.resize(image.GetDataSize());
            std::memcpy(image.data.data(), mat.data, image.GetDataSize());
        }

        // Convert glm::mat3 to cv::Mat
        cv::Mat ToCvMat3(const glm::mat3& m)
        {
            cv::Mat mat(3, 3, CV_64F);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    mat.at<double>(i, j) = static_cast<double>(m[j][i]); // GLM is column-major
            return mat;
        }

        // Convert cv::Mat to glm::mat3
        glm::mat3 ToGlmMat3(const cv::Mat& mat)
        {
            glm::mat3 m(1.0f);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    m[j][i] = static_cast<float>(mat.at<double>(i, j));
            return m;
        }
    }

    // ==================== Image I/O ====================

    bool RSImageProcessor::LoadImage(const std::string& filepath, RSImageData& output)
    {
        cv::Mat mat = cv::imread(filepath, cv::IMREAD_COLOR);
        if (mat.empty())
        {
            RS_MSG("RSImageProcessor::LoadImage - Failed to load: %s", filepath.c_str());
            return false;
        }

        FromMat(mat, output);
        RS_MSG("RSImageProcessor::LoadImage - Loaded: %s (%dx%d)", filepath.c_str(), output.width, output.height);
        return true;
    }

    bool RSImageProcessor::SaveImage(const std::string& filepath, const RSImageData& image)
    {
        if (!image.IsValid())
        {
            RS_MSG("RSImageProcessor::SaveImage - Invalid image data");
            return false;
        }

        cv::Mat mat = ToMat(image);
        bool success = cv::imwrite(filepath, mat);
        
        if (success)
            RS_MSG("RSImageProcessor::SaveImage - Saved: %s", filepath.c_str());
        else
            RS_MSG("RSImageProcessor::SaveImage - Failed to save: %s", filepath.c_str());
        
        return success;
    }

    // ==================== Undistortion ====================

    bool RSImageProcessor::Undistort(const RSImageData& input, RSImageData& output,
                                      const RS_Camera::RSPhysicalCamera& camera)
    {
        if (!input.IsValid())
        {
            RS_MSG("RSImageProcessor::Undistort - Invalid input image");
            return false;
        }

        // Check distortion model
        if (camera.GetDistortionModel() == RS_Camera::DistortionModel::FISHEYE_KANNALA)
        {
            return UndistortFisheye(input, output, camera);
        }

        cv::Mat src = ToMat(input);
        cv::Mat dst;

        // Build intrinsic matrix
        glm::mat3 K = camera.GetIntrinsicMatrix();
        cv::Mat cameraMatrix = ToCvMat3(K);

        // Build distortion coefficients
        const auto& D = camera.GetDistortionCoeffs();
        cv::Mat distCoeffs = (cv::Mat_<double>(5, 1) << D[0], D[1], D[2], D[3], D[4]);

        // Undistort
        cv::undistort(src, dst, cameraMatrix, distCoeffs);

        FromMat(dst, output);
        return true;
    }

    bool RSImageProcessor::UndistortFisheye(const RSImageData& input, RSImageData& output,
                                             const RS_Camera::RSPhysicalCamera& camera)
    {
        if (!input.IsValid())
        {
            RS_MSG("RSImageProcessor::UndistortFisheye - Invalid input image");
            return false;
        }

        cv::Mat src = ToMat(input);
        cv::Mat dst;

        // Build intrinsic matrix
        glm::mat3 K = camera.GetIntrinsicMatrix();
        cv::Mat cameraMatrix = ToCvMat3(K);

        // Build fisheye distortion coefficients (4 params)
        const auto& D = camera.GetDistortionCoeffs();
        cv::Mat distCoeffs = (cv::Mat_<double>(4, 1) << D[0], D[1], D[2], D[3]);

        // Use fisheye undistort
        cv::fisheye::undistortImage(src, dst, cameraMatrix, distCoeffs, cameraMatrix);

        FromMat(dst, output);
        return true;
    }

    // ==================== Homography ====================

    bool RSImageProcessor::WarpPerspective(const RSImageData& input, RSImageData& output,
                                            const glm::mat3& H, int outputWidth, int outputHeight)
    {
        if (!input.IsValid())
        {
            RS_MSG("RSImageProcessor::WarpPerspective - Invalid input image");
            return false;
        }

        cv::Mat src = ToMat(input);
        cv::Mat dst;
        cv::Mat homography = ToCvMat3(H);

        cv::warpPerspective(src, dst, homography, cv::Size(outputWidth, outputHeight));

        FromMat(dst, output);
        return true;
    }

    bool RSImageProcessor::ComputeHomography(const std::vector<glm::vec2>& srcPoints,
                                              const std::vector<glm::vec2>& dstPoints,
                                              glm::mat3& H)
    {
        if (srcPoints.size() < 4 || dstPoints.size() < 4)
        {
            RS_MSG("RSImageProcessor::ComputeHomography - Need at least 4 points");
            return false;
        }

        std::vector<cv::Point2f> src, dst;
        for (size_t i = 0; i < srcPoints.size(); i++)
        {
            src.push_back(cv::Point2f(srcPoints[i].x, srcPoints[i].y));
            dst.push_back(cv::Point2f(dstPoints[i].x, dstPoints[i].y));
        }

        cv::Mat homography = cv::findHomography(src, dst);
        if (homography.empty())
        {
            RS_MSG("RSImageProcessor::ComputeHomography - Failed to compute homography");
            return false;
        }

        H = ToGlmMat3(homography);
        return true;
    }

    // ==================== Image Processing ====================

    bool RSImageProcessor::ConvertToGray(const RSImageData& input, RSImageData& output)
    {
        if (!input.IsValid())
        {
            RS_MSG("RSImageProcessor::ConvertToGray - Invalid input image");
            return false;
        }

        cv::Mat src = ToMat(input);
        cv::Mat dst;

        if (input.channels == 1)
        {
            dst = src.clone();
        }
        else if (input.channels == 3)
        {
            cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);
        }
        else if (input.channels == 4)
        {
            cv::cvtColor(src, dst, cv::COLOR_BGRA2GRAY);
        }

        FromMat(dst, output);
        return true;
    }

    bool RSImageProcessor::Resize(const RSImageData& input, RSImageData& output,
                                   int newWidth, int newHeight)
    {
        if (!input.IsValid())
        {
            RS_MSG("RSImageProcessor::Resize - Invalid input image");
            return false;
        }

        cv::Mat src = ToMat(input);
        cv::Mat dst;

        cv::resize(src, dst, cv::Size(newWidth, newHeight));

        FromMat(dst, output);
        return true;
    }

    bool RSImageProcessor::GaussianBlur(const RSImageData& input, RSImageData& output, int kernelSize)
    {
        if (!input.IsValid())
        {
            RS_MSG("RSImageProcessor::GaussianBlur - Invalid input image");
            return false;
        }

        // Ensure kernel size is odd
        if (kernelSize % 2 == 0) kernelSize++;

        cv::Mat src = ToMat(input);
        cv::Mat dst;

        cv::GaussianBlur(src, dst, cv::Size(kernelSize, kernelSize), 0);

        FromMat(dst, output);
        return true;
    }

    // ==================== OpenGL Integration ====================

    unsigned int RSImageProcessor::CreateGLTexture(const RSImageData& image, bool flipVertical)
    {
        if (!image.IsValid())
        {
            RS_MSG("RSImageProcessor::CreateGLTexture - Invalid image data");
            return 0;
        }

        RSImageData processedImage = image;
        
        // Convert BGR to RGB
        if (image.channels >= 3)
        {
            ConvertBGRtoRGB(processedImage);
        }

        // Flip vertically for OpenGL
        if (flipVertical)
        {
            FlipVertical(processedImage);
        }

        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Determine format
        GLenum format = GL_RGB;
        GLenum internalFormat = GL_RGB8;
        if (processedImage.channels == 1)
        {
            format = GL_RED;
            internalFormat = GL_R8;
        }
        else if (processedImage.channels == 4)
        {
            format = GL_RGBA;
            internalFormat = GL_RGBA8;
        }

        // Upload texture
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
                     processedImage.width, processedImage.height, 0,
                     format, GL_UNSIGNED_BYTE, processedImage.data.data());

        glBindTexture(GL_TEXTURE_2D, 0);

        RS_MSG("RSImageProcessor::CreateGLTexture - Created texture ID: %u", textureId);
        return textureId;
    }

    bool RSImageProcessor::UpdateGLTexture(unsigned int textureId, const RSImageData& image, bool flipVertical)
    {
        if (!image.IsValid() || textureId == 0)
        {
            return false;
        }

        RSImageData processedImage = image;

        if (image.channels >= 3)
        {
            ConvertBGRtoRGB(processedImage);
        }

        if (flipVertical)
        {
            FlipVertical(processedImage);
        }

        glBindTexture(GL_TEXTURE_2D, textureId);

        GLenum format = GL_RGB;
        if (processedImage.channels == 1) format = GL_RED;
        else if (processedImage.channels == 4) format = GL_RGBA;

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                        processedImage.width, processedImage.height,
                        format, GL_UNSIGNED_BYTE, processedImage.data.data());

        glBindTexture(GL_TEXTURE_2D, 0);
        return true;
    }

    void RSImageProcessor::ConvertBGRtoRGB(RSImageData& image)
    {
        if (image.channels < 3) return;

        for (size_t i = 0; i < image.data.size(); i += image.channels)
        {
            std::swap(image.data[i], image.data[i + 2]); // Swap B and R
        }
    }

    void RSImageProcessor::FlipVertical(RSImageData& image)
    {
        if (!image.IsValid()) return;

        int rowSize = image.width * image.channels;
        std::vector<unsigned char> temp(rowSize);

        for (int y = 0; y < image.height / 2; y++)
        {
            unsigned char* row1 = image.data.data() + y * rowSize;
            unsigned char* row2 = image.data.data() + (image.height - 1 - y) * rowSize;

            std::memcpy(temp.data(), row1, rowSize);
            std::memcpy(row1, row2, rowSize);
            std::memcpy(row2, temp.data(), rowSize);
        }
    }

    // ==================== Blending ====================

    bool RSImageProcessor::AlphaBlend(const RSImageData& img1, const RSImageData& img2,
                                       RSImageData& output, float alpha)
    {
        if (!img1.IsValid() || !img2.IsValid())
        {
            RS_MSG("RSImageProcessor::AlphaBlend - Invalid input images");
            return false;
        }

        if (img1.width != img2.width || img1.height != img2.height || img1.channels != img2.channels)
        {
            RS_MSG("RSImageProcessor::AlphaBlend - Image dimensions must match");
            return false;
        }

        cv::Mat mat1 = ToMat(img1);
        cv::Mat mat2 = ToMat(img2);
        cv::Mat dst;

        cv::addWeighted(mat1, 1.0 - alpha, mat2, alpha, 0.0, dst);

        FromMat(dst, output);
        return true;
    }

    bool RSImageProcessor::FeatherBlend(const RSImageData& img1, const RSImageData& img2,
                                         RSImageData& output, int blendWidth)
    {
        if (!img1.IsValid() || !img2.IsValid())
        {
            RS_MSG("RSImageProcessor::FeatherBlend - Invalid input images");
            return false;
        }

        // Simple feather blend implementation
        // For more sophisticated blending, consider using cv::detail::FeatherBlender
        
        cv::Mat mat1 = ToMat(img1);
        cv::Mat mat2 = ToMat(img2);
        cv::Mat dst = mat1.clone();

        int centerX = mat1.cols / 2;
        
        for (int y = 0; y < mat1.rows; y++)
        {
            for (int x = 0; x < mat1.cols; x++)
            {
                float alpha = 0.5f;
                
                // Calculate blend weight based on distance from center
                int distFromCenter = std::abs(x - centerX);
                if (distFromCenter < blendWidth)
                {
                    alpha = static_cast<float>(distFromCenter) / blendWidth;
                    if (x < centerX) alpha = 1.0f - alpha;
                }
                else if (x < centerX)
                {
                    alpha = 0.0f;
                }
                else
                {
                    alpha = 1.0f;
                }

                for (int c = 0; c < mat1.channels(); c++)
                {
                    float v1 = mat1.at<cv::Vec3b>(y, x)[c];
                    float v2 = mat2.at<cv::Vec3b>(y, x)[c];
                    dst.at<cv::Vec3b>(y, x)[c] = static_cast<uchar>(v1 * (1.0f - alpha) + v2 * alpha);
                }
            }
        }

        FromMat(dst, output);
        return true;
    }

} // namespace RS_Vision
