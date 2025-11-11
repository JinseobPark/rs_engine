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

    m_point_cloud_file_path = file_path;
    m_point_cloud_file_name = file_path.substr(file_path.find_last_of("/\\") + 1);

    // Read the point cloud data from the file by binary read.
    std::ifstream file(file_path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to open the point cloud file.");
    }

    ReadPointData(file);

    CreateBufferObject();
    CalculateMinMax();
    CreateBox();

    b_is_loaded = true;
    RS_PRINT("Point cloud data loaded successfully Debug.");

    RS_Timer::RSDebugTimer::GetInstance()->EndNPrint("Load Point Data time ");

  }

  void RSPointClipper::ReadPointData(std::ifstream& file)
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
    const int data_count = m_point_count * 6;

    // Use pointers to avoid accessing the vector using indices repeatedly
    const float* data_ptr = m_data.data();

    glm::vec3 local_min = m_data_min;
    glm::vec3 local_max = m_data_max;

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
