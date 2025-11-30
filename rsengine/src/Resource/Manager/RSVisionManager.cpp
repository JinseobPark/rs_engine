/******************************************************************************/
/*!
\file   RSVisionManager.cpp
\author Jinseob Park
\date   2024/11/29

Vision Manager implementation.

*/
/******************************************************************************/
#include "pch.h"
#include "RSVisionManager.h"

namespace RS_Vision
{
    RSVisionManager* RSVisionManager::m_instance = nullptr;

    RSVisionManager* RSVisionManager::GetInstance()
    {
        if (m_instance == nullptr)
        {
            m_instance = new RSVisionManager();
        }
        return m_instance;
    }

    void RSVisionManager::Release()
    {
        if (m_instance != nullptr)
        {
            delete m_instance;
            m_instance = nullptr;
        }
    }

    RSVisionManager::RSVisionManager()
        : surround_view_(std::make_unique<RSSurroundView>())
    {
    }

    RSVisionManager::~RSVisionManager()
    {
        Shutdown();
    }

    void RSVisionManager::Initialize()
    {
        if (is_initialized_) return;

        RS_MSG("RSVisionManager::Initialize - Initializing Vision Manager");

        // Initialize surround view with default config
        if (surround_view_)
        {
            surround_view_->Initialize();
        }

        is_initialized_ = true;
    }

    void RSVisionManager::Shutdown()
    {
        if (!is_initialized_) return;

        RS_MSG("RSVisionManager::Shutdown - Shutting down Vision Manager");

        if (surround_view_)
        {
            surround_view_->Shutdown();
        }

        is_initialized_ = false;
    }

    // ==================== Surround View ====================

    void RSVisionManager::InitializeSurroundView(const SurroundViewConfig& config)
    {
        if (surround_view_)
        {
            surround_view_->Initialize(config);
        }
    }

    bool RSVisionManager::SetupSurroundViewDemo(const std::string& imageFolder)
    {
        if (!surround_view_)
        {
            RS_MSG("RSVisionManager::SetupSurroundViewDemo - Surround view not initialized");
            return false;
        }

        // Initialize with default config
        SurroundViewConfig config;
        config.outputWidth = 800;
        config.outputHeight = 800;
        config.enableBlending = true;
        surround_view_->Initialize(config);

        // Load images
        int loaded = surround_view_->LoadAllCameraImages(imageFolder);
        if (loaded == 0)
        {
            RS_MSG("RSVisionManager::SetupSurroundViewDemo - No images loaded from: %s", imageFolder.c_str());
            return false;
        }

        // Setup default homographies
        surround_view_->SetupDefaultHomographies();

        // Process all images
        surround_view_->ProcessAllCameraImages();

        // Generate surround view
        bool success = surround_view_->GenerateSurroundView();

        if (success)
        {
            RS_MSG("RSVisionManager::SetupSurroundViewDemo - Demo setup complete");
        }

        return success;
    }

    // ==================== Image Processing ====================

    bool RSVisionManager::LoadImage(const std::string& filepath, RSImageData& output)
    {
        return RSImageProcessor::LoadImage(filepath, output);
    }

    unsigned int RSVisionManager::LoadImageAsTexture(const std::string& filepath)
    {
        RSImageData image;
        if (!RSImageProcessor::LoadImage(filepath, image))
        {
            return 0;
        }
        return RSImageProcessor::CreateGLTexture(image);
    }

    bool RSVisionManager::UndistortImage(const RSImageData& input, RSImageData& output,
                                          const RS_Camera::RSPhysicalCamera& camera)
    {
        return RSImageProcessor::Undistort(input, output, camera);
    }

    bool RSVisionManager::WarpImage(const RSImageData& input, RSImageData& output,
                                     const glm::mat3& H, int width, int height)
    {
        return RSImageProcessor::WarpPerspective(input, output, H, width, height);
    }

    // ==================== Calibration ====================

    bool RSVisionManager::LoadCalibrations(const std::string& calibFolder)
    {
        if (!surround_view_)
        {
            return false;
        }
        return surround_view_->LoadCalibrations(calibFolder);
    }

} // namespace RS_Vision
