#include "pch.h"
#include "RSVTKViewer.h"
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Timer/RSDebugTimer.h"
#include "Systems/Application/RSApplication.h"
#include <bit>
#include <cstdint>
#include <filesystem>

namespace FS = std::filesystem;

constexpr char UNDER_BAR = '_';

const std::string DOUBLE_SLASH = "/\\";
const std::string VTK_FILE_EXTENSION = ".vtk";
const std::string CLIPPING_BOX_NAME = "clipping_box";
const std::string CLIPPING_SPHERE_NAME = "clipping_sphere";
const std::string CLIPPING_PLANE_NAME = "clipping_plane";



namespace RS_VTK
{
  using namespace RS_Shader;

  RSVTKViewer::RSVTKViewer()
  {
    m_particle_count = 0;
    m_vtk_file_id = 0;
    m_vtk_point_vao = 0;
    m_position_vbo = 0;
    m_attribute_data.clear();
    m_attribute_name.clear();
    ClearData();
    EnrollDataSizeMap();

    m_cur_attribute_index = 0;
    m_target_legend_texture = RSResourceManager::GetInstance()->GetTextureManager()->GetTexture_UINT(LEGEND_TEXTURE_NAME);

  }

  RSVTKViewer::~RSVTKViewer()
  = default;

  void RSVTKViewer::Initialize()
  {  }

  void RSVTKViewer::ShutDown()
  {
    // Clear vector data
    ClearData();
    // Clear buffer objects
    DeleteBufferObject();
    m_target_legend_texture = 0;
    m_vtk_main_file_name = "";
    m_vtk_file_path_origin = "";
    std::free(m_data_buffer);
  }

  void RSVTKViewer::Update(float dt)
  {
    // Update the clipping object transform matrix
    UpdateClippingData();
  }

  void RSVTKViewer::Draw(const RSDataRenderType render_type_)
  {
    // If not loaded, return
    if (!b_is_loaded)
      return;

    const field_data_attribute attribute = m_vtk_header_data_list.at(m_vtk_file_id).field_data.
                                                                  at(m_cur_attribute_index);

    // Set shader and bind uniform data by the attribute data type
    switch (attribute.attribute_data_type)
    {
    case AttributeDataType::NONE:
      break;
    case AttributeDataType::SINGLE_FLOAT:
      SetShaderUniformSingleFloatVtk(render_type_);
      break;
    case AttributeDataType::THREE_FLOAT:
      SetShaderUniformThreeFloatVtk(render_type_);
      break;
    case AttributeDataType::SINGLE_UNSIGNED_INT:
      SetShaderUniformSingleUintVtk(render_type_);
      break;
    case AttributeDataType::SINGLE_UNSIGNED_CHAR:
      SetShaderUniformSingleUintVtk(render_type_);
      break;
    }
    // TODO : Add the other rendering type (streamline (line), isosurface (marching cube))
    DrawPoint();
  }

  void RSVTKViewer::Reset()
  {
    m_particle_count  = 0;
    m_vtk_file_id     = 0;

    ClearData();
    DeleteBufferObject();
    m_vtk_main_file_name    = "";
    m_vtk_file_path_origin  = "";
    std::free(m_data_buffer);

    m_cur_attribute_index   = 0;
    //m_target_legend_texture = RSResourceManager::GetInstance()->GetTextureManager()->
    //                                                            GetTexture_UINT(LEGEND_TEXTURE_NAME);
  }

  void RSVTKViewer::LoadVtkFile(const std::string& file_path)
  {
    // If the dir of the loading file is not the same as the previous one, reset the data and parsing the directory.
    if (!CheckIsSamePrevDir(file_path))
    {
      b_is_loaded = false;
      Reset();
      ParsingDataFromDir(file_path);
    }
    // else, update the vtk file id by the file path
    else
    {
      // get m_vtk_file_id from file_path
      SetVtkIdByFilepath(file_path);
    }

    // When the file is loaded, read the file and change the buffer object
    if (ReadFile(m_vtk_file_id))
    {
      ChangeBufferObject();
      b_is_loaded = true;
    }
        
  }

  void RSVTKViewer::UpdateVtkFile(const unsigned int vtk_id)
  {
    // When the file is loaded successfully, read the file and change the buffer object
    if (ReadFile(vtk_id))
      ChangeBufferObject();
  }


  void RSVTKViewer::LoadNextVtkFile()
  {
    RS_Timer::RSDebugTimer::GetInstance()->Start();

    // If not loaded, return
    if (!b_is_loaded)
      return;

    const unsigned int next_file_id = m_vtk_file_id+1;

    // Check if the index is valid
    if (next_file_id > m_max_file_id)
      return;

    // Read file and change the buffer object
    UpdateVtkFile(next_file_id);

    RS_Timer::RSDebugTimer::GetInstance()->EndNPrint("Load VTK Data time ");
    //RS_Timer::RSDebugTimer::GetInstance()->EndNLog("Load VTK Data time ");
  }

  void RSVTKViewer::LoadPrevVtkFile()
  {
    // If not loaded, return
    if (!b_is_loaded)
      return;

    const unsigned int prev_file_id = m_vtk_file_id-1;

    // Check if the index is valid
    if (prev_file_id > m_max_file_id)
      return;

    // Read file and change the buffer object
    UpdateVtkFile(prev_file_id);
  }

  void RSVTKViewer::LoadFirstVtkFile()
  {
    // If not loaded, return
    if (!b_is_loaded)
      return;

    // Set the first file index
    constexpr unsigned int first_vtk_index = 0;

    // Read file and change the buffer object
    UpdateVtkFile(first_vtk_index);
  }

  void RSVTKViewer::ChangeAttributeIndex(const unsigned int index_)
  {
    // Check if the index is valid
    if (index_ != std::clamp(0u, index_, m_max_attribute_index))
    {
      RS_WARN("Invalid vtk attribute index");
      return;
    }

    // Set the current attribute index
    m_cur_attribute_index = index_;

    // Open the origin vtl file with binary mode
    std::ifstream file(m_vtk_file_path_origin, std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open the VTK file.");
    }

    // Update the data buffer void pointer
    ReadDataWithAttribute(file);

    file.close();

    // Bind the buffer from the data buffer
    ChangeBufferObject();
    // Update the min/max value
    AsyncMinMaxValueLegend();
    
  }

  bool RSVTKViewer::CheckIsSamePrevDir(const std::string& file_path) const
  {
    // If the previous file path is empty, return false
    return (m_vtk_file_dir == FS::path(file_path).parent_path().string());
  }

  void RSVTKViewer::SetVtkIdByIdMap(const std::vector<unsigned int>* sorted_file_id_vec, const unsigned int id_)
  {
    // find the index of the id_ in the sorted_file_id_vec
    //for (unsigned int i = 0; i < sorted_file_id_vec->size(); ++i)
    //{
    //  if (sorted_file_id_vec->at(i) == id_)
    //  {
    //    m_vtk_file_id = i;
    //    break;
    //  }
    //}

    // optimization upper for loop
    m_vtk_file_id = static_cast<unsigned int>(
      std::distance(sorted_file_id_vec->begin(),
        std::ranges::find(*sorted_file_id_vec, id_)));

    m_max_file_id = static_cast<unsigned int>(sorted_file_id_vec->size()) - 1u;
  }

  void RSVTKViewer::SetVtkIdByFilepath(const std::string& file_path)
  {
    // Get the file name from the file path
    const std::string file_name = FS::path(file_path).stem().string();

    // Get the number from the file name
    const unsigned int num = std::stoi(file_name.substr(file_name.find_last_of(UNDER_BAR) + 1));

    // Find the vtk index by vtk file id
    //for (size_t i = 0; i < m_vtk_header_data_list.size(); ++i)
    //{
    //  if (m_vtk_header_data_list[i].data_id == num)
    //  {
    //    m_vtk_file_id = static_cast<unsigned int>(i);
    //    break;
    //  }
    //}

    // optimization upper for loop
    m_vtk_file_id = static_cast<unsigned int>(
      std::distance(m_vtk_header_data_list.begin(),
                    std::ranges::find_if(
                      m_vtk_header_data_list,
                    [num](const vtk_file_header_data& data)
                    { return data.data_id == num; }
                    )));

  }

  void RSVTKViewer::ClearData()
  {
    // Clear the data vector

    m_positions.clear();
    m_positions.shrink_to_fit();

    m_attribute_name.clear();
    m_attribute_name.shrink_to_fit();
    m_attribute_data.clear();
    m_attribute_data.shrink_to_fit();

    m_vtk_header_data_list.clear();
    m_vtk_header_data_list.shrink_to_fit();
  }

  void RSVTKViewer::SetShaderUniformThreeFloatVtk(const RSDataRenderType render_type_)
  {
    const auto m_rm = RSResourceManager::GetInstance();

    const RSShaderNames shader_type = (render_type_ == RSDataRenderType::FORWARD)
      ? RSShaderNames::POINT_THREE_FLOAT_VTK
      : RSShaderNames::DEFERRED_POINT_THREE_FLOAT_VTK;

    // Update three float (vec3) uniform data
    m_rm->GetShaderManager()->Use(shader_type);
    m_rm->GetShaderManager()->SetData(shader_type, "view",
                                                    m_rm->GetCamera()->GetView());
    m_rm->GetShaderManager()->SetData(shader_type, "projection",
                                                    m_rm->GetCamera()->GetProj());
    m_rm->GetShaderManager()->SetData(shader_type, "clipping_obj_transform",
                                                      m_object_transform);
    m_rm->GetShaderManager()->SetData(shader_type, "near",
                                                    m_rm->GetCamera()->GetNear());
    m_rm->GetShaderManager()->SetData(shader_type, "far",
                                                    m_rm->GetCamera()->GetFar());
    if (b_clipping_changed)
    {
      m_rm->GetShaderManager()->SetData(shader_type, "point_size", m_point_size);

      m_rm->GetShaderManager()->SetData(shader_type, "b_clip_invert",
                                                      b_invert_clipping);
      m_rm->GetShaderManager()->SetData(shader_type, "clipping_type",
                                                      static_cast<int>(m_clipping_type));
      m_rm->GetShaderManager()->SetData(shader_type, "b_legend_filtering",
                                                      b_legend_filtering);
      m_rm->GetShaderManager()->
                          SetData(shader_type, "value_min", m_min_range_vec3);
      m_rm->GetShaderManager()->
                          SetData(shader_type, "value_max", m_max_range_vec3);
      b_clipping_changed = false;
    }
  }

  void RSVTKViewer::SetShaderUniformSingleFloatVtk(RSDataRenderType render_type_)
  {
    const auto m_rm = RSResourceManager::GetInstance();

    const RSShaderNames shader_type = (render_type_ == RSDataRenderType::FORWARD)
      ? RSShaderNames::POINT_SINGLE_FLOAT_VTK
      : RSShaderNames::DEFERRED_POINT_SINGLE_FLOAT_VTK;
    
    // Update single float uniform data
    m_rm->GetShaderManager()->Use(shader_type);
    m_rm->GetShaderManager()->SetData(shader_type, "view",
                                                    m_rm->GetCamera()->GetView());
    m_rm->GetShaderManager()->SetData(shader_type, "projection",
                                                    m_rm->GetCamera()->GetProj());
    m_rm->GetShaderManager()->SetData(shader_type, "clipping_obj_transform",
                                                      m_object_transform);
    m_rm->GetShaderManager()->SetData(shader_type, "near",
                                                    m_rm->GetCamera()->GetNear());
    m_rm->GetShaderManager()->SetData(shader_type, "far",
                                                    m_rm->GetCamera()->GetFar());
    if (b_clipping_changed)
    {
      m_rm->GetShaderManager()->SetData(shader_type, "point_size", m_point_size);

      m_rm->GetShaderManager()->SetData(shader_type, "b_clip_invert",
                                                  b_invert_clipping);
      m_rm->GetShaderManager()->SetData(shader_type, "clipping_type",
                                                      static_cast<int>(m_clipping_type));
      m_rm->GetShaderManager()->SetData(shader_type, "b_legend_filtering",
                                                  b_legend_filtering);
      m_rm->GetShaderManager()->SetData(shader_type, "legend_min", m_min_legend);
      m_rm->GetShaderManager()->SetData(shader_type, "legend_max", m_max_legend);
      b_clipping_changed = false;
    }
  }

  void RSVTKViewer::SetShaderUniformSingleUintVtk(RSDataRenderType render_type_)
  {
    const auto m_rm = RSResourceManager::GetInstance();

    const RSShaderNames shader_type = (render_type_ == RSDataRenderType::FORWARD)
                                  ? RSShaderNames::POINT_SINGLE_UINT_VTK
                                  : RSShaderNames::DEFERRED_POINT_SINGLE_UINT_VTK;


    // Update single unsigned int uniform data
    m_rm->GetShaderManager()->Use(shader_type);
    m_rm->GetShaderManager()->SetData(shader_type, "view",
                                                    m_rm->GetCamera()->GetView());
    m_rm->GetShaderManager()->SetData(shader_type, "projection",
                                                    m_rm->GetCamera()->GetProj());
    m_rm->GetShaderManager()->SetData(shader_type, "clipping_obj_transform",
                                                      m_object_transform);
    m_rm->GetShaderManager()->SetData(shader_type, "near",
                                                    m_rm->GetCamera()->GetNear());
    m_rm->GetShaderManager()->SetData(shader_type, "far",
                                                    m_rm->GetCamera()->GetFar());

    if (b_clipping_changed)
    {
      m_rm->GetShaderManager()->SetData(shader_type, "point_size", m_point_size);

      m_rm->GetShaderManager()->SetData(shader_type, "b_clip_invert",
                                                      b_invert_clipping);
      m_rm->GetShaderManager()->SetData(shader_type, "clipping_type",
                                                      static_cast<int>(m_clipping_type));
      m_rm->GetShaderManager()->SetData(shader_type, "b_legend_filtering",
                                                      b_legend_filtering);
      m_rm->GetShaderManager()->SetData(shader_type, "legend_min", m_min_legend);
      m_rm->GetShaderManager()->SetData(shader_type, "legend_max", m_max_legend);
      b_clipping_changed = false;
    }
  }

  void RSVTKViewer::DrawPoint()
  {
    const auto m_resource_manager = RSResourceManager::GetInstance();
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_resource_manager->GetTextureManager()->GetTexture_UINT(PARTICLE_TEXTURE_NAME));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_target_legend_texture);

    // Draw particles
    glBindVertexArray(m_vtk_point_vao);
    glDrawArrays(GL_POINTS, 0, static_cast<int>(m_particle_count));
    glBindVertexArray(0);
  }

  void RSVTKViewer::SetNextAttributeData()
  {
    if (m_cur_attribute_index < m_max_attribute_index)
    {
      ChangeAttributeIndex(++m_cur_attribute_index);
    }
  }

  void RSVTKViewer::SetPrevAttributeData()
  {
    if (m_cur_attribute_index > 0)
    {
      ChangeAttributeIndex(--m_cur_attribute_index);
    }
  }

  const std::vector<std::string>* RSVTKViewer::GetAttributeNames() const
  {
    return &m_attribute_name;
  }

  void RSVTKViewer::SetPointSize(const float point_size)
  {
    m_point_size = point_size;
    SetClippingChanged(true);
  }

  void RSVTKViewer::SetMaxValueLegend(const float value_)
  {
    m_max_legend = value_;
    SetClippingChanged(true);
  }

  void RSVTKViewer::SetMinValueLegend(const float value_)
  {
    m_min_legend = value_;
    SetClippingChanged(true);
  }

  void RSVTKViewer::SetMinValueLegendVec3(const glm::vec3& value_)
  {
    m_min_range_vec3 = value_;
    SetClippingChanged(true);
  }

  void RSVTKViewer::SetMaxValueLegendVec3(const glm::vec3& value_)
  {
    m_max_range_vec3 = value_;
    SetClippingChanged(true);
  }

  void RSVTKViewer::AsyncMinMaxValueLegend()
  {
    m_min_legend = m_min;
    m_max_legend = m_max;
    m_min_range_vec3 = m_min_vec3;
    m_max_range_vec3 = m_max_vec3;
    SetClippingChanged(true);
  }


  void RSVTKViewer::SetLegendTextureId(const unsigned int texture_)
  {
    m_target_legend_texture = texture_;
  }


  void RSVTKViewer::SetClippingType(const ClippingType type)
  {
    m_clipping_type = type;
    SwitchClippingObject();
    SetClippingChanged(true);
  }

  void RSVTKViewer::SetClippingInvert(const bool invert)
  {
    b_invert_clipping = invert;
    SetClippingChanged(true);
  }

  void RSVTKViewer::SetLegendFiltering(const bool filtering)
  {
    b_legend_filtering = filtering;
    SetClippingChanged(true);
  }



  void RSVTKViewer::SetClippingType(const int clipping_type_)
  {
    switch (clipping_type_)
    {
    case 0:
      m_clipping_type = ClippingType::NONE;
      break;
    case 1:
      m_clipping_type = ClippingType::CUBE;
      break;
    case 2:
      m_clipping_type = ClippingType::SPHERE;
      break;
    case 3:
      m_clipping_type = ClippingType::PLANE;
      break;
    default:
      break;
    }

    SwitchClippingObject();
  }

  void RSVTKViewer::SwitchClippingObject()
  {
    // Remove the clipping object
    RemoveClippingObjects();
    // Create the clipping object
    CreateClippingObjects();
  }

  void RSVTKViewer::RemoveClippingObjects()
  {
    const auto m_resource_manager = RSResourceManager::GetInstance();
    // Remove the clipping object. If the object is not exist, it will be ignored.
    m_resource_manager->GetObjectManager()->RemoveObject(CLIPPING_BOX_NAME);
    m_resource_manager->GetObjectManager()->RemoveObject(CLIPPING_SPHERE_NAME);
    m_resource_manager->GetObjectManager()->RemoveObject(CLIPPING_PLANE_NAME);
  }

  void RSVTKViewer::CreateClippingObjects()
  {
    const auto m_resource_manager = RSResourceManager::GetInstance();
    const auto object_manager = m_resource_manager->GetObjectManager();
    switch (m_clipping_type)
    {
    case ClippingType::NONE:
      break;
    case ClippingType::CUBE:
      {
        const auto obj = object_manager->CreateCubeObject(CLIPPING_BOX_NAME);
        // Set Transparent to 0.2
        dynamic_cast<RS_Component::RSModel*>(obj->GetComponent(RSComponentType::CT_MODEL))->SetTransparency(0.2f);
        // Set the scale of the object
        obj->GetTransform()->SetScale(glm::vec3(10.0f));
        break;
      }
    case ClippingType::SPHERE:
      {
        const auto obj = object_manager->CreateSphereObject(CLIPPING_SPHERE_NAME);
        // Set Transparent to 0.2
        dynamic_cast<RS_Component::RSModel*>(obj->GetComponent(RSComponentType::CT_MODEL))->SetTransparency(0.2f);
        // Set the scale of the object
        obj->GetTransform()->SetScale(glm::vec3(10.0f));
        break;
      }

    case ClippingType::PLANE:
      {
        const auto obj = object_manager->CreatePlaneObject(CLIPPING_PLANE_NAME);
        // Set Transparent to 0.2
        dynamic_cast<RS_Component::RSModel*>(obj->GetComponent(RSComponentType::CT_MODEL))->SetTransparency(0.2f);
        // Set the scale of the object
        obj->GetTransform()->SetScale(glm::vec3(10.0f));
        break;
      }
    }
  }

  void RSVTKViewer::UpdateClippingData()
  {
    switch (m_clipping_type)
    {
    case ClippingType::NONE:
      m_object_transform = glm::mat4(1.0f);
      break;
    case ClippingType::CUBE:
      m_object_transform = RSResourceManager::GetInstance()->GetObjectManager()->GetObjectW(CLIPPING_BOX_NAME)->
        GetTransform()->GetTransformMatrix();
      break;
    case ClippingType::SPHERE:
      m_object_transform = RSResourceManager::GetInstance()->GetObjectManager()->GetObjectW(CLIPPING_SPHERE_NAME)->
        GetTransform()->GetTransformMatrix();
      break;
    case ClippingType::PLANE:
      m_object_transform = RSResourceManager::GetInstance()->GetObjectManager()->GetObjectW(CLIPPING_PLANE_NAME)->
        GetTransform()->GetTransformMatrix();
      break;
    }
  }

  void RSVTKViewer::EnrollDataSizeMap()
  {
    m_data_size_map.clear();
    m_data_size_map["float"] = 4;
    m_data_size_map["int"] = 4;
    m_data_size_map["unsigned_char"] = 1;
    m_data_size_map["unsigned_int"] = 4;
  }

//  bool RSVTKViewer::ReadFile(const std::string& file_path)
//  {
//    /*
//     *Logic
//      1. Header read
//        Vtk data file version
//        Vtk output
//        BINARY
//        POINTS n type
//        --- Binary data ---
//      2. get data type(ascii or binary), count of points (once)
//      3. get data list(name, type, size, is_single_byte,offset) (once)
//      4. move data selected offset to memory
//        SPH : particle size, position, data
//        If particle count is same with prev data, skip find offset
//
//        LBM : data
//
//      5. find min/max (?) (option)
//     *
//     *
//     */
//
//     // Stack data
//    bool is_same_particle_count = false;
//
//    // Open the VTK file with binary mode
//    std::ifstream file(file_path, std::ios::binary);
//    if (!file.is_open()) {
//      throw std::runtime_error("Failed to open the VTK file.");
//    }
//
//    m_vtk_file_path_origin = file_path;
//    // Read the header data.
//
//    std::string line;
//    // Read header lines to reach the binary data section
//    while (std::getline(file, line)) {
//      // Check if the data is binary
//      if (line.starts_with("BINARY")) {
//        b_binary = true;
//      }
//      else if (line.starts_with("ASCII")) {
//        b_binary = false;
//      }
//
//      // DATASET UNSTRUCTURED_GRID
//      //// If "UNSTRUCTURED_GRID" is found, it is LBM data
//      //if (line.starts_with("DATASET UNSTRUCTURED_GRID")) {
//      //  b_is_SPH = false;
//      //}
//      //// If "POLYDATA" is found, it is SPH data
//      //else if (line.starts_with("DATASET POLYDATA")) {
//      //  b_is_SPH = true;
//      //}
//
//      // Optimization of upper code.
//      if (line.starts_with("DATASET"))
//      {
//        if (line.find("UNSTRUCTURED_GRID") != std::string::npos) {
//          b_is_sph = false;
//        }
//        else if (line.find("POLYDATA") != std::string::npos) {
//          b_is_sph = true;
//        }
//
//      }
//
//      // jump to the "POINTS" line
//      if (line.starts_with("POINTS")) {
//        break;
//      }
//    }
//
//    // get data type(ascii or binary), count of points (once)
//
//
//    // When the file isn't binary, return false
//    if (!b_binary) {
//      return false;
//    }
//
//
//    std::string keyword, dataType;
//
//    std::istringstream(line) >> keyword >> m_particle_count >> dataType;
//
//    std::cout << "position : " << file.tellg() << "\n";
//    auto pos = file.tellg();
//
//    // Save offset of position data
//    m_point_data.data_offset = file.tellg();
//    m_point_data.name = "POINTS";
//
//    // Compare particle count with the previous one
//    is_same_particle_count = (m_particle_count == m_prev_particle_count);
//    if (!is_same_particle_count)
//      m_prev_particle_count = m_particle_count;
//
//
//    // Read the data list
//
//    if (!is_same_particle_count && b_is_sph)
//    {
//      m_attribute_data.clear();
//
//      file.seekg(m_particle_count * 12 + pos + 1);
//      pos = file.tellg();
//
//      std::getline(file, line);
//      //RS_PRINT(line.c_str());
//
//      // Jump "VERTICES 225000 450000" line.
//      long long dummy_counter, jumper;
//      std::istringstream(line) >> keyword >> dummy_counter >> jumper;
//      pos = file.tellg();
//      file.seekg(jumper * 4 + pos + 1);
//
//
//      // "POINT_DATA 85400" line.
//      std::getline(file, line);
//      //RS_PRINT(line.c_str());
//
//
//      // FIELD FieldData 19
//      std::getline(file, line);
//
//      // Parse the line. the field is 19.
//      unsigned int field_count = 0;
//      std::istringstream(line) >> keyword >> keyword >> field_count;
//
//      // If there is no field data, return false
//      if (field_count == 0)
//        return false;
//
//      // Reserve the field data
//      m_attribute_data.reserve(field_count);
//
//      // Initialize the current attribute index
//      //m_cur_attribute_index = 15;
//      m_max_attribute_index = field_count - 1;
//
//      // loop to read the field data
//      for (unsigned int i = 0; i < field_count; i++)
//      {
//        // Read the field data
//        std::getline(file, line);
//
//        // Parse "gas 1 85400 unsigned_char" line to field_data_attribute
//        std::string field_name, field_type;
//        unsigned int field_data_count = 0, field_data_size = 0;
//        bool is_single_byte = false;
//        std::istringstream(line) >> field_name >> field_data_size >> field_data_count >> field_type;
//
//        if (field_data_size == 1) {
//          is_single_byte = true;
//        }
//        else {
//          is_single_byte = false;
//        }
//
//        field_data_attribute field_data;
//        field_data.data_offset = file.tellg();
//        field_data.data_size = field_data_count;
//        field_data.is_single_byte = is_single_byte;
//        field_data.name = field_name;
//        field_data.type = field_type;
//
//        // Set AttributeDataType
//        if (field_type == "float" && is_single_byte) {
//          field_data.attribute_data_type = AttributeDataType::SINGLE_FLOAT;
//        }
//        else if (field_type == "float" && !is_single_byte) {
//          field_data.attribute_data_type = AttributeDataType::THREE_FLOAT;
//        }
//        else if (field_type == "int" || field_type == "unsigned_int") {
//          field_data.attribute_data_type = AttributeDataType::SINGLE_UNSIGNED_INT;
//        }
//        else if (field_type == "unsigned_char") {
//          field_data.attribute_data_type = AttributeDataType::SINGLE_UNSIGNED_CHAR;
//        }
//
//        m_attribute_data.push_back(field_data);
//
//        unsigned int data_size = m_data_size_map.find(field_type)->second;
//        pos = file.tellg();
//
//        // Jump to the next field data
//        file.seekg(field_data_count * data_size * field_data_size + pos + 1);
//      }
//
//      // Enroll attribute names to the vector
//      m_attribute_name.resize(field_count);
//      for (unsigned int i = 0; i < field_count; i++)
//      {
//        m_attribute_name[i] = m_attribute_data[i].name;
//      }
//
//    }
//
//    if (!b_is_sph)
//    {
//      file.close();
//      return true;
//    }
//
//    // When particle count isn't same with prev data, write the data to the m_positions.
//    //if (!is_same_particle_count || b_is_sph) {
//    if (!is_same_particle_count) {
//      m_positions.resize(m_particle_count);
//      // m_data_buffer was deallocated in the ClearData function.
//    }
//#if 0// using _byteswap_ulong
//    float coords[3];
//    // Read the binary data (big-endian float coordinates)
//    for (int i = 0; i < m_particle_count; ++i) {
//      file.read(reinterpret_cast<char*>(coords), sizeof(coords));
//
//      //if (!file) {
//      //  return false;
//      //}
//
//      // Convert from big-endian to native if needed
//      uint32_t* coordsInt = reinterpret_cast<uint32_t*>(coords);
//      for (int j = 0; j < 3; ++j) {
//        coordsInt[j] = bigEndianToNative(coordsInt[j]);
//      }
//
//      m_positions[i] = glm::vec3(coords[0], coords[1], coords[2]);
//    }
//#endif
//
//    // now, currently, this logic is only for sph code first, so I will implement the logic for lbm code later.
//    // So when it isn't sph code, close the file and return.
//
//
//#if 1 // using reverse bytes. this is faster than using _byteswap_ulong
//
//    if (b_is_sph)
//    {
//      // return to the position of the point data
//      file.seekg(m_point_data.data_offset);
//
//      // TODO : This buffer container move to the class member variable for the optimization.
//      std::vector<float> buffer(m_particle_count * 3);
//      file.read(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(float));
//
//      if (!file) {
//        RS_WARN("Error: Could not read point data from file.");
//        return false;
//      }
//
//      // VTK binary data is stored in big-endian format; convert if necessary
//      for (float& value : buffer) {
//        // Convert from big-endian to native if needed
//        const auto bytes = reinterpret_cast<unsigned char*>(&value);
//        std::swap(bytes[0], bytes[3]);
//        std::swap(bytes[1], bytes[2]);
//
//
//        // Convert from big-endian to native if needed
//        //const auto bytes = reinterpret_cast<unsigned char*>(&value);
//        //std::reverse(bytes, bytes + sizeof(float));
//      }
//
//      // Fill points with the read data
//      //std::memcpy(m_positions.data(), buffer.data(), buffer.size() * sizeof(float));
//      std::memmove(m_positions.data(), buffer.data(), buffer.size() * sizeof(float));
//      buffer.clear();
//    }
//
//#endif
//
//    ReadDataWithAttribute(file);
//  
//    //std::vector<field_data> buffer(m_particle_count * 3);
//    //std::vector<glm::vec3> buffer(m_particle_count * 3);
//    //std::vector<unsigned char> buffer(m_particle_count * 3);
//
//
//    //file.clear();
//    //while (std::getline(file, line)) {
//    //  if (line.starts_with("alphat")) {
//    //    break;
//    //  }
//    //}
//
//#if 0
//    // find min and max point (option)
//    glm::vec3 local_min = glm::vec3(std::numeric_limits<float>::max());
//    glm::vec3 local_max = glm::vec3(std::numeric_limits<float>::min());
//
//    for (int i = 0; i < m_particle_count; i++)
//    {
//      local_min = glm::min(local_min, m_positions[i]);
//      local_max = glm::max(local_max, m_positions[i]);
//    }
//#endif
//    // Close the file
//    file.close();
//
//    return true;
//
//  }

  bool RSVTKViewer::ReadFile(const unsigned int vtk_id)
  {
    /*
     * defined : Header already read to m_vtk_header_data_list
     * defined : It is sph data.
     * defined : All data is binary.
     * defined : All attribute map is already set.
     * defined : point_data_attribute is already set.
     *
     */

     // Convert the vtk_id to the file path
    const unsigned int vtk_file_number = m_vtk_header_data_list.at(vtk_id).data_id;
    const std::string  file_path       = m_vtk_file_dir + DOUBLE_SLASH + m_vtk_main_file_name + UNDER_BAR +
                                         std::to_string(vtk_file_number) + VTK_FILE_EXTENSION;

    // Open the VTK file with binary mode
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open the VTK file.");
    }

     // Set the vtk file id
    m_vtk_file_path_origin = file_path;
    m_vtk_file_id = vtk_id;
    m_particle_count = m_vtk_header_data_list.at(vtk_id).point_count;

    // Compare particle count with the previous one
    const bool is_same_particle_count = (m_particle_count == m_prev_particle_count);
    m_prev_particle_count = m_particle_count;

    // When particle count isn't same with prev data, write the data to the m_positions.
    if (!is_same_particle_count) {
      m_positions.resize(m_particle_count);
    }

    // Read the position data
    file.seekg(m_vtk_header_data_list.at(vtk_id).point_data.data_offset);
    std::vector<float> buffer(m_particle_count * 3);
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size() * sizeof(float));

    if (!file) {
      RS_WARN("Error: Could not read point data from file.");
      return false;
    }

    // VTK binary data is stored in big-endian format; convert if necessary
    for (float& value : buffer) {
      // Convert from big-endian to native if needed
      const auto bytes = reinterpret_cast<unsigned char*>(&value);
      std::swap(bytes[0], bytes[3]);
      std::swap(bytes[1], bytes[2]);
    }

    // Fill points with the read data
    std::memmove(m_positions.data(), buffer.data(), buffer.size() * sizeof(float));
    buffer.clear();

    // Read the attribute data
    ReadDataWithAttribute(file);

    // Close the file
    file.close();

    return true;
  }


  bool RSVTKViewer::ReadDataWithAttribute(std::ifstream& file)
  {
    // function to read the data until the end of the file
    // If it includes index of target data, it will write the data to the m_data.
    // The default index of target is 0.

    // Read the data
    const field_data_attribute attribute = m_vtk_header_data_list.at(m_vtk_file_id).field_data.
                                                                  at(m_cur_attribute_index);
    // Move to the data offset
    file.seekg(attribute.data_offset);

    // get type of data
    const unsigned int data_size = attribute.data_size;

    switch (attribute.attribute_data_type)
    {
    case AttributeDataType::NONE:
      break;
    case AttributeDataType::SINGLE_FLOAT:
      {
        if (void* mem = std::realloc(m_data_buffer, data_size * sizeof(float)))
        {
          m_data_buffer = mem;
        }
        else
        {
          RS_WARN("Failed to reallocate the data buffer");
          std::free(m_data_buffer);
          throw std::bad_alloc();
        }
        

        // Read the data
        file.read(static_cast<char*>(m_data_buffer), data_size * sizeof(float));

        if (!file) {
          RS_WARN("Error: Could not read point data from file.");
          return false;
        }

        // m_data_buffer covered by void pointer should be converted from big-endian to native if needed
        for (unsigned int i = 0; i < data_size; i++)
        {
          // Convert from big-endian to native if needed
          const auto bytes = static_cast<unsigned char*>(m_data_buffer);
          std::swap(bytes[i * 4 + 0], bytes[i * 4 + 3]);
          std::swap(bytes[i * 4 + 1], bytes[i * 4 + 2]);
        }

        break;
      }

    case AttributeDataType::THREE_FLOAT:
      {
        if (void* mem = std::realloc(m_data_buffer, 3u * data_size * sizeof(float)))
        {
          m_data_buffer = mem;
        }
        else
        {
          RS_WARN("Failed to reallocate the data buffer");
          std::free(m_data_buffer);
          throw std::bad_alloc();
        }

        // Read the data
        file.read(static_cast<char*>(m_data_buffer), 3u * data_size * sizeof(float));

        if (!file) {
          RS_WARN("Error: Could not read point data from file.");
          return false;
        }

        // m_data_buffer covered by void pointer should be converted from big-endian to native if needed
        for (unsigned int i = 0; i < 3 * data_size; i++)
        {
          // Convert from big-endian to native if needed
          const auto bytes = static_cast<unsigned char*>(m_data_buffer);
          std::swap(bytes[i * 4 + 0], bytes[i * 4 + 3]);
          std::swap(bytes[i * 4 + 1], bytes[i * 4 + 2]);
        }
        break;
      }
    case AttributeDataType::SINGLE_UNSIGNED_INT:
      {
        if (void* mem = std::realloc(m_data_buffer, data_size * sizeof(int)))
        {
          m_data_buffer = mem;
        }
        else
        {
          RS_WARN("Failed to reallocate the data buffer");
          std::free(m_data_buffer);
          throw std::bad_alloc();
        }

        // Read the data
        file.read(static_cast<char*>(m_data_buffer), data_size * sizeof(int));

        if (!file) {
          RS_WARN("Error: Could not read point data from file.");
          return false;
        }

        // m_data_buffer covered by void pointer should be converted from big-endian to native if needed
        for (unsigned int i = 0; i < data_size; i++)
        {
          // Convert from big-endian to native if needed
          const auto bytes = static_cast<unsigned char*>(m_data_buffer);
          std::swap(bytes[i * 4 + 0], bytes[i * 4 + 3]);
          std::swap(bytes[i * 4 + 1], bytes[i * 4 + 2]);
        }


        break;
      }
    case AttributeDataType::SINGLE_UNSIGNED_CHAR:
      {
        // This case is same with the case of SINGLE_UNSIGNED_INT.
        // But, It should be converted to int(4byte) from unsigned char(1byte).
        if (void* mem = std::realloc(m_data_buffer, data_size * sizeof(int)))
        {
          m_data_buffer = mem;
        }
        else
        {
          RS_WARN("Failed to reallocate the data buffer");
          std::free(m_data_buffer);
          throw std::bad_alloc();
        }

        // Read data
        auto* temp_buffer = new unsigned char[data_size];
        file.read(reinterpret_cast<char*>(temp_buffer), data_size * sizeof(unsigned char));

        // Convert unsigned char data to int
        const auto int_buffer = static_cast<int*>(m_data_buffer);
        //for (size_t i = 0; i < data_size; ++i) {
        //  int_buffer[i] = static_cast<int>(temp_buffer[i]); // Converting
        //}

        // Optimize upper loop code
        std::transform(temp_buffer, temp_buffer + data_size, int_buffer,
                       [](const unsigned char c) { return static_cast<int>(c); });


        // Delete the temporary buffer
        delete[] temp_buffer;

        if (!file) {
          RS_WARN("Error: Could not read point data from file.");
          return false;
        }

        break;
      }

    }

    ResetMinMaxValue();

    switch (attribute.attribute_data_type)
    {
    case AttributeDataType::NONE:
      {
        break;
      }
    case AttributeDataType::SINGLE_FLOAT:
    {
      // Find min and max value
      const auto float_data = static_cast<float*>(m_data_buffer);
      for (unsigned int i = 0; i < data_size; i++)
      {
        m_min = std::min(m_min, float_data[i]);
        m_max = std::max(m_max, float_data[i]);
      }
      break;
    }
    case AttributeDataType::SINGLE_UNSIGNED_INT:
    {
      // Find min and max value
      // Now, temporary, It will check with the float data.
      // TODO : It should be changed to the integer data include vec3
      unsigned int i_min = UINT_MAX;
      unsigned int i_max = 0;
      const auto int_data = static_cast<int*>(m_data_buffer);
      for (unsigned int i = 0; i < data_size; i++)
      {
        i_min = std::min(i_min, static_cast<unsigned int>(int_data[i]));
        i_max = std::max(i_max, static_cast<unsigned int>(int_data[i]));
      }
      // m_min and m_max cast to float from unsigned int
      m_min = static_cast<float>(i_min);
      m_max = static_cast<float>(i_max);
      break;
    }
    case AttributeDataType::THREE_FLOAT:
    {
      // Find min and max value
      // Now, temporary, It will check with the float data of x component.
      // TODO : It should be changed to the integer data include vec3
      const auto float_data = static_cast<float*>(m_data_buffer);
      for (unsigned int i = 0; i < data_size; i++)
      {
        m_min = std::min(m_min, float_data[i * 3]);
        m_max = std::max(m_max, float_data[i * 3]);
      }
      // upper code is temporary. It should be changed to the vec3 data.
      m_min_vec3 = glm::vec3(FLT_MAX);
      m_max_vec3 = glm::vec3(FLT_MIN);
      for (unsigned int i = 0; i < data_size; i++)
      {
        m_min_vec3.x = std::min(m_min_vec3.x, float_data[i * 3]);
        m_min_vec3.y = std::min(m_min_vec3.y, float_data[i * 3 + 1]);
        m_min_vec3.z = std::min(m_min_vec3.z, float_data[i * 3 + 2]);
        m_max_vec3.x = std::max(m_max_vec3.x, float_data[i * 3]);
        m_max_vec3.y = std::max(m_max_vec3.y, float_data[i * 3 + 1]);
        m_max_vec3.z = std::max(m_max_vec3.z, float_data[i * 3 + 2]);
      }
      break;
    }
    case AttributeDataType::SINGLE_UNSIGNED_CHAR:
    {
      // Find min and max value
      unsigned char c_min = UCHAR_MAX;
      unsigned char c_max = 0;
      // m_data_buffer = "01'00'00'00" -> 1
      // It should be compared with the integer size. It is 4byte. but data is unsigned char 1byte.
      // So, It should be converted to the integer.
      const auto int_data = static_cast<int*>(m_data_buffer);
      for (unsigned int i = 0; i < data_size; i++)
      {
        c_min = std::min(c_min, static_cast<unsigned char>(int_data[i]));
        c_max = std::max(c_max, static_cast<unsigned char>(int_data[i]));
      }
      // m_min and m_max cast to float from unsigned char
      m_min = static_cast<float>(c_min);
      m_max = static_cast<float>(c_max);
      break;
    }
    }

    //if (attribute.attribute_data_type == AttributeDataType::SINGLE_FLOAT)
    //{
    //  // Find min and max value
    //  const auto float_data = static_cast<float*>(m_data_buffer);
    //  for (int i = 0; i < data_size; i++)
    //  {
    //    m_min = std::min(m_min, float_data[i]);
    //    m_max = std::max(m_max, float_data[i]);
    //  }
    //}
    //else if (attribute.attribute_data_type == AttributeDataType::SINGLE_UNSIGNED_INT)
    //{
    //  // Find min and max value
    //  // Now, temporary, It will check with the float data.
    //  // TODO : It should be changed to the integer data include vec3


    //  unsigned int i_min = UINT_MAX;
    //  unsigned int i_max = 0;

    //  auto int_data = static_cast<int*>(m_data_buffer);
    //  for (int i = 0; i < data_size; i++)
    //  {
    //    i_min = std::min(i_min, static_cast<unsigned int>(int_data[i]));
    //    i_max = std::max(i_max, static_cast<unsigned int>(int_data[i]));
    //  }

    //  // m_min and m_max cast to float from unsigned int
    //  m_min = static_cast<float>(i_min);
    //  m_max = static_cast<float>(i_max);

    //}
    //else if (attribute.attribute_data_type == AttributeDataType::THREE_FLOAT)
    //{
    //  // Find min and max value
    //  // Now, temporary, It will check with the float data of x component.
    //  // TODO : It should be changed to the integer data include vec3
    //  auto float_data = static_cast<float*>(m_data_buffer);
    //  for (unsigned int i = 0; i < data_size; i++)
    //  {
    //    m_min = std::min(m_min, float_data[i * 3]);
    //    m_max = std::max(m_max, float_data[i * 3]);
    //  }

    //  // upper code is temporary. It should be changed to the vec3 data.

    //  m_min_vec3 = glm::vec3(FLT_MAX);
    //  m_max_vec3 = glm::vec3(FLT_MIN);

    //  for (unsigned int i = 0; i < data_size; i++)
    //  {
    //    m_min_vec3.x = std::min(m_min_vec3.x, float_data[i * 3]);
    //    m_min_vec3.y = std::min(m_min_vec3.y, float_data[i * 3 + 1]);
    //    m_min_vec3.z = std::min(m_min_vec3.z, float_data[i * 3 + 2]);
    //    m_max_vec3.x = std::max(m_max_vec3.x, float_data[i * 3]);
    //    m_max_vec3.y = std::max(m_max_vec3.y, float_data[i * 3 + 1]);
    //    m_max_vec3.z = std::max(m_max_vec3.z, float_data[i * 3 + 2]);
    //  }

    //}
    //else if (attribute.attribute_data_type == AttributeDataType::SINGLE_UNSIGNED_CHAR)
    //{
    //  // Find min and max value

    //  unsigned char c_min = UCHAR_MAX;
    //  unsigned char c_max = 0;

    //  // m_data_buffer = "01'00'00'00" -> 1
    //  // It should be compared with the integer size. It is 4byte. but data is unsigned char 1byte.
    //  // So, It should be converted to the integer.

    //  auto int_data = static_cast<int*>(m_data_buffer);
    //  for (unsigned int i = 0; i < data_size; i++)
    //  {
    //    c_min = std::min(c_min, static_cast<unsigned char>(int_data[i]));
    //    c_max = std::max(c_max, static_cast<unsigned char>(int_data[i]));
    //  }


    //  // m_min and m_max cast to float from unsigned char
    //  m_min = static_cast<float>(c_min);
    //  m_max = static_cast<float>(c_max);

    //}

    return true;
  }

  void RSVTKViewer::ParsingDataFromDir(const std::string& file_path)
  {
    //RS_Timer::RSDebugTimer::GetInstance()->Start();
    // This code be assumed that the file dir is different with the previous one or changed.

    const FS::path path(file_path);
    m_vtk_name = path.stem().string();
    m_vtk_main_file_name = m_vtk_name.substr(0, m_vtk_name.find_last_of(UNDER_BAR));
    m_vtk_file_dir = path.parent_path().string();
    const unsigned int vtk_solver_number = std::stoi(m_vtk_name.substr(m_vtk_name.find_last_of(UNDER_BAR) + 1));


    unsigned int vtk_file_count = 0;

    // Get the total file count
    const unsigned int total_files_count = 
      static_cast<unsigned int>(std::count_if(FS::directory_iterator(m_vtk_file_dir),
                    FS::directory_iterator(), 
                    [](const FS::directory_entry& entry) 
        { return entry.path().extension() == VTK_FILE_EXTENSION; }));

    std::vector<unsigned int> vtk_file_ids(total_files_count);

    // Get the file list in the directory
    for (const auto& entry : FS::directory_iterator(m_vtk_file_dir))
    {
      // Get only '.vtk' extension
      if (entry.path().extension() != VTK_FILE_EXTENSION)
        continue;

      // Get the vtk file id
      const unsigned int vtk_file_id = 
        std::stoi(entry.path().stem().string().substr(entry.path().stem().string().find_last_of(UNDER_BAR) + 1));
      vtk_file_ids[vtk_file_count] = vtk_file_id;
      vtk_file_count++;
    }

    // Sort the vtk file ids
    std::ranges::sort(vtk_file_ids);

    // Set the vtk file id by the first file number
    SetVtkIdByIdMap(&vtk_file_ids, vtk_solver_number);

    // Fill the m_vtk_file_id_list
    CreateAttributeMap(&vtk_file_ids);

    // Create the attribute name map
    CreateAttributeNameMap();

    // Set attribute data by the first vtk file
    SetAttributeDataFirstVtkData();

    // Create Buffer Object by maximum particle count and first attribute data
    CreateBufferObjectByDir();

    //RS_DEBUG("file count : %d", vtk_file_count);
    //RS_Timer::RSDebugTimer::GetInstance()->EndNPrint("Read Dir Data time ");
    //RS_Timer::RSDebugTimer::GetInstance()->EndNLog("Read Dir Data time ");
  }

  void RSVTKViewer::CreateAttributeMap(const std::vector<unsigned int>* sorted_file_id_vec)
  {
    m_vtk_header_data_list.resize(sorted_file_id_vec->size());

    // Use m_vtk_file_dir and m_vtk_main_file_name
    for (unsigned int index = 0; index < sorted_file_id_vec->size(); index++)
    {
      std::string file_path = m_vtk_file_dir + DOUBLE_SLASH + CreateOriginFileNameByNum(sorted_file_id_vec->at(index));

      m_vtk_header_data_list[index].data_id = sorted_file_id_vec->at(index);
      SetFieldDataFromFile(file_path, m_vtk_header_data_list[index]);
    }
  }

  void RSVTKViewer::CreateAttributeNameMap()
  {
    // Get the attribute names from the first vtk file
    if (m_vtk_header_data_list.empty())
      return;

    const size_t field_count = m_vtk_header_data_list.at(0).field_data.size();

    // Enroll attribute names to the vector
    m_attribute_name.resize(field_count);
    //for (size_t i = 0; i < field_count; i++)
    //{
    //  m_attribute_name[i] = m_vtk_header_data_list.at(0).field_data.at(i).name;
    //}

    // Optimize the upper loop code
    std::ranges::transform(m_vtk_header_data_list.at(0).field_data, m_attribute_name.begin(),
                           [](const field_data_attribute& field_data) { return field_data.name; });
  }


  void RSVTKViewer::SetFieldDataFromFile(const std::string& file_path, vtk_file_header_data& vtk_header_data_) const
  {
    bool is_binary = false;
    bool is_sph = false;
    // Open the VTK file with binary mode
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open the VTK file.");
    }

    // Read the header data.
    std::string line;
    // Read header lines to reach the binary data section
    while (std::getline(file, line)) {
      // Check if the data is binary
      if (line.starts_with("BINARY")) {
        is_binary = true;
      }
      else if (line.starts_with("ASCII")) {
        is_binary = false;
      }

      // DATASET UNSTRUCTURED_GRID
      //// If "UNSTRUCTURED_GRID" is found, it is LBM data
      //if (line.starts_with("DATASET UNSTRUCTURED_GRID")) {
      //  b_is_SPH = false;
      //}
      //// If "POLYDATA" is found, it is SPH data
      //else if (line.starts_with("DATASET POLYDATA")) {
      //  b_is_SPH = true;
      //}

      // Optimization of upper code.
      if (line.starts_with("DATASET"))
      {
        if (line.find("UNSTRUCTURED_GRID") != std::string::npos) {
          is_sph = false;
        }
        else if (line.find("POLYDATA") != std::string::npos) {
          is_sph = true;
        }

      }

      // jump to the "POINTS" line
      if (line.starts_with("POINTS")) {
        break;
      }
    }

    // get data type(ascii or binary), count of points (once)


    // When the file isn't binary, return false
    if (!is_binary) {
      return;
    }


    std::string keyword, data_type;

    // ex. "POINTS 85400 float" line
    std::istringstream(line) >> keyword >> vtk_header_data_.point_count >> data_type;

    // Save the offset of the point data
    auto pos = file.tellg();

    // Save offset of position data
    vtk_header_data_.point_data.data_offset = file.tellg();
    vtk_header_data_.point_data.name = "POINTS";

    // Read the data list
    if (is_sph)
    {
      vtk_header_data_.field_data.clear();

      file.seekg(vtk_header_data_.point_count * 12 + pos + 1);
      pos = file.tellg();

      std::getline(file, line);

      // Jump "VERTICES 225000 450000" line.
      long long dummy_counter, jumper;
      std::istringstream(line) >> keyword >> dummy_counter >> jumper;
      pos = file.tellg();
      file.seekg(jumper * 4 + pos + 1);

      // "POINT_DATA 85400" line.
      std::getline(file, line);

      // ex. FIELD FieldData 19
      std::getline(file, line);

      // Parse the line. the field is 19.
      unsigned int field_count = 0;
      std::istringstream(line) >> keyword >> keyword >> field_count;

      // If there is no field data, return false
      if (field_count == 0)
        return;

      // Reserve the field data
      vtk_header_data_.field_data.reserve(field_count);

      // Initialize the current attribute index

      // loop to read the field data
      for (unsigned int i = 0; i < field_count; i++)
      {
        // Read the field data
        std::getline(file, line);

        // Parse "gas 1 85400 unsigned_char" line to field_data_attribute
        std::string field_name, field_type;
        unsigned int field_data_count = 0, field_data_size = 0;
        bool is_single_byte = false;
        std::istringstream(line) >> field_name >> field_data_size >> field_data_count >> field_type;

        if (field_data_size == 1) {
          is_single_byte = true;
        }
        else {
          is_single_byte = false;
        }

        field_data_attribute field_data;
        field_data.data_offset = file.tellg();
        field_data.data_size = field_data_count;
        field_data.is_single_byte = is_single_byte;
        field_data.name = field_name;
        field_data.type = field_type;

        // Set AttributeDataType
        if (field_type == "float" && is_single_byte) {
          field_data.attribute_data_type = AttributeDataType::SINGLE_FLOAT;
        }
        else if (field_type == "float" && !is_single_byte) {
          field_data.attribute_data_type = AttributeDataType::THREE_FLOAT;
        }
        else if (field_type == "int" || field_type == "unsigned_int") {
          field_data.attribute_data_type = AttributeDataType::SINGLE_UNSIGNED_INT;
        }
        else if (field_type == "unsigned_char") {
          field_data.attribute_data_type = AttributeDataType::SINGLE_UNSIGNED_CHAR;
        }

        vtk_header_data_.field_data.push_back(field_data);

        unsigned int data_size = GetDataSizeMap(field_type);
        pos = file.tellg();

        // Jump to the next field data
        file.seekg(field_data_count * data_size * field_data_size + pos + 1);
      }

    }
  }

  unsigned int RSVTKViewer::GetDataSizeMap(const std::string& type) const
  {
    if (const auto it = m_data_size_map.find(type); it != m_data_size_map.end())
      return it->second;

    return 0;
  }

  unsigned int RSVTKViewer::GetMaxParticleCountOnDir() const
  {
    unsigned int max_particle_count = 0;
    for (const auto& header_data : m_vtk_header_data_list)
      max_particle_count = std::max(max_particle_count, header_data.point_count);

    return max_particle_count;
  }

  void RSVTKViewer::SetAttributeDataFirstVtkData()
  {
    // If the attribute data is empty, return
    if (m_vtk_header_data_list.empty())
      return;

    // Initialize
    m_cur_attribute_index = 0;

    // Set m_attribute_data to first of the vtk_header_data_list
    m_attribute_data = m_vtk_header_data_list.at(0).field_data;
    m_max_attribute_index = static_cast<unsigned int>(m_attribute_data.size()) - 1u;
  }

  std::string RSVTKViewer::CreateOriginFileNameByNum(const unsigned int num_) const
  {
    return m_vtk_main_file_name + UNDER_BAR + std::to_string(num_) + VTK_FILE_EXTENSION;
  }

  void RSVTKViewer::CreateBufferObjectByDir()
  {
    const field_data_attribute attribute = m_vtk_header_data_list.at(m_vtk_file_id).field_data.at(m_cur_attribute_index);

    // Create VAO
    glGenVertexArrays(1, &m_vtk_point_vao);
    glBindVertexArray(m_vtk_point_vao);

    // Create Position VBO
    glGenBuffers(1, &m_position_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_position_vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * GetMaxParticleCountOnDir() * sizeof(float), m_positions.data(), GL_DYNAMIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Create Data VBO
    glGenBuffers(1, &m_data_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_data_vbo);

    glEnableVertexAttribArray(1);

    // Branch by the attribute data type
    switch (attribute.attribute_data_type)
    {
    case AttributeDataType::NONE:
      break;
    case AttributeDataType::SINGLE_FLOAT:
      // Allocate m_data_buffer from the first attribute data
      m_data_buffer = std::malloc(attribute.data_size * sizeof(float));
      glBufferData(GL_ARRAY_BUFFER, attribute.data_size * sizeof(float), m_data_buffer, GL_DYNAMIC_DRAW);
      glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);

      break;
    case AttributeDataType::THREE_FLOAT:
      // Allocate m_data_buffer from the first attribute data
      m_data_buffer = std::malloc(attribute.data_size * 3 * sizeof(float));
      glBufferData(GL_ARRAY_BUFFER, attribute.data_size * 3 * sizeof(float), m_data_buffer, GL_DYNAMIC_DRAW);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
      break;
    case AttributeDataType::SINGLE_UNSIGNED_INT:
      // Allocate m_data_buffer from the first attribute data
      m_data_buffer = std::malloc(attribute.data_size * sizeof(int));
      glBufferData(GL_ARRAY_BUFFER, attribute.data_size * sizeof(int), m_data_buffer, GL_DYNAMIC_DRAW);
      glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, 0, (void*)0);
      break;
    case AttributeDataType::SINGLE_UNSIGNED_CHAR:
      // Allocate m_data_buffer from the first attribute data
      m_data_buffer = std::malloc(attribute.data_size * sizeof(int));
      glBufferData(GL_ARRAY_BUFFER, attribute.data_size * sizeof(int), m_data_buffer, GL_DYNAMIC_DRAW);
      glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, 0, (void*)0);
      break;
    }

    glBindVertexArray(0);
  }

  void RSVTKViewer::ChangeBufferObject()
  {
    const field_data_attribute attribute = m_vtk_header_data_list.at(m_vtk_file_id).field_data.
                                                                  at(m_cur_attribute_index);


    glBindVertexArray(m_vtk_point_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_position_vbo);
    //glBufferData(GL_ARRAY_BUFFER, 3 * m_particle_count * sizeof(float), m_positions.data(), GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * m_particle_count * sizeof(float), m_positions.data());

    // When changed, it should be changed the buffer size and change attribute pointer also.
    glEnableVertexAttribArray(1);
    switch (attribute.attribute_data_type)
    {
    case AttributeDataType::NONE:
      break;
    case AttributeDataType::SINGLE_FLOAT:
      glBindBuffer(GL_ARRAY_BUFFER, m_data_vbo);
      glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
      //glBufferSubData(GL_ARRAY_BUFFER, 0, m_attribute_data.at(m_cur_attribute_index).data_size * sizeof(float), m_data_buffer);
      glBufferData(GL_ARRAY_BUFFER, attribute.data_size * sizeof(float), m_data_buffer,
                   GL_DYNAMIC_DRAW);
      break;
    case AttributeDataType::THREE_FLOAT:
      glBindBuffer(GL_ARRAY_BUFFER, m_data_vbo);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
      //glBufferSubData(GL_ARRAY_BUFFER, 0, m_attribute_data.at(m_cur_attribute_index).data_size * 3 * sizeof(float), m_data_buffer);
      glBufferData(GL_ARRAY_BUFFER, attribute.data_size * 3 * sizeof(float),
                   m_data_buffer, GL_DYNAMIC_DRAW);

      // change buffer size also.
      break;
    case AttributeDataType::SINGLE_UNSIGNED_INT:
      glBindBuffer(GL_ARRAY_BUFFER, m_data_vbo);
      glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, 0, (void*)0);
      //glBufferSubData(GL_ARRAY_BUFFER, 0, m_attribute_data.at(m_cur_attribute_index).data_size * sizeof(int), m_data_buffer);
      glBufferData(GL_ARRAY_BUFFER, attribute.data_size * sizeof(int), m_data_buffer,
                   GL_DYNAMIC_DRAW);
      break;
    case AttributeDataType::SINGLE_UNSIGNED_CHAR:
      glBindBuffer(GL_ARRAY_BUFFER, m_data_vbo);
      glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, 0, (void*)0);
      //glBufferSubData(GL_ARRAY_BUFFER, 0, m_attribute_data.at(m_cur_attribute_index).data_size * sizeof(int), m_data_buffer);
      glBufferData(GL_ARRAY_BUFFER, attribute.data_size * sizeof(int), m_data_buffer,
                   GL_DYNAMIC_DRAW);
      break;
    }
    glBindVertexArray(0);
  }

  void RSVTKViewer::DeleteBufferObject()
  {
    glDeleteBuffers(1, &m_position_vbo);
    glDeleteBuffers(1, &m_data_vbo);
    glDeleteVertexArrays(1, &m_vtk_point_vao);
    m_vtk_point_vao = 0;
    m_position_vbo = 0;
    m_data_vbo = 0;

  }
  bool RSVTKViewer::IsFileExist(const std::string& file_path)
  {
    return std::ifstream(file_path).good();
  }
}
