/******************************************************************************/
/*!
\file   RSSurroundView.cpp
\author Jinseob Park
\date   2024/11/29

Surround View System implementation.

*/
/******************************************************************************/
#include "pch.h"
#include "RSSurroundView.h"
#include <filesystem>

namespace RS_Vision
{
    RSSurroundView::RSSurroundView()
    {
        // Create cameras for each position
        for (int i = 0; i < 4; i++)
        {
            cameras_[i] = std::make_unique<RS_Camera::RSPhysicalCamera>(
                static_cast<RS_Camera::CameraPosition>(i));
        }
    }

    RSSurroundView::~RSSurroundView()
    {
        Shutdown();
    }

    void RSSurroundView::Initialize(const SurroundViewConfig& config)
    {
        config_ = config;

        // Initialize all cameras
        for (auto& camera : cameras_)
        {
            if (camera)
            {
                camera->Initialize();
            }
        }

        // Prepare output image
        surround_view_image_.width = config_.outputWidth;
        surround_view_image_.height = config_.outputHeight;
        surround_view_image_.channels = 3;
        surround_view_image_.data.resize(surround_view_image_.GetDataSize(), 0);

        // Reset processing state
        images_processed_.fill(false);

        is_initialized_ = true;
        RS_MSG("RSSurroundView::Initialize - Initialized with output size: %dx%d", 
               config_.outputWidth, config_.outputHeight);
               
    }

    void RSSurroundView::Shutdown()
    {
        if (surround_view_texture_id_ != 0)
        {
            glDeleteTextures(1, &surround_view_texture_id_);
            surround_view_texture_id_ = 0;
        }

        // Clear images
        for (auto& img : input_images_) img.data.clear();
        for (auto& img : processed_images_) img.data.clear();
        surround_view_image_.data.clear();

        is_initialized_ = false;
    }

    // ==================== Camera Management ====================

    RS_Camera::RSPhysicalCamera* RSSurroundView::GetCamera(RS_Camera::CameraPosition position)
    {
        int idx = GetCameraIndex(position);
        if (idx >= 0 && idx < 4)
        {
            return cameras_[idx].get();
        }
        return nullptr;
    }

    const RS_Camera::RSPhysicalCamera* RSSurroundView::GetCamera(RS_Camera::CameraPosition position) const
    {
        int idx = GetCameraIndex(position);
        if (idx >= 0 && idx < 4)
        {
            return cameras_[idx].get();
        }
        return nullptr;
    }

    bool RSSurroundView::LoadCalibrations(const std::string& calibFolder)
    {
        const char* cameraNames[] = {"front", "rear", "left", "right"};
        int loaded = 0;

        for (int i = 0; i < 4; i++)
        {
            std::string filepath = calibFolder + "/" + cameraNames[i] + "_calib.yaml";
            if (cameras_[i]->LoadCalibration(filepath))
            {
                loaded++;
            }
        }

        RS_MSG("RSSurroundView::LoadCalibrations - Loaded %d/4 calibrations", loaded);
        return loaded == 4;
    }

    bool RSSurroundView::LoadCameraCalibration(RS_Camera::CameraPosition position, const std::string& filepath)
    {
        RS_Camera::RSPhysicalCamera* camera = GetCamera(position);
        if (!camera)
        {
            RS_MSG("RSSurroundView::LoadCameraCalibration - Invalid camera position");
            return false;
        }
        return camera->LoadCalibration(filepath);
    }

    // ==================== Image Processing ====================

    void RSSurroundView::SetCameraImage(RS_Camera::CameraPosition position, const RSImageData& image)
    {
        int idx = GetCameraIndex(position);
        if (idx >= 0 && idx < 4)
        {
            input_images_[idx] = image;
            images_processed_[idx] = false;
        }
    }

    bool RSSurroundView::LoadCameraImage(RS_Camera::CameraPosition position, const std::string& filepath)
    {
        int idx = GetCameraIndex(position);
        if (idx < 0 || idx >= 4)
        {
            RS_MSG("RSSurroundView::LoadCameraImage - Invalid camera position");
            return false;
        }

        bool success = RSImageProcessor::LoadImage(filepath, input_images_[idx]);
        if (success)
        {
            images_processed_[idx] = false;
            
            // Update camera image size
            cameras_[idx]->SetImageSize(input_images_[idx].width, input_images_[idx].height);
        }
        return success;
    }

    int RSSurroundView::LoadAllCameraImages(const std::string& imageFolder, const std::string& extension)
    {
        const char* cameraNames[] = {"front", "rear", "left", "right"};
        int loaded = 0;

        for (int i = 0; i < 4; i++)
        {
            std::string filepath = imageFolder + "/" + cameraNames[i] + extension;
            if (LoadCameraImage(static_cast<RS_Camera::CameraPosition>(i), filepath))
            {
                loaded++;
            }
        }

        RS_MSG("RSSurroundView::LoadAllCameraImages - Loaded %d/4 images", loaded);
        return loaded;
    }

    bool RSSurroundView::ProcessCameraImage(RS_Camera::CameraPosition position)
    {
        int idx = GetCameraIndex(position);
        if (idx < 0 || idx >= 4)
        {
            RS_MSG("RSSurroundView::ProcessCameraImage - Invalid camera position");
            return false;
        }

        if (!input_images_[idx].IsValid())
        {
            RS_MSG("RSSurroundView::ProcessCameraImage - No input image for camera %d", idx);
            return false;
        }

        RS_Camera::RSPhysicalCamera* camera = cameras_[idx].get();
        RSImageData undistorted;

        // Step 1: Undistort
        if (camera->GetDistortionModel() == RS_Camera::DistortionModel::FISHEYE_KANNALA)
        {
            if (!RSImageProcessor::UndistortFisheye(input_images_[idx], undistorted, *camera))
            {
                RS_MSG("RSSurroundView::ProcessCameraImage - Undistort fisheye failed for camera %d", idx);
                // Use original if undistort fails
                undistorted = input_images_[idx];
            }
        }
        else if (camera->GetDistortionModel() != RS_Camera::DistortionModel::NONE)
        {
            if (!RSImageProcessor::Undistort(input_images_[idx], undistorted, *camera))
            {
                RS_MSG("RSSurroundView::ProcessCameraImage - Undistort failed for camera %d", idx);
                undistorted = input_images_[idx];
            }
        }
        else
        {
            // No distortion, use original
            undistorted = input_images_[idx];
        }

        // Step 2: Apply homography (perspective warp to ground plane)
        if (camera->HasHomography())
        {
            if (!RSImageProcessor::WarpPerspective(undistorted, processed_images_[idx],
                                                    camera->GetHomography(),
                                                    config_.outputWidth, config_.outputHeight))
            {
                RS_MSG("RSSurroundView::ProcessCameraImage - Warp perspective failed for camera %d", idx);
                return false;
            }
        }
        else
        {
            // No homography, just resize
            RSImageProcessor::Resize(undistorted, processed_images_[idx],
                                      config_.outputWidth, config_.outputHeight);
        }

        images_processed_[idx] = true;
        RS_MSG("RSSurroundView::ProcessCameraImage - Processed camera %s", camera->GetCameraName().c_str());
        return true;
    }

    bool RSSurroundView::ProcessAllCameraImages()
    {
        bool allSuccess = true;
        for (int i = 0; i < 4; i++)
        {
            if (input_images_[i].IsValid())
            {
                if (!ProcessCameraImage(static_cast<RS_Camera::CameraPosition>(i)))
                {
                    allSuccess = false;
                }
            }
        }
        return allSuccess;
    }

    bool RSSurroundView::GenerateSurroundView()
    {
        // Check if we have any processed images
        bool hasAnyImage = false;
        for (int i = 0; i < 4; i++)
        {
            if (images_processed_[i] && processed_images_[i].IsValid())
            {
                hasAnyImage = true;
                break;
            }
        }

        if (!hasAnyImage)
        {
            RS_MSG("RSSurroundView::GenerateSurroundView - No processed images available");
            return false;
        }

        // Combine images
        CombineImages();

        // Apply blending if enabled
        if (config_.enableBlending)
        {
            BlendRegions();
        }

        RS_MSG("RSSurroundView::GenerateSurroundView - Generated surround view %dx%d", 
               surround_view_image_.width, surround_view_image_.height);
        return true;
    }

    void RSSurroundView::CombineImages()
    {
        // Initialize output with black
        surround_view_image_.width = config_.outputWidth;
        surround_view_image_.height = config_.outputHeight;
        surround_view_image_.channels = 3;
        surround_view_image_.data.resize(surround_view_image_.GetDataSize(), 0);

        // Define regions for each camera (simplified quad layout)
        // Front: top quarter
        // Rear: bottom quarter  
        // Left: left quarter
        // Right: right quarter
        
        struct Region
        {
            int x, y, w, h;
        };

        int w = config_.outputWidth;
        int h = config_.outputHeight;
        int qw = w / 4;
        int qh = h / 4;

        // Regions where each camera contributes (simplified)
        // In a real implementation, these would be based on homography mappings
        Region regions[4] = {
            {qw, 0, w - 2*qw, qh * 2},      // FRONT: top-center
            {qw, h - qh*2, w - 2*qw, qh * 2}, // REAR: bottom-center
            {0, qh, qw * 2, h - 2*qh},      // LEFT: left-center
            {w - qw*2, qh, qw * 2, h - 2*qh}  // RIGHT: right-center
        };

        // Copy each processed image to its region
        for (int camIdx = 0; camIdx < 4; camIdx++)
        {
            if (!images_processed_[camIdx] || !processed_images_[camIdx].IsValid())
                continue;

            const RSImageData& src = processed_images_[camIdx];
            const Region& r = regions[camIdx];

            // Simple copy with scaling
            for (int y = 0; y < r.h; y++)
            {
                for (int x = 0; x < r.w; x++)
                {
                    int dstX = r.x + x;
                    int dstY = r.y + y;
                    
                    if (dstX >= 0 && dstX < w && dstY >= 0 && dstY < h)
                    {
                        // Map to source coordinates
                        int srcX = x * src.width / r.w;
                        int srcY = y * src.height / r.h;

                        if (srcX >= 0 && srcX < src.width && srcY >= 0 && srcY < src.height)
                        {
                            int dstIdx = (dstY * w + dstX) * 3;
                            int srcIdx = (srcY * src.width + srcX) * src.channels;

                            // Only copy if source pixel is not black (simple masking)
                            int brightness = src.data[srcIdx] + src.data[srcIdx + 1] + src.data[srcIdx + 2];
                            if (brightness > 30) // threshold
                            {
                                surround_view_image_.data[dstIdx] = src.data[srcIdx];
                                surround_view_image_.data[dstIdx + 1] = src.data[srcIdx + 1];
                                surround_view_image_.data[dstIdx + 2] = src.data[srcIdx + 2];
                            }
                        }
                    }
                }
            }
        }
    }

    void RSSurroundView::BlendRegions()
    {
        // Simple blending at overlap regions
        // In a real implementation, this would use sophisticated multi-band blending
        
        // For now, just apply a simple blur at the edges
        RSImageData blurred;
        RSImageProcessor::GaussianBlur(surround_view_image_, blurred, 5);
        
        // Blend edges
        int w = surround_view_image_.width;
        int h = surround_view_image_.height;
        int blendW = config_.blendWidth;

        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                // Calculate distance from center (where car would be)
                int cx = w / 2;
                int cy = h / 2;
                int carW = w / 4;
                int carH = h / 3;

                // Check if near the car region boundary
                bool nearCarBoundary = (std::abs(x - cx) > carW/2 - blendW && std::abs(x - cx) < carW/2 + blendW) ||
                                       (std::abs(y - cy) > carH/2 - blendW && std::abs(y - cy) < carH/2 + blendW);

                if (nearCarBoundary && blurred.IsValid())
                {
                    int idx = (y * w + x) * 3;
                    // Blend with blurred version
                    for (int c = 0; c < 3; c++)
                    {
                        surround_view_image_.data[idx + c] = static_cast<unsigned char>(
                            surround_view_image_.data[idx + c] * 0.7f + blurred.data[idx + c] * 0.3f);
                    }
                }
            }
        }
    }

    // ==================== Output ====================

    const RSImageData& RSSurroundView::GetProcessedImage(RS_Camera::CameraPosition position) const
    {
        static RSImageData empty;
        int idx = GetCameraIndex(position);
        if (idx >= 0 && idx < 4)
        {
            return processed_images_[idx];
        }
        return empty;
    }

    unsigned int RSSurroundView::CreateSurroundViewTexture()
    {
        if (!surround_view_image_.IsValid())
        {
            RS_MSG("RSSurroundView::CreateSurroundViewTexture - No surround view image");
            return 0;
        }

        // Delete existing texture
        if (surround_view_texture_id_ != 0)
        {
            glDeleteTextures(1, &surround_view_texture_id_);
        }

        surround_view_texture_id_ = RSImageProcessor::CreateGLTexture(surround_view_image_);
        return surround_view_texture_id_;
    }

    bool RSSurroundView::UpdateSurroundViewTexture(unsigned int textureId)
    {
        if (!surround_view_image_.IsValid() || textureId == 0)
        {
            return false;
        }

        return RSImageProcessor::UpdateGLTexture(textureId, surround_view_image_);
    }

    bool RSSurroundView::SaveSurroundView(const std::string& filepath) const
    {
        return RSImageProcessor::SaveImage(filepath, surround_view_image_);
    }

    // ==================== Configuration ====================

    void RSSurroundView::SetOutputSize(int width, int height)
    {
        config_.outputWidth = width;
        config_.outputHeight = height;

        // Resize output buffer
        surround_view_image_.width = width;
        surround_view_image_.height = height;
        surround_view_image_.data.resize(surround_view_image_.GetDataSize(), 0);

        // Mark all images as needing reprocessing
        images_processed_.fill(false);
    }

    // ==================== Homography Setup ====================

    bool RSSurroundView::SetHomographyPoints(RS_Camera::CameraPosition position,
                                              const std::vector<glm::vec2>& srcPoints,
                                              const std::vector<glm::vec2>& dstPoints)
    {
        RS_Camera::RSPhysicalCamera* camera = GetCamera(position);
        if (!camera)
        {
            return false;
        }

        glm::mat3 H;
        if (RSImageProcessor::ComputeHomography(srcPoints, dstPoints, H))
        {
            camera->SetHomography(H);
            images_processed_[GetCameraIndex(position)] = false;
            return true;
        }
        return false;
    }

    void RSSurroundView::SetHomography(RS_Camera::CameraPosition position, const glm::mat3& H)
    {
        RS_Camera::RSPhysicalCamera* camera = GetCamera(position);
        if (camera)
        {
            camera->SetHomography(H);
            images_processed_[GetCameraIndex(position)] = false;
        }
    }

    void RSSurroundView::SetupDefaultHomographies()
    {
        // Setup default homographies for typical car camera positions
        // These are approximate values - real implementation needs calibration
        
        int w = config_.outputWidth;
        int h = config_.outputHeight;
        int imgW = 1280;  // Assume 1280x720 input
        int imgH = 720;

        // Front camera: maps to top of output
        {
            std::vector<glm::vec2> src = {
                {0, imgH * 0.6f}, {imgW, imgH * 0.6f},
                {imgW * 0.8f, imgH}, {imgW * 0.2f, imgH}
            };
            std::vector<glm::vec2> dst = {
                {0, 0}, {w, 0},
                {w * 0.6f, h * 0.25f}, {w * 0.4f, h * 0.25f}
            };
            SetHomographyPoints(RS_Camera::CameraPosition::FRONT, src, dst);
        }

        // Rear camera: maps to bottom of output
        {
            std::vector<glm::vec2> src = {
                {imgW * 0.2f, 0}, {imgW * 0.8f, 0},
                {imgW, imgH * 0.4f}, {0, imgH * 0.4f}
            };
            std::vector<glm::vec2> dst = {
                {w * 0.4f, h * 0.75f}, {w * 0.6f, h * 0.75f},
                {w, h}, {0, h}
            };
            SetHomographyPoints(RS_Camera::CameraPosition::REAR, src, dst);
        }

        // Left camera: maps to left side of output
        {
            std::vector<glm::vec2> src = {
                {imgW * 0.6f, 0}, {imgW, imgH * 0.2f},
                {imgW, imgH * 0.8f}, {imgW * 0.6f, imgH}
            };
            std::vector<glm::vec2> dst = {
                {0, 0}, {w * 0.25f, h * 0.4f},
                {w * 0.25f, h * 0.6f}, {0, h}
            };
            SetHomographyPoints(RS_Camera::CameraPosition::LEFT, src, dst);
        }

        // Right camera: maps to right side of output
        {
            std::vector<glm::vec2> src = {
                {0, imgH * 0.2f}, {imgW * 0.4f, 0},
                {imgW * 0.4f, imgH}, {0, imgH * 0.8f}
            };
            std::vector<glm::vec2> dst = {
                {w * 0.75f, h * 0.4f}, {w, 0},
                {w, h}, {w * 0.75f, h * 0.6f}
            };
            SetHomographyPoints(RS_Camera::CameraPosition::RIGHT, src, dst);
        }

        RS_MSG("RSSurroundView::SetupDefaultHomographies - Default homographies set");
    }

    // ==================== Helper ====================

    int RSSurroundView::GetCameraIndex(RS_Camera::CameraPosition position)
    {
        return static_cast<int>(position);
    }

} // namespace RS_Vision
