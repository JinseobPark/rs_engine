/******************************************************************************/
/*!
\file   RSShaderManager.h
\author Jinseob Park
\date   2024/08/01

This file contains Shader Manager for Shader Managing.

*/
/******************************************************************************/
#ifndef RS_SHADER_MANAGER_H_
#define RS_SHADER_MANAGER_H_

#include "../Pipeline/RSShader.h"
#include "Define/RSPipelineList.h"
#include <map>
#include <string>

/**
* @brief Shader namespace
*/
namespace RS_Shader
{
using RS_PipelineList::RSShaderNames;
using RS_PipelineList::RSComputeShaderNames;

/**
* @brief Shader manager
*/
class RSShaderManager
{
public:
	/**
	 * @brief Constructor
	 */
	RSShaderManager() = default;

	/**
	 * @brief Destructor
	 */
	~RSShaderManager();


	/**
	 * @brief Link shaders and compute shaders to shader manager. Add to shader map
	 */
	void Initialize();

	/**
	 * @brief Delete all shaders and compute shaders from shader map
	 */
	void Shutdown();


	/**
	 * @brief Compile all shaders and compute shaders
	 */
	void Compile() const;

	/**
	* @brief Get shader from shader map by name
	* @param name_ shader name
	* @return shader pointer. If not found, return nullptr
	*/
	RSShader* GetShader(const RSShaderNames name_) { return shaders_map_[name_]; }

	/**
	* @brief Get compute shader from compute shader map by name
	* @param name_ compute shader name
	* @return compute shader pointer. If not found, return nullptr
	*/
	ComputeShader* GetShader(const RSComputeShaderNames name_) { return comp_shaders_map_[name_]; }

	/**
	* @brief Use the shader by name
	* @param name_ shader name
	*/
	void Use(RSShaderNames name_);

	/**
	* @brief Use the compute shader by name
	* @param name_ compute shader name
	*/
	void Use(RS_PipelineList::RSComputeShaderNames name_);


  /**
   * @brief Unbind shader
	 */
	void UnbindShader();

	/**
	* @brief Set data to shader matrix 4x4
	* @param[in] shader_name_ shader name
	* @param[in] value_name uniform name
	* @param[in] data matrix 4x4
	*/
	void SetData(RSShaderNames shader_name_, const std::string& value_name, const glm::mat4& data);

	/**
	* @brief Set data to shader vector 3
	* @param[in] shader_name_ shader name
	* @param[in] value_name uniform name
	* @param[in] data vector 3
	*/
	void SetData(RSShaderNames shader_name_, const std::string& value_name, glm::vec3 data);

	/**
	 * @brief Set data to shader int vector 3
	 * @param[in] shader_name_ shader name
	 * @param[in] value_name uniform name
	 * @param[in] data int vector 3
	 */
	void SetData(RSShaderNames shader_name_, const std::string& value_name, glm::ivec3 data);

	/**
	 * @brief Set data to shader vector 4
	 * @param[in] shader_name_ shader name
	 * @param[in] value_name uniform name
	 * @param[in] data vector 4
	 */
	void SetData(RSShaderNames shader_name_, const std::string& value_name, glm::vec4 data);

	/**
	 * @brief Set data to shader int vector 4
	 * @param[in] shader_name_ shader name
	 * @param[in] value_name uniform name
	 * @param[in] data int vector 4
	 */
	void SetData(RSShaderNames shader_name_, const std::string& value_name, glm::ivec4 data);

  /**
   * @brief Set data to shader float vector 2
	 * @param shader_name_ shader name
	 * @param value_name  uniform name
	 * @param data float vector 2
	 */
	void SetData(RSShaderNames shader_name_, const std::string& value_name, glm::vec2 data);

	/**
	 * @brief Set data to shader float
	 * @param[in] shader_name_ shader name
	 * @param[in] value_name uniform name
	 * @param[in] data float
	 */
	void SetData(RSShaderNames shader_name_, const std::string& value_name, glm::float32 data);

	/**
	 * @brief Set data to shader int
	 * @param[in] shader_name_ shader name
	 * @param[in] value_name uniform name
	 * @param[in] data int
	 */
	void SetData(RSShaderNames shader_name_, const std::string& value_name, int data);

	/**
	 * @brief Set data to shader unsigned int
	 * @param[in] shader_name_ shader name
	 * @param[in] value_name uniform name
	 * @param[in] data unsigned int
	 */
	void SetData(RSShaderNames shader_name_, const std::string& value_name, unsigned int data);


	/**
	 * @brief Set data to shader unsigned int
	 * @param[in] shader_name_ shader name
	 * @param[in] value_name uniform name
	 * @param[in] data unsigned bool
	 */
	void SetData(RSShaderNames shader_name_, const std::string& value_name, bool data);

	/**
	 * @brief Set texture to shader
	 * @param[in] shader_name_ shader name
	 * @param[in] value_name uniform name
	 * @param[in] data texture id
	 * @param[in] index texture index
	 */
	void SetTexture(RSShaderNames shader_name_, const std::string& value_name, unsigned int data, unsigned int index);

	/**
	 * @brief Set data to compute shader matrix 4x4
	 * @param[in] shader_name_ compute shader name
	 * @param[in] value_name uniform name
	 * @param[in] data matrix 4x4
	 */
	void SetData(RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name, const glm::mat4& data);

	/**
	 * @brief Set data to compute shader vector 3
	 * @param[in] shader_name_ compute shader name
	 * @param[in] value_name uniform name
	 * @param[in] data vector 3
	 */
	void SetData(RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name, glm::vec3 data);

	/**
	 * @brief Set data to compute shader int vector 3
	 * @param[in] shader_name_ compute shader name
	 * @param[in] value_name uniform name
	 * @param[in] data int vector 3
	 */
	void SetData(RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name, glm::ivec3 data);

	/**
	 * @brief Set data to compute shader vector 4
	 * @param[in] shader_name_ compute shader name
	 * @param[in] value_name uniform name
	 * @param[in] data vector 4
	 */
	void SetData(RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name, glm::vec4 data);

	/**
	 * @brief Set data to compute shader int vector 4
	 * @param[in] shader_name_ compute shader name
	 * @param[in] value_name uniform name
	 * @param[in] data int vector 4
	 */
	void SetData(RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name, glm::ivec4 data);

	/**
	 * @brief Set data to compute shader float
	 * @param[in] shader_name_ compute shader name
	 * @param[in] value_name uniform name
	 * @param[in] data float
	 */
	void SetData(RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name, glm::float32 data);

	/**
	 * @brief Set data to compute shader int
	 * @param[in] shader_name_ compute shader name
	 * @param[in] value_name uniform name
	 * @param[in] data int
	 */
	void SetData(RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name, int data);

	/**
	 * @brief Set data to compute shader unsigned int
	 * @param[in] shader_name_ shader name
	 * @param[in] value_name uniform name
	 * @param[in] data unsigned int
	 */
	void SetData(RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name, unsigned int data);

	/**
	 * @brief Set data to compute shader unsigned int
	 * @param[in] shader_name_ shader name
	 * @param[in] value_name uniform name
	 * @param[in] data bool
	 */
	void SetData(RS_PipelineList::RSComputeShaderNames shader_name_, const std::string& value_name,const bool data);

	/**
	 * @brief Get shader id by name (enum)
	 * @param shader_name_ shader name
	 * @return shader id
	 */
	unsigned int GetID(const RSShaderNames shader_name_) { return shaders_map_[shader_name_]->GetID(); }

	/**
	 * @brief Get compute shader id by name (enum)
	 * @param shader_name_ compute shader name
	 * @return compute shader id
	 */
	unsigned int GetID(const RSComputeShaderNames shader_name_) { return comp_shaders_map_[shader_name_]->GetID(); }

	/**
	 * @brief Get shader map of all shaders
	 * @return shader map (map)
	 */
	std::map<RSShaderNames, RSShader*>* GetShaders() { return &shaders_map_; }

	/**
	 * @brief Get compute shader map of all compute shaders
	 * @return compute shader map (map)
	 */
	std::map<RSComputeShaderNames, ComputeShader*>* GetComputeShaders() { return &comp_shaders_map_; }
private:
  std::map<RSShaderNames, RSShader*> shaders_map_; ///< Shader map
  std::map<RSComputeShaderNames, ComputeShader*> comp_shaders_map_; ///< Compute shader map

};
}

#endif // !RS_SHADER_MANAGER_H_