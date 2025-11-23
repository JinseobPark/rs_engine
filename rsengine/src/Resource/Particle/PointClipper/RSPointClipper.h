/**********************************************************************
 * \file   RSPointClipper.h
 * \brief  Point Clip Renderer
 * 
 * \author Jinseob Park
 * \date   06/11/2024
 *********************************************************************/
#ifndef RS_POINT_CLIPPER_H_
#define RS_POINT_CLIPPER_H_

#include "glm/glm.hpp"
#include <GL/glew.h>


 /**
  * @brief GUI Handler namespace
  */
namespace RS_Handler
{
  class RSImguiHandler;
}

/**
 * @brief Point Clipper namespace
 */
namespace RS_PointClipper
{
  class RSPointClipper
  {
  public:
    RSPointClipper();
    ~RSPointClipper();


    /**
     * @brief Initialize Point Clipper
     */
    void Initialize();

    /**
     * @brief Shutdown Point Clipper
     */
    void ShutDown();

    /**
     * @brief Update Point Clipper
     * @param dt delta_time
     */
    void Update(float dt);


    /**
     * @brief Draw Point Clipper
     */
    void Draw();


    /**
     * @brief Load Point Data
     * @param file_path file path
     */
    void LoadPointData(const std::string& file_path);


    /**
     * @brief Read Point Data
     * @param file file to read
     */
    void ReadBinPointData(std::ifstream& file);

    /**
     * @brief Read PLY Point Data
     * @param file file to read
     */
    void ReadPlyPointData(std::ifstream& file);


    /**
     * @brief Create Buffer Object
     */
    void CreateBufferObject();


    /**
     * @brief Delete Buffer Object
     */
    void DeleteBufferObject() const;


    /**
     * @brief Clear Data
     */
    void ClearData();

    /**
     * @brief Calculate Min Max
     */
    void CalculateMinMax();


    /**
     * @brief Export Point Data
     * @param file_path file path
     */
    void ExportPointData(const std::wstring& file_path);


    /**
     * @brief Get boolean that point clipper loaded
     * @return is loaded
     */
    bool isLoaded() const { return b_is_loaded; }


    /**
     * @brief Get Point Count
     * @return point count
     */
    int GetPointCount() const { return m_point_count; }


    /**
     * @brief Create box object
     */
    void CreateBox() const;


    /**
     * @brief Set boolean that point clipper is clipping
     * @param is_clipping is clipping
     */
    void SetIsClipping(const bool is_clipping) { b_is_clipping = is_clipping; }

    /**
     * @brief Get boolean that point clipper is clipping
     */
    void SwitchClipping() { b_is_clipping = !b_is_clipping; }

    /**
     * @brief Set color data for clipping
     * @param color_data color data
     */
    void SetClippingColor(glm::vec3 color_data);


    /**
     * @brief Reset point clipper data
     */
    void ResetData();

  private:
    int m_point_count = 0; ///< Point count

    std::vector<float> m_data; ///< Point data

    GLuint m_data_vao = 0; ///< Vertex Array Object
    GLuint m_data_vbo = 0; ///< Vertex Buffer Object

    std::string m_point_cloud_file_path; ///< Point cloud file path
    std::string m_point_cloud_file_name; ///< Point cloud file name

    float m_point_size = 10.0f; ///< Point size

    bool b_is_loaded = false; ///< Is loaded

    bool b_is_clipping = false; ///< Is clipping

    int clipping_id = 0; ///< Clipping ID
    glm::vec3 m_clip_color = glm::vec3(0.0f); ///< Clipping color

    glm::vec3 m_data_min = glm::vec3(FLT_MAX); ///< Min data
    glm::vec3 m_data_max = glm::vec3(FLT_MIN); ///< Max data

    friend class RS_Handler::RSImguiHandler; ///< Friend class


  };
}


#endif // !RS_POINT_CLIPPER_H_
