#include "pch.h"
#include "RSPointClipper.h"
#include "Resource/Manager/RSResourceManager.h"
#include "Systems/Timer/RSDebugTimer.h"

namespace RS_PointClipper
{
  using namespace RS_Shader;
  RSPointClipper::RSPointClipper()
  {
  }

  RSPointClipper::~RSPointClipper()
  {
  }

  void RSPointClipper::Initialize()
  {
  }

  void RSPointClipper::ShutDown()
  {
    ResetData();
  }

  void RSPointClipper::Update(float dt)
  {
    if (!b_is_loaded)
      return;

    const auto m_object_manager = RSResourceManager::GetInstance()->GetObjectManager();
    const auto clipping_box = m_object_manager->GetObjectW("clipping_box");
    m_object_manager->SetPickingObject(clipping_box);

  }

  void RSPointClipper::Draw()
  {
    if (!b_is_loaded) {
      return;
    }
    const auto      m_resource_manager = RSResourceManager::GetInstance();
    const glm::mat4 box_transform = m_resource_manager->GetObjectManager()->GetObjectW("clipping_box")->GetTransform()->
                                                        GetTransformMatrix();

    m_resource_manager->GetShaderManager()->Use(RSShaderNames::POINT_CLIPPER);
    /*
    uniform mat4 view;
    uniform mat4 projection;
    uniform float point_size;
    */
    clipping_id = b_is_clipping ? 1 : 0;
    m_resource_manager->GetShaderManager()->SetData(RSShaderNames::POINT_CLIPPER, "view",
                                                    m_resource_manager->GetCamera()->GetView());
    m_resource_manager->GetShaderManager()->SetData(RSShaderNames::POINT_CLIPPER, "projection",
                                                    m_resource_manager->GetCamera()->GetProj());
    m_resource_manager->GetShaderManager()->SetData(RSShaderNames::POINT_CLIPPER, "point_size", m_point_size);
    m_resource_manager->GetShaderManager()->SetData(RSShaderNames::POINT_CLIPPER, "clipping_id", clipping_id);
    m_resource_manager->GetShaderManager()->SetData(RSShaderNames::POINT_CLIPPER, "clipping_color", m_clip_color);
    m_resource_manager->GetShaderManager()->SetData(RSShaderNames::POINT_CLIPPER, "box_transform", box_transform);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_resource_manager->GetTextureManager()->GetTexture_UINT("default"));

    glEnable(GL_BLEND);
    glBindVertexArray(m_data_vao);
    glDrawArrays(GL_POINTS, 0, m_point_count);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
  }

  void RSPointClipper::LoadPointData(const std::string& file_path)
  {
    RS_Timer::RSDebugTimer::GetInstance()->Start();

    // Clear previous data
    ClearData();

    m_point_cloud_file_path = file_path;
    m_point_cloud_file_name = file_path.substr(file_path.find_last_of("/\\") + 1);

    // format of the point cloud file:
    const std::string file_extension = file_path.substr(file_path.find_last_of(".") + 1);

    // the format will be 'bin' for binary read and 'ply' for ascii read
    if (file_extension == "bin") {
      // Read the point cloud data from the file by binary read.
      std::ifstream file(file_path, std::ios::in | std::ios::binary);
      if (!file.is_open()) {
        throw std::runtime_error("Failed to open the point cloud file.");
      }

      ReadBinPointData(file);
      CreateBufferObject();
      CalculateMinMax();
    }
    else if (file_extension == "ply") {
        // Read the point cloud data from the file by ascii read.
        std::ifstream file(file_path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open the point cloud file.");
        }
        
      ReadPlyPointData(file);
      CreateBufferObject();
      CalculateMinMax();
    }
    else if (file_extension == "las") {
      // Read the point cloud data from the file by LAS format read.
      std::ifstream file(file_path, std::ios::in | std::ios::binary);
      if (!file.is_open()) {
          throw std::runtime_error("Failed to open the point cloud file.");
      }

      ReadLasPointData(file);
      CreateBufferObject();
      CalculateMinMax();
    }
    else {
      throw std::runtime_error("Unsupported point cloud file format.");
    }


    CreateBox();
    SetCameraPosition();

    b_is_loaded = true;
    RS_PRINT("Point cloud data loaded successfully Debug.");

    RS_Timer::RSDebugTimer::GetInstance()->EndNPrint("Load Point Data time ");

  }

  void RSPointClipper::ReadBinPointData(std::ifstream& file)
  {
    /*
    Int32 (Point count)
    Float32 (X)
    Float32 (Y)
    Float32 (Z)
    Float32 (R)
    Float32 (G)
    Float32 (B)
    Float32 (X)
    Float32 (Y)
    ...
    */
    // first read the point count with int32
    file.read(reinterpret_cast<char*>(&m_point_count), sizeof(int32_t));

    const int size_of_data = m_point_count * 6; // count * 6(point, color) * 4(float)
    m_data.resize(size_of_data);

    file.read(reinterpret_cast<char*>(m_data.data()), size_of_data * sizeof(float));

    // End read
    file.close();
  }

  void RSPointClipper::ReadPlyPointData(std::ifstream& file)
  {
    // Currently, the ply format is ascii format.
    /*
    ply
    format ascii 1.0
    comment author: <String>
    element vertex <N>
    property float x
    property float y
    property float z
    property uchar red
    property uchar green
    property uchar blue
    end_header
    0.0 0.0 0.0 255 0 0
    1.0 0.0 0.0 0 255 0
    ...
    */

    std::string line;
    // Read header
    while (std::getline(file, line)) {
      if (line.substr(0, 14) == "element vertex") {
        m_point_count = std::stoi(line.substr(15));
      }
      if (line == "end_header") {
        break;
      }
    }
    const int size_of_data = m_point_count * 6; // count * 6(point, color)
    m_data.resize(size_of_data);


  // 전체 파일을 메모리로 읽기
  std::string buffer;
  std::string temp_line;
  while (std::getline(file, temp_line)) {
    buffer += temp_line + "\n";  // 줄바꿈 명시적으로 추가
  }
  file.close();
  
  // strtof를 사용한 수동 파싱
  const char* ptr = buffer.c_str();
  char* end_ptr = nullptr;
  const char* buffer_end = ptr + buffer.size();
   int index = 0;
  int parsed_count = 0;
  
  while (ptr < buffer_end && parsed_count < m_point_count) {
    // 공백과 줄바꿈 건너뛰기
    while (ptr < buffer_end && (*ptr == ' ' || *ptr == '\t' || *ptr == '\r' || *ptr == '\n')) {
      ptr++;
    }
    
    if (ptr >= buffer_end) break;
    
    // x, y, z 파싱
    float x = strtof(ptr, &end_ptr);
    if (ptr == end_ptr) {
      // 파싱 실패 - 다음 줄로 이동
      while (ptr < buffer_end && *ptr != '\n') ptr++;
      if (ptr < buffer_end) ptr++;
      continue;
    }
    ptr = end_ptr;
    
    float y = strtof(ptr, &end_ptr);
    if (ptr == end_ptr) {
      while (ptr < buffer_end && *ptr != '\n') ptr++;
      if (ptr < buffer_end) ptr++;
      continue;
    }
    ptr = end_ptr;
    
    float z = strtof(ptr, &end_ptr);
    if (ptr == end_ptr) {
      while (ptr < buffer_end && *ptr != '\n') ptr++;
      if (ptr < buffer_end) ptr++;
      continue;
    }
    ptr = end_ptr;
    
    long r = strtol(ptr, &end_ptr, 10);
    if (ptr == end_ptr) {
      while (ptr < buffer_end && *ptr != '\n') ptr++;
      if (ptr < buffer_end) ptr++;
      continue;
    }
    ptr = end_ptr;
    
    long g = strtol(ptr, &end_ptr, 10);
    if (ptr == end_ptr) {
      while (ptr < buffer_end && *ptr != '\n') ptr++;
      if (ptr < buffer_end) ptr++;
      continue;
    }
    ptr = end_ptr;
    
    long b = strtol(ptr, &end_ptr, 10);
    if (ptr == end_ptr) {
      while (ptr < buffer_end && *ptr != '\n') ptr++;
      if (ptr < buffer_end) ptr++;
      continue;
    }
    ptr = end_ptr;
    
    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
      while (ptr < buffer_end && *ptr != '\n') ptr++;
      if (ptr < buffer_end) ptr++;
      continue;
    }
    
    m_data[index++] = x;
    m_data[index++] = y;
    m_data[index++] = z;
    m_data[index++] = static_cast<float>(r) / 255.0f;
    m_data[index++] = static_cast<float>(g) / 255.0f;
    m_data[index++] = static_cast<float>(b) / 255.0f;
    
    parsed_count++;
  }

  m_point_count = parsed_count;

  }

  void RSPointClipper::ReadLasPointData(std::ifstream& file)
  {
    #pragma pack(push, 1)
    // LAS 1.2 format header structure
    struct LASHeader {
      char file_signature[4];
      uint16_t file_source_id;
      uint16_t global_encoding;
      uint32_t project_id_guid_data_1;
      uint16_t project_id_guid_data_2;
      uint16_t project_id_guid_data_3;
      uint8_t project_id_guid_data_4[8];
      uint8_t version_major;
      uint8_t version_minor;
      char system_identifier[32];
      char generating_software[32];
      uint16_t file_creation_day;
      uint16_t file_creation_year;
      uint16_t header_size;
      uint32_t offset_to_point_data;
      uint32_t number_of_variable_length_records;
      uint8_t point_data_format;
      uint16_t point_data_record_length;
      uint32_t number_of_point_records;
      uint32_t number_of_points_by_return[5];
      double x_scale_factor;
      double y_scale_factor;
      double z_scale_factor;
      double x_offset;
      double y_offset;
      double z_offset;
      double max_x;
      double min_x;
      double max_y;
      double min_y;
      double max_z;
      double min_z;
    };

    struct VLR_Header {
      uint16_t reserved;
      char user_id[16];
      uint16_t record_id;
      uint16_t record_length;
      char description[32];
    };
    #pragma pack(pop)

    // Read header
    LASHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(LASHeader));

  if (header.number_of_variable_length_records > 0) {
    file.seekg(header.header_size); 

    for (uint32_t i = 0; i < header.number_of_variable_length_records; ++i) {
      VLR_Header vlr_header;
      file.read(reinterpret_cast<char*>(&vlr_header), sizeof(VLR_Header));

      if (strncmp(vlr_header.user_id, "LASF_Projection", 15) == 0) {
        std::vector<char> crs_data(vlr_header.record_length);
        file.read(crs_data.data(), vlr_header.record_length);
        
        std::string crs_info(crs_data.data(), vlr_header.record_length);
        RS_DEBUG("Coordinate System: %s", crs_info.c_str());
      }
      else {
        // Skip other VLRs
        file.seekg(vlr_header.record_length, std::ios::cur);
      }
    }
  }
    // Verify LAS signature
    if (strncmp(header.file_signature, "LASF", 4) != 0) {
      throw std::runtime_error("Invalid LAS file signature.");
    }

    m_point_count = header.number_of_point_records;
    const int size_of_data = m_point_count * 6; // count * 6(point, color)
    m_data.resize(size_of_data);

    // Seek to point data
    file.seekg(header.offset_to_point_data);

    #pragma pack(push, 1)
    // Point Data Record Format 0, 1, 2, 3 structures
    struct LASPoint_Format0 {
      int32_t x;
      int32_t y;
      int32_t z;
      uint16_t intensity;
      uint8_t return_number : 3;
      uint8_t number_of_returns : 3;
      uint8_t scan_direction_flag : 1;
      uint8_t edge_of_flight_line : 1;
      uint8_t classification;
      int8_t scan_angle_rank;
      uint8_t user_data;
      uint16_t point_source_id;
    };

    struct LASPoint_Format1 : LASPoint_Format0 {
      double gps_time;
    };

    struct LASPoint_Format2 : LASPoint_Format0 {
      uint16_t red;
      uint16_t green;
      uint16_t blue;
    };

    struct LASPoint_Format3 : LASPoint_Format1 {
      uint16_t red;
      uint16_t green;
      uint16_t blue;
    };
    #pragma pack(pop)


    const size_t point_record_size = header.point_data_record_length;
    std::vector<char> buffer(m_point_count * point_record_size);
    file.read(buffer.data(), buffer.size());
    file.close();

    int index = 0;
    
    // Format 0: Base format - no RGB, no GPS
  if (header.point_data_format == 0) {
    for (uint32_t i = 0; i < m_point_count; ++i) {
      const char* point_data = buffer.data() + i * point_record_size;

      LASPoint_Format0 point;
      memcpy(&point, point_data, std::min(sizeof(LASPoint_Format0), point_record_size));

      // Convert scaled integer coordinates to float
      float x = static_cast<float>(point.x) * header.x_scale_factor;
      float y = static_cast<float>(point.y) * header.y_scale_factor;
      float z = static_cast<float>(point.z) * header.z_scale_factor;

      // Use intensity as grayscale color
      float intensity = static_cast<float>(point.intensity) / 65535.0f;

      m_data[index++] = static_cast<float>(x);
      m_data[index++] = static_cast<float>(y);
      m_data[index++] = static_cast<float>(z);
      m_data[index++] = intensity;
      m_data[index++] = intensity;
      m_data[index++] = intensity;
    }
  }
  // Format 1: Format 0 + GPS Time - no RGB
  else if (header.point_data_format == 1) {
    for (uint32_t i = 0; i < m_point_count; ++i) {
      const char* point_data = buffer.data() + i * point_record_size;

      LASPoint_Format1 point;
      memcpy(&point, point_data, std::min(sizeof(LASPoint_Format1), point_record_size));

      // Convert scaled integer coordinates to float
      float x = static_cast<float>(point.x) * header.x_scale_factor;
      float y = static_cast<float>(point.y) * header.y_scale_factor;
      float z = static_cast<float>(point.z) * header.z_scale_factor;

      // Use intensity as grayscale color
      float intensity = static_cast<float>(point.intensity) / 65535.0f;

      m_data[index++] = static_cast<float>(x);
      m_data[index++] = static_cast<float>(y);
      m_data[index++] = static_cast<float>(z);
      m_data[index++] = intensity;
      m_data[index++] = intensity;
      m_data[index++] = intensity;
    }
  }
  // Format 2: Format 0 + RGB - no GPS
  else if (header.point_data_format == 2) {
    for (uint32_t i = 0; i < m_point_count; ++i) {
      const char* point_data = buffer.data() + i * point_record_size;

      LASPoint_Format2 point;
      memcpy(&point, point_data, std::min(sizeof(LASPoint_Format2), point_record_size));

      // Convert scaled integer coordinates to float
      float x = static_cast<float>(point.x) * header.x_scale_factor;
      float y = static_cast<float>(point.y) * header.y_scale_factor;
      float z = static_cast<float>(point.z) * header.z_scale_factor;

      // Convert RGB from 16-bit to normalized float [0, 1]
      float r = static_cast<float>(point.red) / 65535.0f;
      float g = static_cast<float>(point.green) / 65535.0f;
      float b = static_cast<float>(point.blue) / 65535.0f;

      m_data[index++] = static_cast<float>(x);
      m_data[index++] = static_cast<float>(y);
      m_data[index++] = static_cast<float>(z);
      m_data[index++] = r;
      m_data[index++] = g;
      m_data[index++] = b;
    }
  }
  // Format 3: Format 1 + RGB (GPS Time + RGB)
  else if (header.point_data_format == 3) {
    for (uint32_t i = 0; i < m_point_count; ++i) {
      const char* point_data = buffer.data() + i * point_record_size;

      LASPoint_Format3 point;
      memcpy(&point, point_data, std::min(sizeof(LASPoint_Format3), point_record_size));

      // Convert scaled integer coordinates to float
      float x = static_cast<float>(point.x) * header.x_scale_factor;
      float y = static_cast<float>(point.y) * header.y_scale_factor;
      float z = static_cast<float>(point.z) * header.z_scale_factor;

      // Convert RGB from 16-bit to normalized float [0, 1]
      float r = static_cast<float>(point.red) / 65535.0f;
      float g = static_cast<float>(point.green) / 65535.0f;
      float b = static_cast<float>(point.blue) / 65535.0f;

      m_data[index++] = static_cast<float>(x);
      m_data[index++] = static_cast<float>(y);
      m_data[index++] = static_cast<float>(z);
      m_data[index++] = r;
      m_data[index++] = g;
      m_data[index++] = b;
    }
  }
    else {
      throw std::runtime_error("Unsupported LAS point data format.");
    }

    // Print point count
    RS_DEBUG("LAS Point Count: %d", m_point_count);

    // Print system identifier
    RS_DEBUG("System Identifier: %s", std::string(header.system_identifier, 32).c_str());

    // Print generating software
    RS_DEBUG("Generating Software: %s", std::string(header.generating_software, 32).c_str());

    // Print offsets
    RS_DEBUG("X Offset: %f", header.x_offset);
    RS_DEBUG("Y Offset: %f", header.y_offset);
    RS_DEBUG("Z Offset: %f", header.z_offset);

    file.close();
  }

  void RSPointClipper::CreateBufferObject()
  {
    // Create VAO
    glGenVertexArrays(1, &m_data_vao);
    glBindVertexArray(m_data_vao);

    // Create VBO
    glGenBuffers(1, &m_data_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_data_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_data.size() * sizeof(float), m_data.data(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);

  }

  void RSPointClipper::DeleteBufferObject() const
  {
    glDeleteBuffers(1, &m_data_vbo);
    glDeleteVertexArrays(1, &m_data_vao);
  }

  void RSPointClipper::ClearData()
  {
    m_data.clear();
    m_point_count = 0;
    clipping_id = 0;
    b_is_clipping = false;
    b_is_loaded = false;
    m_data_min = glm::vec3(FLT_MAX);
    m_data_max = glm::vec3(-FLT_MAX);
  }

  void RSPointClipper::CalculateMinMax()
  {
    if (m_data.empty()) {
      return;
    }

    // Pre-calculate loop bounds to avoid redundant computation in each iteration
    const int data_count = m_data.size();

    // Use pointers to avoid accessing the vector using indices repeatedly
    const float* data_ptr = m_data.data();

    glm::vec3 local_min(data_ptr[0], data_ptr[1], data_ptr[2]);
    glm::vec3 local_max(data_ptr[0], data_ptr[1], data_ptr[2]);

    for (int index = 0; index < data_count; index += 6) {
      glm::vec3 position(data_ptr[index], data_ptr[index + 1], data_ptr[index + 2]);

      local_min = glm::min(local_min, position);
      local_max = glm::max(local_max, position);
    }

    // Assign the final min and max values to the class variables
    m_data_min = local_min;
    m_data_max = local_max;
  }

  void RSPointClipper::ExportPointData(const std::wstring& file_path)
  {
    if (m_point_count == 0)
    {
      RS_WARN("No point data to export.");
      return;
    }
    // Open the file for writing
    std::ofstream file;
    file.open(file_path, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
      throw std::runtime_error("Failed to open the point cloud file.");
    }

    RS_Timer::RSDebugTimer::GetInstance()->Start();
    // Calculate clipped point count
    int32_t                    clipped_point_count = 0;
    const int                  data_count = m_point_count * 6;
    const auto                 m_resource_manager = RSResourceManager::GetInstance();
    RS_Component::RSTransform* transform = m_resource_manager->GetObjectManager()->GetObjectW("clipping_box")->
                                                               GetTransform();
    glm::vec3 box_position = transform->GetPosition();

    const glm::mat4 clipped_box_inverse_matrix                 = glm::inverse(transform->GetTransformMatrix());
    glm::mat4       clipped_box_inverse_matrix_without_scaling = glm::inverse(m_resource_manager->GetObjectManager()->
                                                                              GetObjectW("clipping_box")->GetTransform()
                                                                              ->
                                                                              GetTransformMatrixWithoutScale());
    float y_min_position = FLT_MAX;

    for (int index = 0; index < data_count; index += 6) {
      glm::vec3 position(m_data[index], m_data[index + 1], m_data[index + 2]);
      auto calculated_pos = glm::vec3(clipped_box_inverse_matrix * glm::vec4(position, 1.0f));
      
      /*
        if (abs(pos.x) > 0.5 || abs(pos.y) > 0.5 || abs(pos.z) > 0.5)
      */
      if (!(abs(calculated_pos.x) > 0.5 || abs(calculated_pos.y) > 0.5 || abs(calculated_pos.z) > 0.5)) {
        clipped_point_count++;
        y_min_position = glm::min(y_min_position,
                                  glm::vec3(clipped_box_inverse_matrix_without_scaling * glm::vec4(position, 1.0f)).y);
      }
    }


    // Write the clipped point count and the clipped points to the file
    file.write(reinterpret_cast<char*>(&clipped_point_count), sizeof(int32_t));

    // Get Clipping box inverse matrix without scaling. 


    for (int index = 0; index < data_count; index += 6) {
      glm::vec3 position(m_data[index], m_data[index + 1], m_data[index + 2]);
      glm::vec3 color(m_data[index + 3], m_data[index + 4], m_data[index + 5]);
      auto calculated_pos = glm::vec3(clipped_box_inverse_matrix * glm::vec4(position, 1.0f));

      if (!(abs(calculated_pos.x) > 0.5 || abs(calculated_pos.y) > 0.5 || abs(calculated_pos.z) > 0.5)) {
        position = glm::vec3(clipped_box_inverse_matrix_without_scaling * glm::vec4(position, 1.0f)) -
                   glm::vec3(box_position.x, y_min_position, box_position.z);
        file.write(reinterpret_cast<char*>(&position), sizeof(glm::vec3));
        file.write(reinterpret_cast<char*>(&color), sizeof(glm::vec3));
      }
    }

    file.close();

    RS_Timer::RSDebugTimer::GetInstance()->EndNPrint("Save Point Data time ");

  }

  void RSPointClipper::CreateBox() const
  {
    const auto m_resource_manager = RSResourceManager::GetInstance();

    const auto clipping_box = m_resource_manager->GetObjectManager()->CreateObject("clipping_box");

    const auto clipping_box_model = new RS_Component::RSModel();
    clipping_box_model->SetTransparency(0.1f);
    clipping_box->AddComponent(clipping_box_model);

    const auto clipping_box_mesh = new RS_Mesh::RSMesh();
    clipping_box_mesh->SetMeshData(m_resource_manager->GetMeshManager()->GetMesh("Cube"));

    clipping_box_model->SetMesh(clipping_box_mesh);

    // Calculate position and scale from min and max values
    const glm::vec3 box_position = (m_data_min + m_data_max) * 0.5f;
    const glm::vec3 box_scale = m_data_max - m_data_min;

    clipping_box->GetTransform()->SetPosition(box_position);
    clipping_box->GetTransform()->SetScale(box_scale);
  }

  void RSPointClipper::SetCameraPosition()
  {
    const auto m_resource_manager = RSResourceManager::GetInstance();
    const auto camera = m_resource_manager->GetCamera();

    // Get clipping box center and size
    const auto clipping_box = m_resource_manager->GetObjectManager()->GetObjectW("clipping_box");


    const glm::vec3 center_position = clipping_box->GetTransform()->GetPosition();
    const glm::vec3 extents = clipping_box->GetTransform()->GetScale() * 0.5f;
    const float extent_length = glm::length(extents);
    
    // Position the camera
    glm::vec3 camera_position = center_position + glm::vec3(0.0f, 0.0f, extent_length * 2.0f);
    camera->SetPosition(camera_position);

    // Make the camera look at the center of the point cloud
    camera->SetFocalPoint(center_position);

	// Update camera's matrix
	camera->UpdateCameraVectors();
  }

  void RSPointClipper::SetClippingColor(const glm::vec3 color_data)
  {
    m_clip_color = color_data;
  }

  void RSPointClipper::ResetData()
  {
    ClearData();
    DeleteBufferObject();
  }


}
