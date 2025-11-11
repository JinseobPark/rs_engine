/*********************************************************************************************************************/
/*!
\file   RSVTKViewer.h
\author Jinseob Park
\date   2025/02/07
\version 0.0.5

This file contains VTK Viewer for rendering particle system.

*/
/*********************************************************************************************************************/
#ifndef RS_VTK_VIEWER_H_
#define RS_VTK_VIEWER_H_

#include "glm/glm.hpp"
#include <GL/glew.h>
#include "Resource/RSGraphicsData.h"
#include <variant>

/**
 * @brief GUI Handler namespace
 */
namespace RS_Handler
{
  class RSImguiHandler;
}

/**
 * @brief VTK Loader namespace
 */
namespace RS_VTK
{

  /**
   * @brief Attribute Data Type
   */
  enum class AttributeDataType : uint8_t
  {
    NONE = 0,
    SINGLE_FLOAT,
    THREE_FLOAT,
    SINGLE_UNSIGNED_INT,
    SINGLE_UNSIGNED_CHAR,
  };

  /**
   * @brief Clipping Type
   */
  enum class ClippingType : int
  {
    NONE = 0,
    CUBE = 1,
    SPHERE = 2,
    PLANE = 3
  };

  /**
   * @brief This struct is for field data attribute include offset, size, type, name
   */
  struct field_data_attribute
  {
    // line example : solid 1 85400 unsigned_char
    std::streampos data_offset; // ex. 1024414. // start binary data.
    unsigned int data_size = 0; // ex. 85400
    //unsigned int file_offset; // ex. 1024414
    AttributeDataType attribute_data_type = AttributeDataType::NONE;
    bool is_single_byte = false;  // ex. true = 4 byte (1 float), false = 12 byte (3 float)
    std::string name;             // ex. gas, solid
    std::string type;             // ex. float, unsigned char
  };

  /**
   * @brief This struct is for point data attribute
   */
  struct point_data_attribute
  {
    std::string name;
    std::streampos data_offset; // ex. 1024414. // start binary data.
  };

  /**
   * @brief This struct is for vtk file header data
   * @details It includes the data_id and field_data
   */
  struct vtk_file_header_data
  {
    unsigned int data_id = 0;
    unsigned int point_count = 0;
    point_data_attribute point_data;
    std::vector<field_data_attribute> field_data;
  };


  /**
   * @brief This class is for VTK Viewer
   */
  class RSVTKViewer
  {
  public:
    RSVTKViewer();
    ~RSVTKViewer();

    void Initialize();
    void ShutDown();

    void Update(float dt);

    void Draw(RSDataRenderType render_type_ = RSDataRenderType::FORWARD);


    void Reset();

    //************************************************************
    //************ File IO ***************************************
    //************************************************************

    /**
     * @brief This function load the vtk file from the file path or drag and drop
     * This function should be called when new vtk file is loaded.
     * Don't call this function when the successive vtk file is loaded.
     * @param file_path 'new' vtk file path
     */
    void LoadVtkFile(const std::string& file_path);

    /**
     * @brief This function update the vtk file by a successive controlled by the user.
     * This function should be called when the successive vtk file is loaded.
     * Don't call this function when new vtk file is loaded.
     * @param vtk_id vtk file index.
     */
    void UpdateVtkFile(const unsigned int vtk_id);

    /**
     * @brief Check if the file exists on the file path
     * @param file_path file path
     * @return true if the file exists, otherwise false
     */
    bool IsFileExist(const std::string& file_path);

    /**
     * @brief Load the next vtk file by the user control
     * Finding next file by the vtk index id, not the file name.
     */
    void LoadNextVtkFile();

    /**
    * @brief Load the previous vtk file by the user control
    * Finding previous file by the vtk index id, not the file name.
    */
    void LoadPrevVtkFile();

    /**
    * @brief Load the first vtk file by the user control
    * Finding the first file by the vtk index id, not the file name.
    */
    void LoadFirstVtkFile();

    /**
     * @brief Check if the vtk file is loaded
     * @return true if the vtk file is loaded, otherwise false
     */
    bool IsLoaded() const { return b_is_loaded; }

    /**
     * @brief Change the attribute index
     * This function include change the attribute index and change the buffer object
     */
    void ChangeAttributeIndex(unsigned int index_);

    /**
    * @brief Check if the directory path of the file is same with the previous one
    * @param file_path file path
    * @return true if the directory path of the file path is same with the previous one, otherwise false
    */
    bool CheckIsSamePrevDir(const std::string& file_path) const;

    /**
     * @brief Set the vtk file id by the sorted file id vector
     * @param sorted_file_id_vec sorted file id vector
     * @param id_ vtk file id
     */
    void SetVtkIdByIdMap(const std::vector<unsigned int>* sorted_file_id_vec, const unsigned int id_);

    /**
    * @brief Set the vtk file id by the file path
    * @param file_path file path
    */
    void SetVtkIdByFilepath(const std::string& file_path);



    //************************************************************
    //************ Data parsing **********************************
    //************************************************************

    /**
     * @brief Enroll the data size map to the unordered map
     * It is used for getting the data size by the type
     * ex. float = 4, unsigned char = 1
     */
    void EnrollDataSizeMap();


    /**
     * @brief Read the vtk file by the file path.
     * It was substituted by the ReadFile(const unsigned int vtk_id). not use this function
     * @param file_path file path
     * @return true if the file is read, otherwise false
     */
    //bool ReadFile(const std::string& file_path);

    /**
     * @brief Read the vtk file by the vtk id
     * @param vtk_id vtk file id from the index of the file list
     * @return true if the file is read, otherwise false
     */
    bool ReadFile(const unsigned int vtk_id);


    /**
     * @brief Parsing the data from the file
     * @param file file stream
     * @return true if the data is parsed, otherwise false
     */
    bool ReadDataWithAttribute(std::ifstream& file);

    /**
     * @brief Parsing the data from the directory
     * @param file_path file path
     */
    void ParsingDataFromDir(const std::string& file_path);

    /**
     * @brief Create the attribute map by the sorted file id vector
     * @param sorted_file_id_vec sorted file id vector
     */
    void CreateAttributeMap(const std::vector<unsigned int>* sorted_file_id_vec);

    /**
     * @brief Create Attribute Name Map to m_attribute_map vector
     */
    void CreateAttributeNameMap();

    /**
     * @brief Set the field data from the file path.
     * It assumed the file is a sph result file.
     * @param file_path file path
     * @param vtk_header_data_ vtk file header data struct
     */
    void SetFieldDataFromFile(const std::string& file_path, vtk_file_header_data& vtk_header_data_) const;

    /**
     * @brief Get the data size by the type
     * ex. "float" = 4, "unsigned char" = 1
     * @param type data type string
     * @return data size
     */
    unsigned int GetDataSizeMap(const std::string& type) const;

    /**
     * @brief Get the max particle count on the directory files
     * @return max particle count
     */
    unsigned int GetMaxParticleCountOnDir() const;

    /**
     * @brief Set the attribute data to the first of the vtk header data list
     */
    void SetAttributeDataFirstVtkData();

    /**
     * @brief Create the origin file name by the number
     * @param num_ vtk file number
     * @return the file's full name
     */
    std::string CreateOriginFileNameByNum(unsigned int num_) const;



    //************************************************************
    //************  OGL Buffer Object ****************************
    //************************************************************

    /**
     * @brief Create the buffer object when the vtk file is loaded
     * the buffer object include the position and data buffer
     * the buffer size is determined by the maximum particle count and the first attribute data
     */
    void CreateBufferObjectByDir();

    /**
     * @brief Change the buffer object when file loaded or attribute index changed 
     */
    void ChangeBufferObject();

    /**
     * @brief Deallocate the buffer object when the vtk file is unloaded or reset the data
     */
    void DeleteBufferObject();

    
    /**
     * @brief Clear the all list data when the vtk file is unloaded or reset the data
     */
    void ClearData();

    //************************************************************
    //*****************Draw***************************************
    //************************************************************

    /**
     * @brief Draw the vtk file by the three float data(vec3 data)
     */
    void SetShaderUniformThreeFloatVtk(RSDataRenderType render_type_ = RSDataRenderType::FORWARD);

    /**
     * @brief Draw the vtk file by the single float data(float data)
     */
    void SetShaderUniformSingleFloatVtk(RSDataRenderType render_type_ = RSDataRenderType::FORWARD);

    /**
     * @brief Draw the vtk file by the single unsigned int data(uint or unsigned char data)
     */
    void SetShaderUniformSingleUintVtk(RSDataRenderType render_type_ = RSDataRenderType::FORWARD);

    void DrawPoint();

    //************************************************************
    //***************** Data Access ******************************
    //************************************************************

    void ResetMinMaxValue()
    {
      m_min = FLT_MAX;
      m_max = FLT_MIN;
    }

    /**
     * @brief Get float min value from the vtk data
     * @return min value (float)
     */
    float GetMin() const { return m_min; }

    /**
     * @brief Get float max value from the vtk data
     * @return max value (float)
     */
    float GetMax() const { return m_max; }

    /**
     * @brief Get vec3 min value from the vtk data
     * @return min value (vec3)
     */
    glm::vec3 GetMinVec3() const { return m_min_vec3; }

    /**
     * @brief Get vec3 max value from the vtk data
     * @return max value (vec3)
     */
    glm::vec3 GetMaxVec3() const { return m_max_vec3; }


    /**
     * @brief Get the attribute data from the vtk file
     * @return field_data_attribute struct
     */
    field_data_attribute GetAttributeData() const
    {
      return m_vtk_header_data_list.at(m_vtk_file_id).field_data.at(m_cur_attribute_index);
    }

    /**
     * @brief Set the next attribute data 
     */
    void SetNextAttributeData();

    /**
     * @brief Set the previous attribute data
     */
    void SetPrevAttributeData();

    /**
     * @brief Get the max attribute index
     * @return max attribute index
     */
    unsigned int GetMaxAttributeIndex() const { return m_max_attribute_index; }

    /**
     * @brief Get the current attribute index
     * @return current attribute index
     */
    unsigned int GetCurAttributeIndex() const { return m_cur_attribute_index; }

    /**
     * @brief Get the particle count
     * @return particle count
     */
    size_t GetParticleCount() const { return m_particle_count; }

    /**
     * @brief Get maximum file id of the file name
     * @return maximum file id
     */
    unsigned int GetMaxFileId() const { return m_max_file_id; }

    /**
     * @brief Get the current file id of the file name
     * @return current file id
     */
    unsigned int GetVtkFileId() const { return m_vtk_file_id; }

    /**
     * @brief Set current attribute index
     * @param index attribute index
     */
    void SetAttributeIndex(const unsigned int index) { m_cur_attribute_index = index; }

    /**
     * @brief Get the attribute names to the vector
     * @return attribute names vector
     */
    const std::vector<std::string>* GetAttributeNames() const;

    /**
     * @brief Set point size for visualization
     * @param point_size point size
     * @see SetClippingChanged()
     */
    void SetPointSize(const float point_size);

    /**
     * @brief Get point size for visualization
     * @return point size
     */
    float GetPointSize() const { return m_point_size; }

    /**
     * @brief Get min value for legend and filtering
     * @return min value 
     */
    float GetMinValueLegend() const { return m_min_legend; }

    /**
     * @brief Get max value for legend and filtering
     * @return max value
     */
    float GetMaxValueLegend() const { return m_max_legend; }

    /**
     * @brief Get min value for legend and filtering
     * @return min value
     */
    glm::vec3 GetMinValueLegendVec3() const { return m_min_range_vec3; }

    /**
     * @brief Get max value for legend and filtering
     * @return max value
     */
    glm::vec3 GetMaxValueLegendVec3() const { return m_max_range_vec3; }

    /**
     * @brief Set min value for legend and filtering
     * @param value_ min value
     * @see SetClippingChanged()
     */
    void SetMinValueLegend(const float value_);

    /**
     * @brief Set max value for legend and filtering
     * @param value_ max value
     * @see SetClippingChanged()
     */
    void SetMaxValueLegend(const float value_);

    /**
     * @brief Set min value for legend and filtering
     * @param value_ min value
     * @see SetClippingChanged()
     */
    void SetMinValueLegendVec3(const glm::vec3& value_);

    /**
     * @brief Set max value for legend and filtering
     * @param value_ max value
     * @see SetClippingChanged()
     */
    void SetMaxValueLegendVec3(const glm::vec3& value_);

    /**
     * @brief Update the min and max value for legend and filtering
     * @see SetClippingChanged()
     */
    void AsyncMinMaxValueLegend();
    

    /**
     * @brief Get the legend texture
     * @param texture_ texture id
     */
    void SetLegendTextureId(unsigned int texture_);

    unsigned int GetLegendTextureId() const { return m_target_legend_texture; }

    // ************************************************************
    // *************** Clipping and Filtering *********************
    // ************************************************************

    /**
     * @brief Set the clipping type
     * @param type clipping type
     * @see SetClippingChanged()
     */
    void SetClippingType(const ClippingType type);

    /**
     * @brief Get the clipping type
     * @return clipping type
     */
    ClippingType GetClippingType() const { return m_clipping_type; }

    /**
     * @brief Set the clipping invert or not
     * @param invert is invert clipping
     * @see SetClippingChanged()
     */
    void SetClippingInvert(const bool invert);

    /**
     * @brief Get the clipping invert or not
     * @return is invert clipping
     */
    bool GetClippingInvert() const { return b_invert_clipping; }

    /**
     * @brief Set the clipping changed or not
     * @param changed is clipping changed
     */
    void SetClippingChanged(const bool changed) { b_clipping_changed = changed; }

    /**
     * @brief Get the clipping changed or not
     * @return is clipping changed
     */
    bool GetClippingChanged() const { return b_clipping_changed; }

    /**
     * @brief Set the legend filtering or not for the visualization
     * @param filtering is legend filtering
     * @see SetClippingChanged()
     */
    void SetLegendFiltering(const bool filtering);

    /**
     * @brief Get the legend filtering or not for the visualization
     * @return is legend filtering
     */
    bool GetLegendFiltering() const { return b_legend_filtering; }

    /**
     * @brief Set the clipping type by the integer (0 = NONE, 1 = CUBE, 2 = SPHERE, 3 = PLANE)
     * @param clipping_type_ clipping type integer
     */
    void SetClippingType(const int clipping_type_);

    /*
     * @brief Switch the clipping object.
     * @see RSVTKViewer::RemoveClippingObjects()
     * @see RSVTKViewer::CreateClippingObjects()
     * This function include removing the previous clipping object and creating the new clipping object.
     */
    void SwitchClippingObject();

    /**
     * @brief Remove the clipping object
     */
    void RemoveClippingObjects();

    /**
     * @brief Create the clipping object by the clipping type
     */
    void CreateClippingObjects();

    /**
     * @brief Update the clipping object transform matrix
     */
    void UpdateClippingData();


  private:
    size_t m_particle_count = 0;  // @brief current particle count
    size_t m_prev_particle_count = 0; // @brief previous particle count

    GLuint m_vtk_point_vao = 0; // @brief vtk point vertex array buffer object
    GLuint m_position_vbo = 0;  // @brief position vertex buffer object
    GLuint m_data_vbo = 0;      // @brief data vertex buffer object

    
    unsigned int m_max_file_id = 0; // @brief Not vtk file number. It is max index of the file list.
    unsigned int m_vtk_file_id = 0; // @brief Not vtk file number. It is the cur index of the file list.

    bool b_is_loaded = false; // @brief is vtk file loaded
    bool b_is_sph = false;    // @brief is sph file
    bool b_binary = true; // @brief is binary file


    std::vector<glm::vec3> m_positions; // @brief position data

    // @brief data buffer.
    // @details It allocated by (the data size * particle count * data type size)
    void* m_data_buffer = nullptr;


    // @todo Consider to create to pointer 
    std::vector<field_data_attribute> m_attribute_data; // @brief attribute data vector
    std::vector<vtk_file_header_data> m_vtk_header_data_list; // @brief vtk file header data list

    std::string m_vtk_name; // @brief vtk file name include the number and '_'
    std::string m_vtk_main_file_name; // @brief vtk file name exclude the number and '_'

    std::string m_vtk_file_path_origin; // @brief vtk file path origin for updating attribute data
    std::string m_vtk_file_dir; // @brief vtk file directory path
    std::unordered_map<std::string, unsigned int> m_data_size_map; // @brief data size map
    std::vector<std::string> m_attribute_name; // @brief attribute name vector

    float m_min = FLT_MAX; // @brief min value for float data
    float m_max = FLT_MIN; // @brief max value for float data

    glm::vec3 m_min_vec3 = glm::vec3(FLT_MAX); // @brief min value for vec3 data
    glm::vec3 m_max_vec3 = glm::vec3(FLT_MIN); // @brief max value for vec3 data

  protected:
    // ************************************************************
    // ******************* User Control ***************************
    // ************************************************************

    float m_point_size = 50.0f; // @brief point size for visualization

    GLuint m_target_legend_texture = 0; // @brief target legend texture

    //point_data_attribute m_point_data; // @brief point data attribute. It was included by the vtk_file_header_data

    unsigned int m_cur_attribute_index = 0; // @brief current attribute index
    unsigned int m_max_attribute_index = 0; // @brief max attribute index

    float m_min_legend = FLT_MAX; // @brief min value for legend and filtering.
    float m_max_legend = FLT_MIN; // @brief max value for legend and filtering.

    glm::vec3 m_min_range_vec3 = glm::vec3(FLT_MAX); // @brief min value for legend and filtering.
    glm::vec3 m_max_range_vec3 = glm::vec3(FLT_MIN);  // @brief max value for legend and filtering.


    // @brief clipping type
    // @details NONE = 0, CUBE = 1, SPHERE = 2, PLANE = 3
    ClippingType m_clipping_type = ClippingType::NONE;

    bool b_invert_clipping = false; // @brief is invert clipping

    glm::mat4 m_object_transform = glm::mat4(1.0f); // @brief object transform matrix for clipping

    bool b_clipping_changed = true; // @brief is clipping changed

    bool                          b_legend_filtering = false; // @brief is legend filtering

    
  private:
    friend class RS_Handler::RSImguiHandler; // @brief friend class
  };
}

#endif



  