#include "pch.h"
#include "RSImguiHandler.h"
#include "../Systems/Application/RSApplication.h"
#include "../Systems/EventHandler/RSEventHandler.h"
#include "../Systems/Logic/LogicClass.h"
#include "../Systems/Graphics/RSGraphics.h"
#include "RSEngine.h"
#include "Particle/VTKViewer/RSVTKViewer.h"
#include "Particle/RSClothSimulator.h"
//#include "imgui/imgui.h"
//#include "imgui/imgui_impl_glfw.h"
//#include "imgui/imgui_impl_opengl3.h"
//#include "imgui/ImGuiFileDialog.h"

#pragma execution_character_set("utf-8")
#define IMGUI_DOCKING true

namespace RS_Handler
{
  using namespace RS_Event;

  RSImguiHandler* mRSIMGUIHANDLER = nullptr;


  RSImguiHandler::RSImguiHandler()
  {
    RS_ASSERT(mRSIMGUIHANDLER == nullptr, "RSImguiHandler is a singleton class and should only be created once.");
    mRSIMGUIHANDLER = this;

    p_window = nullptr;
  }

  RSImguiHandler::~RSImguiHandler() = default;

  void RSImguiHandler::Initialize()
  {
    p_window = RS_Application::mRSAPPLICATION->GetWindow();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();


    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.WantCaptureMouse = true;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
  	// Get Korean Font from windows system
	  m_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

    //m_font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\HMFMPYUN.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesKorean());

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
#ifndef _SPH_DEBUG_DLL_CY
    if (IMGUI_DOCKING)
      io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;           // Enable Docking
#endif
    ImGui_ImplGlfw_InitForOpenGL(p_window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    ImGuiStyle& style = ImGui::GetStyle();
#ifndef _SPH_DEBUG_DLL_CY
    if (IMGUI_DOCKING)
      if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
      {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
      }
#endif
    // Setup Dear ImGui style
    ImGui::StyleColorsLight();
    file_dialog_helper = new ImGuiFileDialog();

    //ImGuiWindowClass& window_class = ImGui::Get
    //window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlagsPrivate_::ImGuiDockNodeFlags_NoTabBar;
    //ImGui::SetNextWindowClass(window_class);

    /*
    ImVec4(0.74f, 1.00f, 0.93f, 1.00f); // Light Mint
    ImVec4(0.60f, 1.00f, 0.85f, 1.00f); // Mint Cream
    ImVec4(0.62f, 0.94f, 0.82f, 1.00f); // Mint Green
    ImVec4(0.00f, 1.00f, 0.60f, 1.00f); // Aquamarine
    ImVec4(0.48f, 0.93f, 0.81f, 1.00f); // Pale Mint
    ImVec4(0.40f, 0.90f, 0.70f, 1.00f); // Medium Mint
    ImVec4(0.68f, 0.93f, 0.81f, 1.00f); // Mint Frost
    ImVec4(0.67f, 0.95f, 0.87f, 1.00f); // Mint Ice
    ImVec4(0.50f, 0.99f, 0.77f, 1.00f); // Mint Mist
    ImVec4(0.39f, 0.85f, 0.72f, 1.00f); // Soft Mint
    ImVec4(0.57f, 0.90f, 0.81f, 1.00f); // Cool Mint
    ImVec4(0.47f, 0.91f, 0.78f, 1.00f); // Pastel Mint
    ImVec4(0.36f, 0.89f, 0.68f, 1.00f); // Mint Tea
    ImVec4(0.30f, 0.85f, 0.73f, 1.00f); // Green Mint
    ImVec4(0.55f, 0.94f, 0.85f, 1.00f); // Icy Mint
    ImVec4(0.70f, 0.93f, 0.88f, 1.00f); // Soft Seafoam
    ImVec4(0.64f, 0.95f, 0.80f, 1.00f); // Misty Mint
    ImVec4(0.58f, 0.92f, 0.82f, 1.00f); // Frosted Mint
    ImVec4(0.42f, 0.92f, 0.78f, 1.00f); // Mint Blue
    ImVec4(0.44f, 0.90f, 0.76f, 1.00f); // Aqua Mint
    ImVec4(0.62f, 1.00f, 0.87f, 1.00f); // Light Seafoam
    ImVec4(0.55f, 1.00f, 0.80f, 1.00f); // Fresh Mint
    ImVec4(0.64f, 0.98f, 0.88f, 1.00f); // Glacial Mint
    ImVec4(0.59f, 0.99f, 0.83f, 1.00f); // Mint Glow
    ImVec4(0.40f, 0.88f, 0.78f, 1.00f); // Deep Mint
    ImVec4(0.38f, 0.80f, 0.67f, 1.00f); // Dark Mint
    ImVec4(0.72f, 0.98f, 0.89f, 1.00f); // Pale Mint Ice
    ImVec4(0.68f, 1.00f, 0.88f, 1.00f); // Mint Breeze
    ImVec4(0.66f, 0.99f, 0.90f, 1.00f); // Mint Powder
    ImVec4(0.50f, 1.00f, 0.85f, 1.00f); // Mint Water

    */
#if 1
    // Setup style
    style.Colors[ImGuiCol_Text]                 = ImVec4(0.f, 0.00f, 0.03f, 1.00f); // Light Mint
    style.Colors[ImGuiCol_WindowBg]                   = ImVec4(0.60f, 1.00f, 0.85f, 1.00f); // Mint Cream
    style.Colors[ImGuiCol_MenuBarBg]                  = ImVec4(0.62f, 0.94f, 0.82f, 1.00f); // Mint Green
    style.Colors[ImGuiCol_Border]             = ImVec4(0.48f, 0.93f, 0.81f, 1.00f); // Pale Mint
    style.Colors[ImGuiCol_BorderShadow]       = ImVec4(0.40f, 0.90f, 0.70f, 1.00f); // Medium Mint
    style.Colors[ImGuiCol_FrameBg]            = ImVec4(0.68f, 0.93f, 0.81f, 1.00f); // Mint Frost
    style.Colors[ImGuiCol_FrameBgHovered]     = ImVec4(0.67f, 0.95f, 0.87f, 1.00f); // Mint Ice
    style.Colors[ImGuiCol_FrameBgActive]  = ImVec4(0.50f, 0.99f, 0.77f, 1.00f); // Mint Mist
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.39f, 0.85f, 0.72f, 1.00f); // Soft Mint
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.57f, 0.90f, 0.81f, 1.00f); // Cool Mint
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.47f, 0.91f, 0.78f, 1.00f); // Pastel Mint
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.89f, 0.68f, 1.00f); // Mint Tea
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.85f, 0.73f, 1.00f); // Green Mint
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.55f, 0.94f, 0.85f, 1.00f); // Icy Mint
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.0f, 0.18f, 1.00f); // Soft Seafoam
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.95f, 0.80f, 1.00f); // Misty Mint
    style.Colors[ImGuiCol_Button] = ImVec4(0.58f, 0.92f, 0.82f, 1.00f); // Frosted Mint
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.42f, 0.92f, 0.78f, 1.00f); // Mint Blue
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.44f, 0.90f, 0.76f, 1.00f); // Aqua Mint
    style.Colors[ImGuiCol_Header] = ImVec4(0.62f, 1.00f, 0.87f, 1.00f); // Light Seafoam
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.55f, 1.00f, 0.80f, 1.00f); // Fresh Mint
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.64f, 0.98f, 0.88f, 1.00f); // Glacial Mint
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.59f, 0.99f, 0.83f, 1.00f); // Mint Glow
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.88f, 0.78f, 1.00f); // Deep Mint
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.38f, 0.80f, 0.67f, 1.00f); // Dark Mint
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.72f, 0.98f, 0.89f, 1.00f); // Pale Mint Ice
#endif
#if 0
    // Setup style
    style.Colors[ImGuiCol_Text] = ImVec4(0.31f, 0.25f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.74f, 0.74f, 0.94f, 1.00f);
    //style.Colors[ImGuiCol_ChildWindowBg]      = ImVec4(0.68f, 0.68f, 0.68f, 0.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.62f, 0.70f, 0.72f, 0.56f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.33f, 0.14f, 0.47f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.97f, 0.31f, 0.13f, 0.81f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.42f, 0.75f, 1.00f, 0.53f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.65f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.40f, 0.62f, 0.80f, 0.15f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.39f, 0.64f, 0.80f, 0.30f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.67f, 0.80f, 0.59f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.48f, 0.53f, 0.67f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.48f, 0.47f, 0.47f, 0.71f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.31f, 0.47f, 0.99f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(1.00f, 0.79f, 0.18f, 0.78f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.42f, 0.82f, 1.00f, 0.81f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.72f, 1.00f, 1.00f, 0.86f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.65f, 0.78f, 0.84f, 0.80f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.75f, 0.88f, 0.94f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.55f, 0.68f, 0.74f, 0.80f);//ImVec4(0.46f, 0.84f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.60f, 0.60f, 0.80f, 0.30f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.99f, 0.54f, 0.43f);
#endif
    //style.Colors[ImGuiCol_ComboBg]            = ImVec4(0.89f, 0.98f, 1.00f, 0.99f);
    //style.Colors[ImGuiCol_CloseButton] = ImVec4(0.41f, 0.75f, 0.98f, 0.50f);
    //style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(1.00f, 0.47f, 0.41f, 0.60f);
    //style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(1.00f, 0.16f, 0.00f, 1.00f);
    //style.Colors[ImGuiCol_TooltipBg] = ImVec4(0.82f, 0.92f, 1.00f, 0.90f);
    style.Alpha = 1.0f;
    //style.WindowFillAlphaDefault = 1.0f;
    style.FrameRounding = 4;
    style.IndentSpacing = 12.0f;
    style.WindowMenuButtonPosition = ImGuiDir_None;


    // GUI Data initialization
    m_imgui_layers.reserve(3);
    m_imgui_layers.emplace_back(RSRenderLayer::R_SURFACE_OPAQUE, "Surface");
    m_imgui_layers.emplace_back(RSRenderLayer::R_LINE_OPAQUE, "Edge");
    m_imgui_layers.emplace_back(RSRenderLayer::R_POINT, "Point");

    MappingKeyMouseBinding();
  }

  void RSImguiHandler::UpdatePre(float dt)
  {
    if (mRSEVENTHANDLER->GetIsImguiShow())
    {
      const RSResourceManager* resource_manager = RSResourceManager::GetInstance();
      RSGraphicsData* graphics_data = resource_manager->GetGraphicsData();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      ImGui::Begin("WhatSseob World");

      ShowOpenFileDialog();

      ImGui::ColorEdit3("clear color", graphics_data->clear_color);

      if (ImGui::SliderFloat3("light position", glm::value_ptr(graphics_data->light_pos), -100.0f, 100.0f))
      {
        graphics_data->SetLightUpdated(true);
      }

      ImGui::Text("GUI average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::Text("RSEngine average %.3f ms/frame (%.1f FPS)", RS_Engine::mRSENGINE->GetDeltaTime(),
                  RS_Engine::mRSENGINE->GetFps());

      bool fixed_fps_checkbox = RS_Engine::mRSENGINE->b_fixed_fps;
      if(ImGui::Checkbox("Fixed FPS", &(fixed_fps_checkbox)))
      {
        RS_Engine::mRSENGINE->SetFixedFps(fixed_fps_checkbox);
      }

      ImGui::Checkbox("show debug window", &m_show_debug_buffer);
      ImGui::Checkbox("show final viewport", &m_show_final_viewport);

      float fps = RS_Engine::mRSENGINE->GetFps();
      if (ImGui::InputFloat("FPS", &(fps)))
      {
        fps = glm::max(fps, 15.0f);
        RS_Engine::mRSENGINE->SetFps(fps);
      }

      bool any_property_changed = false;
      // Checkbox for RenderingFlag (Shadow, Deferred, SSAO)
      bool b_shadow = RS_Graphics::mRSGRAPHICS->GetRenderingFlagBool(RenderingFlag::SHADOW);
      if (ImGui::Checkbox("Shadow", &b_shadow))
      {
        RS_Graphics::mRSGRAPHICS->SetRenderingFlag(RenderingFlag::SHADOW, b_shadow);
        any_property_changed = true;
      }

      bool b_opaque_deferred = RS_Graphics::mRSGRAPHICS->GetRenderingFlagBool(RenderingFlag::OPAQUE_DEFERRED);
      if (ImGui::Checkbox("Deferred", &b_opaque_deferred))
      {
        RS_Graphics::mRSGRAPHICS->SetRenderingFlag(RenderingFlag::OPAQUE_DEFERRED, b_opaque_deferred);
        any_property_changed = true;
      }

      bool b_fluid_deferred = RS_Graphics::mRSGRAPHICS->GetRenderingFlagBool(RenderingFlag::FLUID_DEFERRED);
      if (ImGui::Checkbox("Fluid Deferred", &b_fluid_deferred))
      {
        RS_Graphics::mRSGRAPHICS->SetRenderingFlag(RenderingFlag::FLUID_DEFERRED, b_fluid_deferred);
        any_property_changed = true;
      }

      bool b_cloth_defered = RS_Graphics::mRSGRAPHICS->GetRenderingFlagBool(RenderingFlag::CLOTH_DEFERRED);
      if (ImGui::Checkbox("Cloth Deferred", &b_cloth_defered))
      {
        RS_Graphics::mRSGRAPHICS->SetRenderingFlag(RenderingFlag::CLOTH_DEFERRED, b_cloth_defered);
        any_property_changed = true;
      }

      bool b_ssao = RS_Graphics::mRSGRAPHICS->GetRenderingFlagBool(RenderingFlag::SSAO);
      if (ImGui::Checkbox("SSAO", &b_ssao))
      {
        RS_Graphics::mRSGRAPHICS->SetRenderingFlag(RenderingFlag::SSAO, b_ssao);
        any_property_changed = true;
      }

      bool b_skybox = RS_Graphics::mRSGRAPHICS->GetRenderingFlagBool(RenderingFlag::SKYBOX);
      if (ImGui::Checkbox("Skybox", &b_skybox))
      {
        any_property_changed = true;
        RS_Graphics::mRSGRAPHICS->SetRenderingFlag(RenderingFlag::SKYBOX, b_skybox);
      }

      bool b_ssr = RS_Graphics::mRSGRAPHICS->GetRenderingFlagBool(RenderingFlag::SSR);
      if (ImGui::Checkbox("SSR", &b_ssr))
      {
        RS_Graphics::mRSGRAPHICS->SetRenderingFlag(RenderingFlag::SSR, b_ssr);
        any_property_changed = true;
      }

      // Color Filter Post-Processing checkbox
      bool b_color_filter = RS_Graphics::mRSGRAPHICS->GetRenderingFlagBool(RenderingFlag::COLOR_FILTER);
      if (ImGui::Checkbox("Color Filter", &b_color_filter))
      {
        RS_Graphics::mRSGRAPHICS->SetRenderingFlag(RenderingFlag::COLOR_FILTER, b_color_filter);
        any_property_changed = true;
      }

      // Color Filter controls (only show when enabled)
      if (b_color_filter)
      {
        RSColorFilterData& color_data = RS_Graphics::mRSGRAPHICS->GetColorFilterData();

        ImGui::Separator();
        ImGui::Text("Color Filter");

        // Mode combo box
        const char* mode_names[] = { "None", "Grayscale", "Red", "Green", "Blue", "Sepia", "Invert", "Custom" };
        int current_mode = static_cast<int>(color_data.mode);
        if (ImGui::Combo("Filter Mode", &current_mode, mode_names, IM_ARRAYSIZE(mode_names)))
        {
          color_data.SetMode(static_cast<ColorFilterMode>(current_mode));
        }

        // Intensity slider
        if (ImGui::SliderFloat("Intensity", &color_data.intensity, 0.0f, 1.0f, "%.1f"))
        {
          color_data.is_dirty = true;
        }

        // Custom weights (only show for Custom mode or in collapsing header)
        if (color_data.mode == ColorFilterMode::CUSTOM || ImGui::CollapsingHeader("Custom Weights"))
        {
          ImGui::Text("RGB Weights:");
          bool weights_changed = false;

          if (ImGui::SliderFloat("R Weight", &color_data.weights[0], 0.0f, 2.0f, "%.2f")) weights_changed = true;
          if (ImGui::SliderFloat("G Weight", &color_data.weights[1], 0.0f, 2.0f, "%.2f")) weights_changed = true;
          if (ImGui::SliderFloat("B Weight", &color_data.weights[2], 0.0f, 2.0f, "%.2f")) weights_changed = true;

          if (weights_changed)
          {
            color_data.mode = ColorFilterMode::CUSTOM;
            color_data.is_dirty = true;
          }

          if (ImGui::Button("Reset Filter"))
          {
            color_data.Reset();
          }
        }
        ImGui::Separator();
      }

      // Image Kernel Post-Processing checkbox
      bool b_image_kernel = RS_Graphics::mRSGRAPHICS->GetRenderingFlagBool(RenderingFlag::IMAGE_KERNEL);
      if (ImGui::Checkbox("Image Kernel", &b_image_kernel))
      {
        RS_Graphics::mRSGRAPHICS->SetRenderingFlag(RenderingFlag::IMAGE_KERNEL, b_image_kernel);
        any_property_changed = true;
      }

      // Image Kernel controls (only show when enabled)
      if (b_image_kernel)
      {
        RSKernelPostProcessData& kernel_data = RS_Graphics::mRSGRAPHICS->GetKernelData();

        ImGui::Separator();
        ImGui::Text("Kernel Post-Processing");

        // Preset combo box
        const char* preset_names[] = { "None", "Edge Detect", "Sharpen", "Box Blur", "Gaussian Blur", "Emboss", "Custom" };
        int current_preset = static_cast<int>(kernel_data.preset);
        if (ImGui::Combo("Preset", &current_preset, preset_names, IM_ARRAYSIZE(preset_names)))
        {
          kernel_data.SetPreset(static_cast<KernelPreset>(current_preset));
        }

        // Pass count slider (1-4)
        if (ImGui::SliderInt("Pass Count", &kernel_data.pass_count, 1, 4))
        {
          kernel_data.is_dirty = true;
        }

        // Divisor and offset sliders
        if (ImGui::SliderFloat("Divisor", &kernel_data.divisor, 0.1f, 20.0f, "%.1f"))
        {
          kernel_data.is_dirty = true;
        }
        if (ImGui::SliderFloat("Offset", &kernel_data.offset, -1.0f, 1.0f, "%.1f"))
        {
          kernel_data.is_dirty = true;
        }

        // 3x3 Kernel matrix editor (only for Custom preset)
        if (kernel_data.preset == KernelPreset::CUSTOM || ImGui::CollapsingHeader("Kernel Matrix"))
        {
          ImGui::Text("3x3 Kernel Matrix:");
          bool kernel_changed = false;

          ImGui::PushItemWidth(60);
          // Row 0
          if (ImGui::InputFloat("##k0", &kernel_data.kernel[0], 0, 0, "%.2f")) kernel_changed = true;
          ImGui::SameLine();
          if (ImGui::InputFloat("##k1", &kernel_data.kernel[1], 0, 0, "%.2f")) kernel_changed = true;
          ImGui::SameLine();
          if (ImGui::InputFloat("##k2", &kernel_data.kernel[2], 0, 0, "%.2f")) kernel_changed = true;

          // Row 1
          if (ImGui::InputFloat("##k3", &kernel_data.kernel[3], 0, 0, "%.2f")) kernel_changed = true;
          ImGui::SameLine();
          if (ImGui::InputFloat("##k4", &kernel_data.kernel[4], 0, 0, "%.2f")) kernel_changed = true;
          ImGui::SameLine();
          if (ImGui::InputFloat("##k5", &kernel_data.kernel[5], 0, 0, "%.2f")) kernel_changed = true;

          // Row 2
          if (ImGui::InputFloat("##k6", &kernel_data.kernel[6], 0, 0, "%.2f")) kernel_changed = true;
          ImGui::SameLine();
          if (ImGui::InputFloat("##k7", &kernel_data.kernel[7], 0, 0, "%.2f")) kernel_changed = true;
          ImGui::SameLine();
          if (ImGui::InputFloat("##k8", &kernel_data.kernel[8], 0, 0, "%.2f")) kernel_changed = true;
          ImGui::PopItemWidth();

          if (kernel_changed)
          {
            kernel_data.preset = KernelPreset::CUSTOM;
            kernel_data.is_dirty = true;
          }

          // Reset button
          if (ImGui::Button("Reset Kernel"))
          {
            kernel_data.Reset();
          }
        }
        ImGui::Separator();
      }



      bool b_radio_changed = false;
      // Select RSDataRenderType of graphics data with radio button
      int render_num = static_cast<int>(graphics_data->data_render_type);
      ImGui::Text("Data(Fluid) Render Type");
      if (ImGui::RadioButton("forward",       &render_num, 1))
        b_radio_changed = true;
      if (ImGui::RadioButton("deferred" ,     &render_num, 2))
        b_radio_changed = true;
      if(ImGui::RadioButton("stream line",   &render_num, 3))
        b_radio_changed = true;
      if(ImGui::RadioButton("path line" ,    &render_num, 4))
        b_radio_changed = true;
      if(ImGui::RadioButton("vector field",  &render_num, 5))
        b_radio_changed = true;
      if(ImGui::RadioButton("contour" ,      &render_num, 6))
        b_radio_changed = true;
      if(ImGui::RadioButton("isosurface" ,   &render_num, 7))
        b_radio_changed = true;

      if (b_radio_changed)
      {
        graphics_data->data_render_type = static_cast<RSDataRenderType>(render_num);
        any_property_changed = true;
      }

      if (any_property_changed)
      {
        if (RS_VTK::RSVTKViewer* p_vtk = RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer(); p_vtk != nullptr)
          p_vtk->SetClippingChanged(true);
        if (RSParticleSimulator* p_ps = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator(); p_ps != nullptr)
          p_ps->UpdateRenderProperty();
      }




      // controll ssr's g_max_thickness and g_depthbias between -1.0f and 1.0f
      if (b_ssr)
      {
        ImGui::SliderFloat("SSR Max Thickness", &graphics_data->g_max_thickness, -30.0f, 30.0f);
        ImGui::SliderFloat("SSR Depth Bias", &graphics_data->g_depthbias, -20.0f, 20.0f);
        ImGui::SliderFloat("SSR Blur Coefficient", &graphics_data->g_blur_coefficient, 0.0f, 10.0f);
      }


      ImGui::End();
      ShowManagerItems();
      ShowCameraItems();
      ShowPlayItems();
      ShowClothSimulatorItems();
      ShowPointClipperItems();
      ShowVtkControllerItems();
      ShowViewport();
    }
  }

  void RSImguiHandler::UpdatePost(float dt)
  {
    if (mRSEVENTHANDLER->GetIsImguiShow())
    {
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      const ImGuiIO& io = ImGui::GetIO(); (void)io;
      // Update and Render additional Platform Windows
#ifndef _SPH_DEBUG_DLL_CY
      if (IMGUI_DOCKING)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
          GLFWwindow* backup_current_context = glfwGetCurrentContext();
          ImGui::UpdatePlatformWindows();
          ImGui::RenderPlatformWindowsDefault();
          glfwMakeContextCurrent(backup_current_context);
        }
#endif
    }
  }

  void RSImguiHandler::Shutdown() const
  {
    delete file_dialog_helper;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void RSImguiHandler::ShowOpenFileDialog() const
  {
    if (file_dialog_helper == nullptr)
      return;
    // open Dialog Simple
    if (ImGui::Button("Open File Dialog")) {
      IGFD::FileDialogConfig config; config.path = ".";
      file_dialog_helper->OpenDialog("ChooseFileDlgKey", "Choose File", ".stl,.obj,.png,.jpg", config);
    }

    // display
    if (file_dialog_helper->Display("ChooseFileDlgKey")) {
      if (file_dialog_helper->IsOk()) { // action if OK
        const std::string file_path_name = file_dialog_helper->GetFilePathName();
        const std::string file_name = file_dialog_helper->GetCurrentFileName();
        const std::string file_filter = file_dialog_helper->GetCurrentFilter();
        // Open the file
        if (file_filter == ".obj")
          RSResourceManager::GetInstance()->GetMeshManager()->LoadMesh(file_name, file_path_name);
        if (file_filter == ".stl")
          RSResourceManager::GetInstance()->GetMeshManager()->LoadMeshStl2(file_name, file_path_name);
        if (file_filter == ".png" || file_filter == ".jpg")
          RSResourceManager::GetInstance()->GetTextureManager()->LoadTextures(file_name, file_path_name);
      }

      // close
      file_dialog_helper->Close();
    }
  }

  void RSImguiHandler::ShowManagerItems()
  {
    const RSResourceManager* resource_manager = RSResourceManager::GetInstance();

    ShowObjectList(resource_manager);

    ShowMeshList(resource_manager);

    ShowTextureList(resource_manager);

    ShowMaterialList(resource_manager);

    ShowObjectControl(resource_manager);

    ShowModelControl(resource_manager);

    ShowMaterialControl(resource_manager);

  }

  void RSImguiHandler::ShowObjectList(const RSResourceManager* rm)
  {

    ImGui::Begin("RSObject List");
    // Check if the map is not empty
    if (rm->GetObjectManager()->GetObjects()->empty())
    {
      ImGui::Text("No objects.");
      ImGui::End();
      return;
    }

    for (const auto& pair : *rm->GetObjectManager()->GetObjects()) 
    {
      if (ImGui::Selectable(pair.first.c_str())) 
      {
        selected_object = pair.second;
        rm->GetObjectManager()->ClearPickingObjectList();
        rm->GetObjectManager()->SetPickingObject(selected_object);

        RS_INFO("Selected object: %s", pair.first.c_str());
      }
    }
    ImGui::End();
  }

  void RSImguiHandler::ShowMeshList(const RSResourceManager* rm)
  {
    ImGui::Begin("Mesh List");
    if (rm->GetMeshManager()->GetMeshes()->empty())
    {
      ImGui::Text("No meshes.");
      ImGui::End();
      return;
    }

    for (const auto& pair : *rm->GetMeshManager()->GetMeshes()) {
      if (ImGui::Selectable(pair.first.c_str())) 
      {
        if (selected_object == nullptr) continue;
        const auto model = dynamic_cast<RSModel*>(selected_object->GetComponent(RSComponentType::CT_MODEL));
        if (model == nullptr) continue;

        if (model->GetMesh() == nullptr)
        {
          const auto mesh = new RSMesh();
          mesh->SetMeshData(pair.second);
          model->SetMesh(mesh);
        }
        else {
          model->GetMesh()->SetMeshData(pair.second);
        }
        RS_INFO("Selected mesh: %s", pair.first.c_str());
      }
    }
    ImGui::End();
  }

  void RSImguiHandler::ShowMaterialList(const RSResourceManager* rm)
  {
    ImGui::Begin("Material List");
    if (rm->GetMaterialManager()->GetMaterials()->empty())
    {
      ImGui::Text("No materials.");
      ImGui::End();
      return;
    }

    for (const auto& pair : *rm->GetMaterialManager()->GetMaterials())
    {
      if (const char* name = pair.first.data(); ImGui::Selectable(name))
      {
        if (selected_object == nullptr) continue;
        const auto model = dynamic_cast<RSModel*>(selected_object->GetComponent(RSComponentType::CT_MODEL));
        if (model == nullptr) continue;
        RSMesh* mesh = model->GetMesh();
        if (mesh == nullptr) continue;

        mesh->SetMaterial(pair.second);

        RS_INFO("Selected material: %s", pair.first.c_str());
      }
    }
    ImGui::End();
  }

  void RSImguiHandler::ShowTextureList(const RSResourceManager* rm)
  {

    ImGui::Begin("RSTexture List");
    if (rm->GetTextureManager()->GetTextures()->empty())
    {
      ImGui::Text("No textures.");
      ImGui::End();
      return;
    }

    for (const auto& pair : *rm->GetTextureManager()->GetTextures()) 
    {
      if (const char* name = pair.first.data(); ImGui::Selectable(name)) 
      {
        if (selected_object == nullptr) continue;

        const auto model = dynamic_cast<RSModel*>(selected_object->GetComponent(RSComponentType::CT_MODEL));
        if (model == nullptr) continue;

        RSMesh* mesh = model->GetMesh();
        if (mesh == nullptr) continue;

        if (mesh->GetMeshData() != nullptr)
        {
          if (mesh->GetMeshData()->has_texcoord == false)
          {
            RS_INFO("This mesh does not have texture coordinates.");
            mesh->SetTextureID(rm->GetTextureManager()->GetTexture("default")->GetTexture());
            continue;
          }
          mesh->SetTextureID(pair.second->GetTexture());
        }

        RS_INFO("Selected texture: %s", pair.first.c_str());
      }
    }
    ImGui::End();
  }

  void RSImguiHandler::ShowObjectControl(const RSResourceManager* rm)
  {
    ImGui::Begin("RSObject Control");

    selected_obj_name = selected_object != nullptr ? selected_object->GetName().c_str() : "nullptr";
    ImGui::Text("Current RSObject : %s", selected_obj_name.c_str());

    static char str0[128] = "name";
    ImGui::InputText("Target Name", str0, IM_ARRAYSIZE(str0));
    const std::string name_(str0);
    if (ImGui::Button("Add RSObject"))
      mRSEVENTHANDLER->AddEvent(EventType::CREATE_OBJECT, name_);

    if (selected_object == nullptr)
    {
      ImGui::End();
      return;
    }
    
    ImGui::Text("RSObject Components");
    for (const auto& component : *selected_object->GetComponents())
      ImGui::Text(component.second->GetName().c_str());

    if (ImGui::Button("Add RSModel RSComponent"))
      mRSEVENTHANDLER->AddEvent(EventType::OBJECT_ADD_MODEL_COMPONENT, selected_object->GetName());

    if (ImGui::Button("Add RSRigidbody RSComponent"))
      mRSEVENTHANDLER->AddEvent(EventType::OBJECT_ADD_RIGIDBODY_COMPONENT, selected_object->GetName());

    if (ImGui::Button("Delete RSObject"))
    {
      mRSEVENTHANDLER->AddEvent(EventType::DELETE_OBJECT, selected_object->GetName());
      selected_object = nullptr;
    }

    ImGui::End();
  }

  void RSImguiHandler::ShowModelControl(const RSResourceManager* rm)
  {

    ImGui::Begin("RSModel Control");
    if (selected_object == nullptr)
    {
      ImGui::End();
      return;
    }

    const auto model = dynamic_cast<RSModel*>(selected_object->GetComponent(RSComponentType::CT_MODEL));
    if (model == nullptr)
    {
      ImGui::End();
      return;
    }

    ImGui::Checkbox("Visible", &model->m_is_visible);
    ImGui::ColorEdit3("Color", glm::value_ptr(model->m_color));

    if (ImGui::InputFloat3("Position", glm::value_ptr(selected_object->m_transform.m_position)))
      selected_object->m_transform.UpdatedTransformFromGui();
    if (ImGui::InputFloat3("Scale", glm::value_ptr(selected_object->m_transform.m_scale)))
      selected_object->m_transform.UpdatedTransformFromGui();
    if (ImGui::InputFloat3("Rotation", glm::value_ptr(selected_object->m_transform.m_rotation)))
      selected_object->m_transform.UpdatedTransformFromGui();

    // Set Transparency (float) by slide
    if (ImGui::SliderFloat("Transparency", &model->m_transparency, 0.0f, 1.0f))
      model->SetTransparency(model->m_transparency); // Is it right the order?? Should be changed on GUI

    if (glm::vec2 model_tiling = model->m_tiling; ImGui::SliderFloat2("Tiling", glm::value_ptr(model_tiling), 0.1f, 10.0f))
      model->SetTiling(model_tiling);

    if (glm::vec2 model_offset = model->m_offset; ImGui::SliderFloat2("Offset", glm::value_ptr(model_offset), -1.0f, 1.0f))
      model->SetOffset(model_offset);

    // Layer Combo box enum class
    int layer = GetLayerIndex(selected_object->GetRenderLayer());
    if (ImGui::Combo("Layer", &layer, "Surface\0Edge\0Point\0SurfaceEdge\0"))
      SetLayerIndex(layer);
    
    
    ImGui::End();

  }

  void RSImguiHandler::ShowMaterialControl(const RSResourceManager* rm)
  {
    ImGui::Begin("RSMaterial Control");
    if (selected_object == nullptr)
    {
      ImGui::End();
      return;
    }
    const auto model = dynamic_cast<RSModel*>(selected_object->GetComponent(RSComponentType::CT_MODEL));
    if (model == nullptr)
    {
      ImGui::End();
      return;
    }
    RSMesh* mesh = model->GetMesh();
    if (mesh == nullptr)
    {
      ImGui::End();
      return;
    }
    RS_Material::RSMaterial* material = mesh->GetMaterial();
    if (material == nullptr)
    {
      ImGui::End();
      return;
    }

    // Show material name
    ImGui::Text("Material Name : %s", material->GetName().c_str());

    // select diffuse texture from texture list
    if (ImGui::BeginCombo("Diffuse Texture", rm->GetTextureManager()->GetTextureName(material->GetDiffuseMap()).c_str()))
    {
      for (const auto& pair : *rm->GetTextureManager()->GetTextures())
      {
        const bool is_selected = (material->GetDiffuseMap() == pair.second->GetTexture());
        if (ImGui::Selectable(pair.first.c_str(), is_selected))
        {
          material->SetDiffuseMap(pair.second->GetTexture());
        }
      }
      ImGui::EndCombo();
    }

    // Show diffuse texture image
    if (material->GetDiffuseMap() != 0)
    {
      ImGui::Image((void*)(intptr_t)material->GetDiffuseMap(), ImVec2(50, 50));
    }

    // select normal texture from texture list
    if (ImGui::BeginCombo("Normal Texture", rm->GetTextureManager()->GetTextureName(material->GetNormalMap()).c_str()))
    {
      for (const auto& pair : *rm->GetTextureManager()->GetTextures())
      {
        const bool is_selected = (material->GetNormalMap() == pair.second->GetTexture());
        if (ImGui::Selectable(pair.first.c_str(), is_selected))
        {
          material->SetNormalMap(pair.second->GetTexture());
        }
      }
      ImGui::EndCombo();
    }

    // Show normal texture image
    if (material->GetNormalMap() != 0)
    {
      ImGui::Image((void*)(intptr_t)material->GetNormalMap(), ImVec2(50, 50));
    }

    // Set metallic (float) by slide 0 ~ 1
    float metallic = material->GetMetallic();
    if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f))
    {
      material->SetMetallic(metallic);
    }

    // Set roughness (float) by slide 0 ~ 1
    float roughness = material->GetRoughness();
    if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f))
    {
      material->SetRoughness(roughness);
    }

    float ambient = material->GetAmbient();
    if (ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f))
    {
      material->SetAmbient(ambient);
    }


    ImGui::End();
  }

  void RSImguiHandler::ShowCameraItems()
  {
    ImGui::Begin("Camera Control");
    RS_Camera::RSCamera* camera = RSResourceManager::GetInstance()->GetCamera();

    if (ImGui::Button("Reset"))
      camera->Reset();

    // Get Camera position (glm::vec3) by text
    ImGui::InputFloat3("Camera Position", glm::value_ptr(camera->position_));
    // Get Camera focal point (glm::vec3) by text
    ImGui::InputFloat3("Camera Focal Point", glm::value_ptr(camera->focal_point_));
    // Get Camera view up (glm::vec3) by text
    ImGui::InputFloat3("Camera View Up", glm::value_ptr(camera->view_up_));
    // Get Camera rotate axis (glm::vec3) by text
    ImGui::InputFloat3("Camera Rotate Axis", glm::value_ptr(camera->rotate_axis_));

    // Set Camera mouse_rotation_sensitivity_ (float) by text
    ImGui::InputFloat("Mouse Rotation Sensitivity", &camera->mouse_rotation_sensitivity_);
    // Set camera mouse move sensitivity_ (float) by text
    ImGui::InputFloat("Mouse Move Sensitivity", &camera->mouse_move_sensitivity_);

    ImGui::End();
  }

  void RSImguiHandler::ShowPlayItems()
  {
    if (!RSResourceManager::GetInstance()->GetParticleManager()->IsUseSph())
      return;
    RSParticleSimulator* p_ps = RSResourceManager::GetInstance()->GetParticleManager()->GetParticleSimulator();
    ImGui::Begin("Play Control");
    //ImGui::InputFloat("Player speed", &mLOGIC->GetPLRolling()->m_player_speed); // Play logic be moved to game logic

    int solver_type = static_cast<int>(p_ps->GetSolverType());
    if (ImGui::Combo("SolverType", &solver_type, "None\0BruteForce\0Grid\0DynamicBruteForce\0"))
      p_ps->SetSolverType(static_cast<RSSolverType>(solver_type));


    // Set Clipping Type
    int clipping_type = p_ps->GetClippingTypeI();
    if (ImGui::Combo("ClippingType", &clipping_type, "None\0Plane\0Cube\0Sphere\0"))
      p_ps->SetClippingType(clipping_type);

    // Set Clipping Area
    int clipping_area = p_ps->GetClippingAreaI();
    if (ImGui::Combo("ClippingArea", &clipping_area, "Inside\0Outside\0"))
      p_ps->SetClippingArea(clipping_area);

    ImGui::InputFloat3("ClippingPMP", glm::value_ptr(p_ps->m_clipping_PMP));
    ImGui::InputFloat3("ClippingVMR", glm::value_ptr(p_ps->m_clipping_VMR));

    if (ImGui::SliderFloat("Viscosity", &(p_ps->m_particle_property.viscosity), 10.0f, 2000.0f))
    {
      p_ps->SetAnyCalculatePropertyChanged();
      //std::cout << "viscosity was changed" << std::endl;
    }
    if (ImGui::SliderFloat("Gas Constant", &(p_ps->m_particle_property.gas_constant), 10.0f, 3000.0f))
    {
      p_ps->SetAnyCalculatePropertyChanged();
      //std::cout << "gas constant was changed" << std::endl;
    }
    if (ImGui::SliderFloat("Density", &(p_ps->m_particle_property.density), 10.0f, 2000.0f))
    {
      p_ps->SetAnyCalculatePropertyChanged();
      //std::cout << "density was changed" << std::endl;
    }

    if (ImGui::SliderFloat("space", &(p_ps->m_init_setting.init_particle_setting_space), 0.01f, 2.0f))
    {
      //p_ps->SetAnyRenderPropertyChanged();
    }

    if (ImGui::SliderFloat3("inflow_direction", glm::value_ptr(p_ps->m_init_setting.m_inflow_direction), -1, 1))
    {
      p_ps->SetInflowDirectionUpdated();
    }

    if (ImGui::SliderFloat("speed", &(p_ps->m_init_setting.m_inflow_speed), 0.01f, 100.0f))
    {

    }

    if (ImGui::InputFloat("min_legend", &(p_ps->value_min_legend)))
    {
      p_ps->SetAnyRenderPropertyChanged();
    }
    if (ImGui::InputFloat("max_legend", &(p_ps->value_max_legend)))
    {
      p_ps->SetAnyRenderPropertyChanged();
    }

    // Legend Texture Changer
    if (ImGui::BeginCombo("Legend texture",
                          RSResourceManager::GetInstance()->GetTextureManager()->
                                                            GetTextureName(p_ps->GetLegendTextureId()).c_str()))
    {
      for (const auto& pair : *RSResourceManager::GetInstance()->GetTextureManager()->GetTextures())
      {
        // the 'legend_divided' name texture should be pass.
        if (pair.first == "legend_divided")
          continue;

        const bool is_selected = (p_ps->GetLegendTextureId() == pair.second->GetTexture());
        if (ImGui::Selectable(pair.first.c_str(), is_selected))
        {
          p_ps->SetLegendTextureId(pair.second->GetTexture());
          // Get legend widget from widget manager.
          if (const auto widget = RSResourceManager::GetInstance()->GetWidgetManager()->GetLegendWidget();
            widget != nullptr)
            widget->SetTexture(pair.second->GetTexture());
        }
        if (is_selected)
          ImGui::SetItemDefaultFocus();
      }

      ImGui::EndCombo();
    }

    // Register button to add mesh data to collision
    if (ImGui::Button("Collision"))
    {
      if (selected_object != nullptr)
      {
        p_ps->AddMeshDataToCollision(selected_object);
      }
    }

    ImGui::End();
  }

  void RSImguiHandler::ShowPointClipperItems()
  {
    if (!RSResourceManager::GetInstance()->GetParticleManager()->IsExistPointClipper())
      return;

    RS_PointClipper::RSPointClipper* p_pc = RSResourceManager::GetInstance()->GetParticleManager()->GetPointClipper();
    ImGui::Begin("PointClipper Control");

    // clipping id 0 : none, 1 : do
    if (ImGui::Checkbox("Clipping", &p_pc->b_is_clipping))
    {
      p_pc->SetIsClipping(p_pc->b_is_clipping);
    }

    // clipping color(vec3) 0 ~ 1 by slide
    if (ImGui::SliderFloat3("Clipping Color", glm::value_ptr(p_pc->m_clip_color), 0, 1))
    {
      //p_pc->SetClippingColor(p_pc->m_clipping_color);
    }


    ImGui::End();



  }

  void RSImguiHandler::ShowVtkControllerItems()
  {
    if (!RSResourceManager::GetInstance()->GetParticleManager()->IsExistVtkViewer())
      return;

    RS_VTK::RSVTKViewer* p_vtk = RSResourceManager::GetInstance()->GetParticleManager()->GetVtkViewer();
    ImGui::Begin("VTK Controller");

     if (p_vtk->IsLoaded())
     {
      ImGui::Text("Data min value : %.3f ", p_vtk->GetMin());
      ImGui::Text("Data max value : %.3f ", p_vtk->GetMax());

      ImGui::Text("Vec3 Data min value : %.3f, %.3f, %.3f ", 
                    p_vtk->GetMinVec3().x, p_vtk->GetMinVec3().y, p_vtk->GetMinVec3().z);
      ImGui::Text("Vec3 Data max value : %.3f, %.3f, %.3f ", 
                    p_vtk->GetMaxVec3().x, p_vtk->GetMaxVec3().y, p_vtk->GetMaxVec3().z);

       // Input Value float for legend min and max
      float legend_min = p_vtk->GetMinValueLegend();
      float legend_max = p_vtk->GetMaxValueLegend();
      if (ImGui::InputFloat("Legend Min", &legend_min))
        p_vtk->SetMinValueLegend(legend_min);
      if (ImGui::InputFloat("Legend Max", &legend_max))
        p_vtk->SetMaxValueLegend(legend_max);

      glm::vec3 legend_min_vec3 = p_vtk->GetMinValueLegendVec3();
      glm::vec3 legend_max_vec3 = p_vtk->GetMaxValueLegendVec3();
      if (ImGui::InputFloat3("Legend Min Vec3", glm::value_ptr(legend_min_vec3)))
        p_vtk->SetMinValueLegendVec3(legend_min_vec3);
      if (ImGui::InputFloat3("Legend Max Vec3", glm::value_ptr(legend_max_vec3)))
        p_vtk->SetMaxValueLegendVec3(legend_max_vec3);

      if (ImGui::Button("Async data range to legend"))
        p_vtk->AsyncMinMaxValueLegend();

      ImGui::Text("Attribute name : %s", p_vtk->GetAttributeData().name.c_str());
      ImGui::Text("Attribute type : %s", p_vtk->GetAttributeData().type.c_str());
      ImGui::Text("Particle count %d", p_vtk->GetParticleCount());
      ImGui::Text("Max File Id %d", p_vtk->GetMaxFileId());
      ImGui::Text("Cur File Id %d", p_vtk->GetVtkFileId());

       // Slider Point Size 1 ~ 100f.
      float point_size = p_vtk->GetPointSize();
      if (ImGui::SliderFloat("Point Size", &point_size, 1.0f, 300.0f))
      {
        p_vtk->SetPointSize(point_size);
      }

      if (ImGui::BeginCombo("Attributes", p_vtk->GetAttributeData().name.c_str()))
      {
        for (int i = 0; i < p_vtk->GetAttributeNames()->size(); i++)
        {
          const bool is_selected = (p_vtk->GetAttributeData().name == p_vtk->GetAttributeNames()->at(i));
          if (ImGui::Selectable(p_vtk->GetAttributeNames()->at(i).c_str(), is_selected))
          {
            p_vtk->ChangeAttributeIndex(i);
          }
          if (is_selected)
            ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
      }

       // Legend Texture Changer
       if (ImGui::BeginCombo("Legend texture",
                             RSResourceManager::GetInstance()->GetTextureManager()->
                                                               GetTextureName(p_vtk->GetLegendTextureId()).c_str()))
       {
         for (const auto& pair : *RSResourceManager::GetInstance()->GetTextureManager()->GetTextures())
         {
           // the 'legend_divided' name texture should be pass.
           if (pair.first == "legend_divided")
             continue;
           
           const bool is_selected = (p_vtk->GetLegendTextureId() == pair.second->GetTexture());
           if (ImGui::Selectable(pair.first.c_str(), is_selected))
           {
             p_vtk->SetLegendTextureId(pair.second->GetTexture());
             // Get legend widget from widget manager.
             if (const auto widget = RSResourceManager::GetInstance()->GetWidgetManager()->GetLegendWidget(); widget != nullptr)
               widget->SetTexture(pair.second->GetTexture());
           }
           if (is_selected)
             ImGui::SetItemDefaultFocus();
         }

         ImGui::EndCombo();
       }

      // Set Clipping Type
      int clipping_type = static_cast<int>(p_vtk->GetClippingType());
      if (ImGui::Combo("ClippingType", &clipping_type, "None\0Cube\0Sphere\0Plane\0"))
      {
        p_vtk->SetClippingType(clipping_type);
        p_vtk->SetClippingChanged(true);
      }

      //// Set Clipping Area
      //int clipping_area = p_vtk->GetClippingAreaI();
      //if (ImGui::Combo("ClippingArea", &clipping_area, "Inside\0Outside\0"))
      //{
      //  p_vtk->SetClippingArea(clipping_area);
      //  p_vtk->SetClippingChanged(true);
      //}

      if (ImGui::Checkbox("InvertClipping", &p_vtk->b_invert_clipping))
      {
        p_vtk->SetClippingChanged(true);
      }

      //if (ImGui::InputFloat3("ClippingPMP", glm::value_ptr(p_vtk->m_clipping_pmp)))
      //{
      //  p_vtk->SetClippingChanged(true);
      //}
      //if (ImGui::InputFloat3("ClippingVMR", glm::value_ptr(p_vtk->m_clipping_vmr)))
      //{
      //  p_vtk->SetClippingChanged(true);
      //}

      if (ImGui::Checkbox("LegendFiltering", &p_vtk->b_legend_filtering))
      {
        p_vtk->SetLegendFiltering(p_vtk->b_legend_filtering);
      }

     }

    ImGui::End();

  } 

  void RSImguiHandler::ShowClothSimulatorItems()
  {
    if (!RSResourceManager::GetInstance()->GetParticleManager()->IsUseCloth())
      return;

    RS_Cloth::RSClothSimulator* p_cloth = RSResourceManager::GetInstance()->GetParticleManager()->GetClothSimulator();
    if (!p_cloth)
      return;

    ImGui::Begin("Cloth Simulator Control");

    //********************************************************************************
    // Simulation Control
    //********************************************************************************
    ImGui::Text("Simulation Control");
    ImGui::Separator();

    // Play/Pause button
    if (p_cloth->IsPlaying())
    {
      if (ImGui::Button("Pause"))
        p_cloth->SetPlaying(false);
    }
    else
    {
      if (ImGui::Button("Play"))
        p_cloth->SetPlaying(true);
    }
    ImGui::SameLine();

    // Reset button
    if (ImGui::Button("Reset"))
      p_cloth->ResetSimulation();

    // Solver type
    int solver_type = static_cast<int>(p_cloth->GetSolverType());
    if (ImGui::Combo("Solver Type", &solver_type, "Mass-Spring\0PBD\0"))
      p_cloth->SetSolverType(static_cast<RSClothSolverType>(solver_type));

    // Info display
    ImGui::Text("Particles: %d", p_cloth->GetParticleCount());
    ImGui::Text("Springs: %d", p_cloth->GetSpringCount());
    ImGui::Text("Triangles: %d", p_cloth->GetTriangleCount());

    ImGui::Spacing();

    //********************************************************************************
    // Cloth Properties
    //********************************************************************************
    if (ImGui::CollapsingHeader("Cloth Properties", ImGuiTreeNodeFlags_DefaultOpen))
    {
      // Stiffness parameters
      if (ImGui::SliderFloat("Structural Stiffness", &p_cloth->m_cloth_property.structural_stiffness, 1.0f, 200.0f))
        p_cloth->b_property_changed = true;

      if (ImGui::SliderFloat("Shear Stiffness", &p_cloth->m_cloth_property.shear_stiffness, 1.0f, 100.0f))
        p_cloth->b_property_changed = true;

      if (ImGui::SliderFloat("Bend Stiffness", &p_cloth->m_cloth_property.bend_stiffness, 0.1f, 50.0f))
        p_cloth->b_property_changed = true;

      if (ImGui::SliderFloat("Damping", &p_cloth->m_cloth_property.damping, 0.9f, 1.0f))
        p_cloth->b_property_changed = true;

      if (ImGui::SliderFloat("Gravity", &p_cloth->m_cloth_property.gravity, 0.0f, 20.0f))
        p_cloth->b_property_changed = true;

      // PBD specific
      if (p_cloth->GetSolverType() == RSClothSolverType::PBD)
      {
        if (ImGui::SliderInt("Solver Iterations", &p_cloth->m_cloth_property.solver_iterations, 1, 50))
          p_cloth->b_property_changed = true;
      }
    }

    ImGui::Spacing();

    //********************************************************************************
    // Wind Control
    //********************************************************************************
    if (ImGui::CollapsingHeader("Wind", ImGuiTreeNodeFlags_DefaultOpen))
    {
      glm::vec3 wind_dir = p_cloth->GetWindDirection();
      if (ImGui::SliderFloat3("Wind Direction", glm::value_ptr(wind_dir), -1.0f, 1.0f))
        p_cloth->SetWindDirection(wind_dir);

      float wind_strength = p_cloth->GetWindStrength();
      if (ImGui::SliderFloat("Wind Strength", &wind_strength, 0.0f, 50.0f))
        p_cloth->SetWindStrength(wind_strength);
    }

    ImGui::Spacing();

    //********************************************************************************
    // Collision Control
    //********************************************************************************
    if (ImGui::CollapsingHeader("Collision"))
    {
      // Sphere collision
      ImGui::Checkbox("Sphere Collision", &p_cloth->b_use_sphere_collision);
      if (p_cloth->b_use_sphere_collision)
      {
        ImGui::InputFloat3("Sphere Center", glm::value_ptr(p_cloth->m_collision_sphere.center));
        ImGui::SliderFloat("Sphere Radius", &p_cloth->m_collision_sphere.radius, 0.1f, 10.0f);
      }

      ImGui::Separator();

      // Plane collision
      ImGui::Checkbox("Plane Collision", &p_cloth->b_use_plane_collision);
      if (p_cloth->b_use_plane_collision)
      {
        ImGui::InputFloat3("Plane Point", glm::value_ptr(p_cloth->m_collision_plane.point));
        ImGui::InputFloat3("Plane Normal", glm::value_ptr(p_cloth->m_collision_plane.normal));
      }
    }

    ImGui::Spacing();

    //********************************************************************************
    // Init Settings (requires reset)
    //********************************************************************************
    if (ImGui::CollapsingHeader("Init Settings (Reset Required)"))
    {
      ImGui::InputFloat3("Start Position", glm::value_ptr(p_cloth->m_init_setting.start_position));
      ImGui::InputInt("Width", &p_cloth->m_init_setting.width);
      ImGui::InputInt("Height", &p_cloth->m_init_setting.height);
      ImGui::SliderFloat("Spacing", &p_cloth->m_init_setting.spacing, 0.05f, 1.0f);

      ImGui::Separator();
      ImGui::Text("Pin Constraints:");
      ImGui::Checkbox("Fix Top-Left", &p_cloth->m_init_setting.fix_top_left);
      ImGui::SameLine();
      ImGui::Checkbox("Fix Top-Right", &p_cloth->m_init_setting.fix_top_right);
      ImGui::Checkbox("Fix Bottom-Left", &p_cloth->m_init_setting.fix_bottom_left);
      ImGui::SameLine();
      ImGui::Checkbox("Fix Bottom-Right", &p_cloth->m_init_setting.fix_bottom_right);
      ImGui::Checkbox("Fix Top Edge", &p_cloth->m_init_setting.fix_top_edge);

      if (ImGui::Button("Apply & Reset"))
        p_cloth->ResetSimulation();
    }

    ImGui::End();
  }

  void RSImguiHandler::ShowViewport()
  {
    if (m_show_debug_buffer)
    {
      // Draw Final image by the viewport
      ImGui::Begin("Viewport", &m_show_debug_buffer);
      {
        ImGui::BeginChild("GameRender");

        // Get size from ImGui window
        ImVec2 size = ImGui::GetContentRegionAvail();

        // Get aspect ratio from application. width / height
        const float aspect_ratio = static_cast<float>(RS_Application::mRSAPPLICATION->GetWidth())
                                  / static_cast<float>(RS_Application::mRSAPPLICATION->GetHeight());

        size.x /= 2.0f;
        size.y = size.x / aspect_ratio;


        auto RS = RSResourceManager::GetInstance();

        //// Draw g-buffer images with 2 row 3 column
        /*ImGui::Text("Final");
        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetFinalTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));*/

        ImGui::Text("Position + metallic");
        ImGui::SameLine();
        ImGui::SetCursorPosX(size.x);
        ImGui::Text("Normal + roughness");

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetDeferredPositionTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));

        ImGui::SameLine();

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetDeferredNormalTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));


        ImGui::Text("Albedo texture + ambient");
        ImGui::SameLine();
        ImGui::SetCursorPosX(size.x);
        ImGui::Text("Depth texture");

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetDeferredAlbedoTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));

        ImGui::SameLine();

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetDepthMap()), size,
          ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("Fluid Position + metallic");
        ImGui::SameLine();
        ImGui::SetCursorPosX(size.x);
        ImGui::Text("Fluid Normal + roughness");

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetFluidDeferredPositionTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SameLine();
        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetFluidDeferredNormalTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("Fluid Albedo texture + ambient");
        ImGui::SameLine();
        ImGui::SetCursorPosX(size.x);
        ImGui::Text("Custom Depth texture");

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetFluidDeferredAlbedoTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SameLine();
        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetFluidDeferredDepthTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));


        ImGui::Text("HUD Frame");
        ImGui::SameLine();
        ImGui::SetCursorPosX(size.x);
        ImGui::Text("Shadow Frame");

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetHudTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SameLine();

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetShadowMap()), size,
          ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("SSAO Frame");
        ImGui::SameLine();
        ImGui::SetCursorPosX(size.x);
        ImGui::Text("SSAO Blur Frame");

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetSSAOTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SameLine();
        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetSSAOBlurTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Text("SSAO Noise Frame");
        ImGui::SameLine();
        ImGui::SetCursorPosX(size.x);
        ImGui::Text("SSR Frame");

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetSSAONoiseTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));
        ImGui::SameLine();
        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetSSRTexture()), size,
        //ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetSSRTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));

        // SSRBLUR
        ImGui::Text("SSR Blur Frame");
        ImGui::SameLine();
        //ImGui::SetCursorPosX(size.x);
        //ImGui::Text("SSR Blur Frame");

        ImGui::Image(reinterpret_cast<ImTextureID>(RS->GetBufferManager()->GetSSRBlurTexture()), size,
          ImVec2(0, 1), ImVec2(1, 0));
      }
      ImGui::EndChild();
      ImGui::End();
    }

    if (m_show_final_viewport)
    {
      ImGui::Begin("Final Viewport", &m_show_final_viewport,
                   ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
      {
        // Get aspect ratio from application. width / height
        const float aspect_ratio = static_cast<float>(RS_Application::mRSAPPLICATION->GetWidth())
          / static_cast<float>(RS_Application::mRSAPPLICATION->GetHeight());

        // Get size from ImGui window
        ImVec2 final_size = ImGui::GetContentRegionAvail();
        final_size.y = final_size.x / aspect_ratio;

        const ImVec2 image_pos = ImGui::GetCursorScreenPos();



        const auto rs = RSResourceManager::GetInstance();

        ImGui::Image(reinterpret_cast<ImTextureID>(rs->GetBufferManager()->GetFinalTexture()), final_size,
          ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::IsItemHovered())
        {
          const auto io = ImGui::GetIO();

          // Calculate mouse position in image space normalized to [0, 1]
          const ImVec2 mouse_pos = io.MousePos;
          auto mouse_pos_in_image = ImVec2(mouse_pos.x - image_pos.x, mouse_pos.y - image_pos.y);
          mouse_pos_in_image.x /= final_size.x;
          mouse_pos_in_image.y /= final_size.y;
          

          // Calculate mouse_pos_in_image to application space. (0, 0) ~ (width, height)
          mouse_pos_in_image.x *= static_cast<float>(RS_Application::mRSAPPLICATION->GetWidth());
          mouse_pos_in_image.y *= static_cast<float>(RS_Application::mRSAPPLICATION->GetHeight());


          // Set HandleCursorPosition to RS_Input::mRSINPUT
          RS_Input::mRSINPUT->HandleCursorPosition(mouse_pos_in_image.x, mouse_pos_in_image.y);

          // If mouse is clicked, handle mouse click
          for (auto[mouse_id, button] : m_mouse_map)
          {
            if (ImGui::IsMouseClicked(mouse_id))
            {
              MouseButtonHandler(button, true);
            }
            if (ImGui::IsMouseReleased(mouse_id))
            {
              MouseButtonHandler(button, false);
            }
          }

          // Wheel scroll
          if (io.MouseWheel != 0.0f)
          {
            RS_Input::mRSINPUT->HandleMouseScroll(0, static_cast<int>(io.MouseWheel));
          }

          // If Keyboard is pressed, handle keyboard press
          for (auto [key_id, button] : m_key_map)
          {
            // Pressed
            if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(key_id)))
              KeyButtonHandler(button, true);

            // Release
            if (ImGui::IsKeyReleased(static_cast<ImGuiKey>(key_id)))
              KeyButtonHandler(button, false);
          }

        }
      }
      ImGui::End();
    }

  }


  void RSImguiHandler::SelectedObjectByMouse(RSObject* obj_)
  {
    if (obj_ != nullptr)
      selected_object = obj_;
  }

  int RSImguiHandler::GetLayerIndex(const RSRenderLayer layer_)
  {
    switch (layer_)
    {
    case RSRenderLayer::R_SURFACE_OPAQUE:
      return 0;
    case RSRenderLayer::R_LINE_OPAQUE:
      return 1;
    case RSRenderLayer::R_POINT:
      return 2;
    case RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE:
      return 3;
    default:
      return 0;
    }
  }

  void RSImguiHandler::SetLayerIndex(const int index_)
  {
    switch (index_)
    {
    case 0:
      selected_object->SetRenderLayer(RSRenderLayer::R_SURFACE_OPAQUE);
      break;
    case 1:
      selected_object->SetRenderLayer(RSRenderLayer::R_LINE_OPAQUE);
      break;
    case 2:
      selected_object->SetRenderLayer(RSRenderLayer::R_POINT);
      break;
    case 3:
      selected_object->SetRenderLayer(RSRenderLayer::R_SURFACE_WIREFRAME_OPAQUE);
      break;
    default:
      break;
    }
  }

  void RSImguiHandler::MouseButtonHandler(RSMouse button, const bool down)
  {
    RS_Input::mRSINPUT->HandleMouseClick(static_cast<int>(button), down ? GLFW_PRESS : GLFW_RELEASE, 0);
  }

  void RSImguiHandler::KeyButtonHandler(RSKey key, const bool down)
  {
    RS_Input::mRSINPUT->HandleKey(static_cast<int>(key), down ? GLFW_PRESS : GLFW_RELEASE, 0);
  }

  void RSImguiHandler::MappingKeyMouseBinding()
  {
    const auto io = ImGui::GetIO();
    // Enroll m_key_map from ImGuiKey to RSKey
    m_key_map[ImGuiKey_Tab] = RSKey::TAB;
    m_key_map[ImGuiKey_LeftArrow] = RSKey::LEFT;
    m_key_map[ImGuiKey_RightArrow] = RSKey::RIGHT;
    m_key_map[ImGuiKey_UpArrow] = RSKey::UP;
    m_key_map[ImGuiKey_DownArrow] = RSKey::DOWN;

    // Control
    m_key_map[ImGuiKey_LeftCtrl] = RSKey::CTRL_L;
    m_key_map[ImGuiKey_RightCtrl] = RSKey::CTRL_R;
    m_key_map[ImGuiKey_LeftShift] = RSKey::SHIFT_L;
    m_key_map[ImGuiKey_RightShift] = RSKey::SHIFT_R;

    // Space
    m_key_map[ImGuiKey_Space] = RSKey::SPACE;

    // Insert ~ PageDown
    m_key_map[ImGuiKey_Insert] = RSKey::INSERT;
    m_key_map[ImGuiKey_Delete] = RSKey::DEL;
    m_key_map[ImGuiKey_Home] = RSKey::HOME;
    m_key_map[ImGuiKey_End] = RSKey::END;
    m_key_map[ImGuiKey_PageUp] = RSKey::PAGE_UP;
    m_key_map[ImGuiKey_PageDown] = RSKey::PAGE_DOWN;

    // Esc
    m_key_map[ImGuiKey_Escape] = RSKey::ESCAPE;

    // Function keys
    m_key_map[ImGuiKey_F1] = RSKey::F1;
    m_key_map[ImGuiKey_F2] = RSKey::F2;
    m_key_map[ImGuiKey_F3] = RSKey::F3;
    m_key_map[ImGuiKey_F4] = RSKey::F4;
    m_key_map[ImGuiKey_F5] = RSKey::F5;
    m_key_map[ImGuiKey_F6] = RSKey::F6;
    m_key_map[ImGuiKey_F7] = RSKey::F7;
    m_key_map[ImGuiKey_F8] = RSKey::F8;
    m_key_map[ImGuiKey_F9] = RSKey::F9;
    m_key_map[ImGuiKey_F10] = RSKey::F10;
    m_key_map[ImGuiKey_F11] = RSKey::F11;
    m_key_map[ImGuiKey_F12] = RSKey::F12;

    // Numbers
    m_key_map[ImGuiKey_0] = RSKey::NUM_0;
    m_key_map[ImGuiKey_1] = RSKey::NUM_1;
    m_key_map[ImGuiKey_2] = RSKey::NUM_2;
    m_key_map[ImGuiKey_3] = RSKey::NUM_3;
    m_key_map[ImGuiKey_4] = RSKey::NUM_4;
    m_key_map[ImGuiKey_5] = RSKey::NUM_5;
    m_key_map[ImGuiKey_6] = RSKey::NUM_6;
    m_key_map[ImGuiKey_7] = RSKey::NUM_7;
    m_key_map[ImGuiKey_8] = RSKey::NUM_8;
    m_key_map[ImGuiKey_9] = RSKey::NUM_9;
      
    // A to Z
    m_key_map[ImGuiKey_A] = RSKey::A;
    m_key_map[ImGuiKey_B] = RSKey::B;
    m_key_map[ImGuiKey_C] = RSKey::C;
    m_key_map[ImGuiKey_D] = RSKey::D;
    m_key_map[ImGuiKey_E] = RSKey::E;
    m_key_map[ImGuiKey_F] = RSKey::F;
    m_key_map[ImGuiKey_G] = RSKey::G;
    m_key_map[ImGuiKey_H] = RSKey::H;
    m_key_map[ImGuiKey_I] = RSKey::I;
    m_key_map[ImGuiKey_J] = RSKey::J;
    m_key_map[ImGuiKey_K] = RSKey::K;
    m_key_map[ImGuiKey_L] = RSKey::L;
    m_key_map[ImGuiKey_M] = RSKey::M;
    m_key_map[ImGuiKey_N] = RSKey::N;
    m_key_map[ImGuiKey_O] = RSKey::O;
    m_key_map[ImGuiKey_P] = RSKey::P;
    m_key_map[ImGuiKey_Q] = RSKey::Q;
    m_key_map[ImGuiKey_R] = RSKey::R;
    m_key_map[ImGuiKey_S] = RSKey::S;
    m_key_map[ImGuiKey_T] = RSKey::T;
    m_key_map[ImGuiKey_U] = RSKey::U;
    m_key_map[ImGuiKey_V] = RSKey::V;
    m_key_map[ImGuiKey_W] = RSKey::W;
    m_key_map[ImGuiKey_X] = RSKey::X;
    m_key_map[ImGuiKey_Y] = RSKey::Y;
    m_key_map[ImGuiKey_Z] = RSKey::Z;

    // Mouse button
    m_mouse_map[ImGuiMouseButton_Left] = RSMouse::LEFT;
    m_mouse_map[ImGuiMouseButton_Right] = RSMouse::RIGHT;
    m_mouse_map[ImGuiMouseButton_Middle] = RSMouse::MIDDLE;
  }

}