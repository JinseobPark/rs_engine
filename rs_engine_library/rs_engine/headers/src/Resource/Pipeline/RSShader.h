/******************************************************************************/
/*!
\file   RSShader.h
\author Jinseob Park
\date   2024/08/01

This file contains shader class for rendering, 
    which includes vertex, fragment, and geometry shader.

*/
/******************************************************************************/
#ifndef RS_SHADER_H_
#define RS_SHADER_H_

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GL/glew.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstring>

/**
 * @brief Shader namespace
 */
namespace RS_Shader
{
    /**
     * @brief Shader Class.
     */
    class RSShader
    {
    private:
      unsigned int ID = 0; ///< ID of the shader.
      bool b_from_path = true; ///< If it is true, it will load the shader from the given path. If it is false, it will load the shader from the given string.

      char* vertex_path_; ///< Vertex shader path.
      char* frag_path_; ///< Fragment shader path.
      char* geo_path_; ///< Geometry shader path.
    public:
        /**
		 * @brief RS Shader Constructor.
         * @param vertex_path vertex shader path.
		 * @param fragment_path fragment shader path.
     * @param is_path_ If it is true, it will load the shader from the given path. 
     *                 If it is false, it will load the shader from the given string.
		 * @param geometry_path geometry shader path. If it is nullptr, it will be ignored.
         */
        explicit RSShader(const char* vertex_path /*= "resources\\GLSL\\default.vert"*/, const char* fragment_path /*= "resources\\GLSL\\default.frag"*/, const bool is_path_ = true, const char* geometry_path = nullptr);
        //RSShader(const int vertex_id, const int fragment_id);

        ~RSShader();

        /**
		 * @brief Initialize Shader.
		 * This code moved to constructor. You should not use this function.
		 * @param vertex_path vertex shader path.
		 * @param fragment_path fragment shader path.
         */
        void Initialize(const char* vertex_path = "resources\\GLSL\\default.vert", const char* fragment_path = "resources\\GLSL\\default.frag");

        /**
		 * @brief Compile the shader from the given path.
         */
        void CompileShader();

        /**
         * @brief Compile the shader from the given path.
         * @param vertex_path vertex shader apth
		 * @param fragment_path fragment shader path
         */
        void CompileShaderFromFile(const char* vertex_path, const char* fragment_path);

        /**
		 * @brief Switch the shader to use.
         */
        void Use() const;

        /**
		 * @brief Set the mat4 value to the shader.
		 * @param name uniform name.
		 * @param mat mat4 value.
         */
        void SetMat4(const std::string& name, const glm::mat4& mat) const;

        /**
		 * @brief Set the vec3 value to the shader.
		 * @param name uniform name.
		 * @param vec vec3 value.
         */
        void SetVec3(const std::string& name, const glm::vec3 vec) const;

        /**
		 * @brief Set the ivec3 value to the shader.
		 * @param name uniform name.
		 * @param vec ivec3 value.
         */
        void SetIVec3(const std::string& name, const glm::ivec3 vec) const;

        /**
		 * @brief Set the vec4 value to the shader.
		 * @param name uniform name.
		 * @param vec vec4 value.
         */
        void SetVec4(const std::string& name, const glm::vec4 vec) const;

        /**
		 * @brief Set the ivec4 value to the shader.
		 * @param name uniform name.
		 * @param vec ivec4 value.
         */
        void SetIVec4(const std::string& name, const glm::ivec4 vec) const;

      /**
         * @brief Set the vec2 value to the shader.
         * @param name uniform name.
         * @param vec vec2 value.
         */
        void SetVec2(const std::string& name, const glm::vec2 vec) const;

        /**
		 * @brief Set the bool value to the shader.
		 * @param name uniform name.
		 * @param value bool value.
         */
        void SetBool(const std::string& name, bool value) const;

        /**
		 * @brief Set the int value to the shader.
		 * @param name uniform name.
		 * @param value int value.
         */
        void SetInt(const std::string& name, int value) const;

        /**
		 * @brief Set the unsigned int value to the shader.
		 * @param name uniform name.
		 * @param value unsigned int value.
         */
        void SetUInt(const std::string& name, unsigned int value) const;

        /**
		 * @brief Set the float value to the shader.
		 * @param name uniform name.
		 * @param value float value.
         */
        void SetFl32(const std::string& name, float value) const;

        /**
		 * @brief Get the ID of the shader.
		 * @return ID of the shader.
         */
        unsigned int GetID() const { return ID; }

    };

    /**
	 * @brief Compute Shader Class.
     */
    class ComputeShader
    {
    private:
      unsigned int ID = 0; ///< ID of the shader.
      bool b_from_path = true; ///< If it is true, it will load the shader from the given path. If it is false, it will load the shader from the given string.
      char* compute_path_; ///< Compute shader path.

    public:
        /**
		 * @brief Compute Shader Constructor.
		 * @param compute_path compute shader path.
         */
        explicit ComputeShader(const char* compute_path, const bool is_from_path = true);
        ~ComputeShader() = default;

        /**
		 * @brief Compile the shader from the given path.
         */
        void CompileShader();

        /**
         * @brief Switch the compute shader to use.
         */
        void Use() const;

        /**
		 * @brief Set the mat4 value to the shader.
		 * @param name uniform name.
		 * @param mat mat4 value.
         */
        void SetMat4(const std::string& name, const glm::mat4& mat) const;

        /**
		 * @brief Set the vec3 value to the shader.
		 * @param name uniform name.
		 * @param vec vec3 value.
         */
        void SetVec3(const std::string& name, const glm::vec3 vec) const;

        /**
		 * @brief Set the ivec3 value to the shader.
		 * @param name uniform name.
		 * @param vec ivec3 value.
         */
        void SetIVec3(const std::string& name, const glm::ivec3 vec) const;

        /**
		 * @brief Set the vec4 value to the shader.
		 * @param name uniform name.
		 * @param vec vec4 value.
         */
        void SetVec4(const std::string& name, const glm::vec4 vec) const;

        /**
		 * @brief Set the ivec4 value to the shader.
         * @param name uniform name
		 * @param vec ivec4 value
         */
        void SetIVec4(const std::string& name, const glm::ivec4 vec) const;

        /**
		 * @brief Set the bool value to the shader.
		 * @param name uniform name
		 * @param value bool value
         */
        void SetBool(const std::string& name, bool value) const;

        /**
		 * @brief Set the int value to the shader.
		 * @param name uniform name
		 * @param value int value
         */
        void SetInt(const std::string& name, int value) const;

        /**
		 * @brief Set the unsigned int value to the shader.
		 * @param name uniform name
		 * @param value unsigned int value
         */
        void SetUInt(const std::string& name, unsigned int value) const;

        /**
		 * @brief Set the float value to the shader.
		 * @param name uniform name
		 * @param value float value
         */
        void SetFl32(const std::string& name, float value) const;

        /**
		 * @brief Get the ID of the shader.
		 * @return ID of the shader.
         */
        unsigned int GetID() const { return ID; }

    };
}

#endif // !RS_SHADER_H_